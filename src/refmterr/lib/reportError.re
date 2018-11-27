/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
module Make = (Styl: Stylish.StylishSig) => {
  module Printer = Printer.Make(Styl);

  open BetterErrorsTypes;
  open Helpers;
  open Printer;
  open Styl;

  let suggestifyList = suggestions =>
    suggestions |> List.map(sug => yellow("- " ++ sug));

  let refmttypeNewlineR = Re.Pcre.regexp({|\\n|});

  /*
  * Remove GADT cruft etc.
  */
  let gadtR = Re.Pcre.regexp({|\$[a-zA-Z0-9']*|});

  let replaceDollar = s =>
    s.[0] === '$' ?
      "thereExistsAType_" ++ String.sub(s, 1, String.length(s) - 1) : s;

  let normalizeType = s =>
    Re.Pcre.substitute(~rex=gadtR, ~subst=replaceDollar, s);

  let toReasonTypes = (~refmttypePath, types) =>
    switch (refmttypePath) {
    | None => types
    | Some(path) =>
      let types = concatList("\\\"", List.map(normalizeType, types));
      let cmd = path ++ (sp({| "%s"|}))(types);
      let input = Unix.open_process_in(cmd);
      let result = {contents: []};
      try (
        while (true) {
          result.contents = [
            Re.Pcre.substitute(
              ~rex=refmttypeNewlineR,
              ~subst=(_) => "\n",
              input_line(input),
            ),
            ...result.contents,
          ];
        }
      ) {
      | End_of_file => ignore(Unix.close_process_in(input))
      };
      List.rev(result^);
    };

  let toReasonTypes1 = (~refmttypePath, one) =>
    switch (toReasonTypes(~refmttypePath, [one])) {
    | [a] => a
    | _ => one
    };

  let toReasonTypes2 = (~refmttypePath, one, two) =>
    switch (toReasonTypes(~refmttypePath, [one, two])) {
    | [a, b] => (a, b)
    | _ => (one, two)
    };

  let highlightType = (typ, other, hl, diffHl) => {
    let (typPrefixLen, typSuffixLen, otherPrefixLen, otherSuffixLen) =
      try (findCommonEnds(typ, other)) {
      | _ => (0, 0, 0, 0)
      };
    let len = String.length(typ);
    let prefix = String.sub(typ, 0, typPrefixLen);
    let suffix = String.sub(typ, len - typSuffixLen, typSuffixLen);
    let mid = String.sub(typ, typPrefixLen, len - typPrefixLen - typSuffixLen);
    if (String.length(prefix)
        + String.length(mid)
        + String.length(suffix) === len) {
      hl(prefix) ++ diffHl(mid) ++ hl(suffix);
    } else {
      /* Computed the prefix/suffix incorrectly. */
      hl(typ);
    };
  };

  /*
  * Renders type equalities but also accepts "others" which are the
  * contradicting types which can be used to highlight the diff in the types
  * being printed.
  *
  * ~types: A list of type equivalencies.
  * ~others: Corresponding type equivalences for the contradicting type.
  * ~diffHl: How to highlight the diff portion between the two.
  */
  let rec renderTypeEquality = (~lines=[], ~types, ~others, hl, diffHl) =>
    switch (types, others) {
    | ([hd, ...tl], [opHd, ...opTl]) =>
      let indent = List.length(lines) > 0 ? "" : "";
      let lines = [
        indentStr(indent, highlightType(hd, opHd, hl, diffHl)),
        ...lines,
      ];
      renderTypeEquality(~lines, ~types=tl, ~others=opTl, hl, diffHl);
    | ([hd, ...tl], []) =>
      let indent = List.length(lines) > 0 ? "" : "";
      let lines = [
        indentStr(indent, highlightType(hd, "", hl, diffHl)),
        ...lines,
      ];
      renderTypeEquality(~lines, ~types=tl, ~others=[], hl, diffHl);
    | ([], _) => concatList(dim("\nEquals\n"), List.rev(lines))
    };

  let renderInequality = (~isDetail, ~actual, ~expected) => {
    /*
    * If there are more than one total incompatable messages, or if the one
    * and only incompat message has line breaks, we'll print the more detailed
    * version with some comforatable white space.
    */
    let diffBad = highlight(~color=red, ~bold=true, ~dim=false);
    let diffGood = highlight(~color=green, ~bold=true, ~dim=false);
    let bad = highlight(~color=red, ~bold=false, ~dim=false);
    let good = highlight(~color=green, ~bold=false, ~dim=false);
    let actualStr =
      renderTypeEquality(~types=actual, ~others=expected, bad, diffBad);
    let expectedStr =
      renderTypeEquality(~types=expected, ~others=actual, good, diffGood);
    let indent = "  ";
    let thisType = isDetail ? "The type:   " : "This type:";
    let expecting = isDetail ? "Contradicts:" : "Expecting:";
    let ret =
      if (List.length(actual) > 1
          || List.length(expected) > 1
          || List.exists(hasNewline, actual)
          || List.exists(hasNewline, expected)) {
        [
          indent ++ thisType,
          indentStr(indent ++ indent, actualStr),
          "",
          indent ++ expecting,
          indentStr(indent ++ indent, expectedStr),
          "",
        ];
      } else {
        [
          sp("%s %s", indent ++ thisType, actualStr),
          sp("%s %s", indent ++ expecting, expectedStr),
          "",
        ];
      };
    concatList("\n", ret);
  };

  let doubleUnder = Re.Pcre.regexp({|__|});

  let subDot = s => ".";

  /*
  * Often types are reported with separate type equalities listed for module
  * aliases but these are really not very helpful and more confusing than
  * anything. Let's filter these out.
  */
  let normalizeEquivalencies = strings =>
    switch (strings) {
    | [] => []
    | [hd] => [hd]
    | [hd, ...tl] =>
      let hdNoSpace = collapseSpacing(hd);
      let rest =
        List.filter(
          s => collapseSpacing(removeModuleAlias(s)) != hdNoSpace,
          tl,
        );
      [hd, ...rest];
    };

  let normalizeIncompat = (inc: incompat) => {
    actual: normalizeEquivalencies(inc.actual),
    expected: normalizeEquivalencies(inc.expected),
  };

  let normalizeIncompatibleType = incType => {
    ...incType,
    main: normalizeIncompat(incType.main),
    incompats: List.map(normalizeIncompat, incType.incompats),
  };

  let reportEscape = originalType => {
    let normalized = normalizeType(originalType);
    let _inferred = normalized != originalType;
    concatList(
      "\n",
      [
        purple(~bold=true, "Learn:")
        ++ highlight(
            ~bold=true,
            ~dim=true,
            " What does it mean for a type variable to \"escape\" its scope?",
          ),
        concatList(
          "\n",
          [
            "Important assumptions about the type are potentially being violated because",
            "values of that type are being passed outside the region (\"escaping\").",
            "The error you see is likely preventing a runtime error.",
            "",
            "These types that aren't allowed to escape are introduced when using GADTs or",
            "locally abstract types (when you see functions defined in these forms):",
            "",
            "  let myFunction : type a . a => a = ...",
            "  let myFunction = (type a, x: a) => ...",
            "",
            "If the names of these types are weird, then they're probably not ones defined",
            "in locally abstract types, but generated by type inference when using GADTs",
            "",
            "Some things that might solve the problem:",
            "- Avoid passing these values outside this local region",
            "- Wrap the value in a GADT making the escaping type \"existential\".",
            "- Annotate the function it escapes to with locally abstract to match your function.",
            "- Make sure locally abstract type annotations are polymorphic. They aren't by default.",
          ],
        )
        |> dim,
      ],
    );
  };

  /*
  * Algorithm for injecting highlighting:
  *
  * A = A' not compatible with B = B'
  * AA = AA' not compatible with BB = BB'
  * AAA = AAA' not compatible with BBB = BBB'
  *
  * Either AA or AA' will be in at least one of A or A'.
  * Either BB or BB' will be in at least one of B or B'.
  * And so on.
  *
  * 1. Find the first point where A and B differ.
  *    Find the first point where A' and B' differ.
  *
  * We try to fold AAA/BBB or their primes into AA/BB or their primes just by
  * highlighting.
  * If not, we show AAA/BBB.
  *
  * We try to fold AA/BB into A/B just by highlighting.
  * If not, we show AA/BB.
  */
  let report = (~refmttypePath, parsedContent) : list(string) => {
    let toReasonTypes = toReasonTypes(~refmttypePath);
    let toReasonTypes1 = toReasonTypes1(~refmttypePath);
    let toReasonTypes2 = toReasonTypes2(~refmttypePath);
    /*
    * For some reason refmttype outputs the string (backslash followed by n)
    * instead of actual newlines.
    */
    switch (parsedContent) {
    | NoErrorExtracted => []
    | Type_MismatchTypeArguments({typeConstructor, expectedCount, actualCount}) => [
        sp(
          "You must pass exactly %d argument%s to this variant. You have passed %d argument%s.",
          expectedCount,
          expectedCount == 1 ? "" : "s",
          actualCount,
          actualCount == 1 ? "" : "s",
        ),
      ]
    | Type_IncompatibleType(incompatibleType) =>
      let {term, extra, main, incompats, escapedScope} =
        normalizeIncompatibleType(incompatibleType);
      let expected = toReasonTypes(main.expected);
      let actual = toReasonTypes(main.actual);
      /*
      * If there are more than one total incompatable messages, or if the one
      * and only incompat message has line breaks, we'll print the more detailed
      * version with some comforatable white space.
      */
      let termStr = term === Pattern ? "This pattern" : "This";
      let typeInequality =
        renderInequality(~isDetail=false, ~expected, ~actual);
      let main =
        switch (escapedScope) {
        | None => [
            bold(termStr ++ " type doesn't match what is expected."),
            "",
            typeInequality,
          ]
        | Some(t) => [
            "",
            bold(termStr ++ " allows type ")
            ++ red(~underline=true, ~bold=true, normalizeType(t))
            ++ bold(" to \"escape\" its scope."),
            "",
            "",
            typeInequality,
            reportEscape(t),
          ]
        };
      let mainStr = concatList("\n", main);
      let withoutExtra =
        switch (incompats) {
        | [] => [mainStr]
        | [_, ..._] =>
          let incompatLines =
            List.map(
              (inc: incompat) => {
                let expected = toReasonTypes(inc.expected);
                let actual = toReasonTypes(inc.actual);
                renderInequality(~isDetail=true, ~expected, ~actual);
              },
              incompats,
            )
            |> concatList("\n\n");
          [
            incompatLines,
            "",
            bold(
              "The contradicting part"
              ++ (List.length(incompats) > 1 ? "s:" : ":"),
            ),
            "",
            mainStr,
          ];
        };
      extra == "" ? withoutExtra : ["Extra info: " ++ extra, ...withoutExtra];
    | Type_NotAFunction({actual}) =>
      let actual = toReasonTypes1(actual);
      [
        "This has type " ++ actual ++ ", but you are calling it as a function.",
        "Perhaps you have forgoten a semicolon, or a comma somewhere.",
      ];
    | Type_AppliedTooMany({functionType, expectedArgCount}) =>
      let functionType = toReasonTypes1(functionType);
      [
        sp(
          "It accepts only %d arguments. You gave more. Maybe you forgot a ; somewhere?",
          expectedArgCount,
        ),
        sp("This function has type %s", functionType),
      ];
    | Type_FunctionWrongLabel({functionType, labelIssue}) =>
      let functionType = toReasonTypes1(functionType);
      let labelIssueStr =
        switch (labelIssue) {
        | HasOptionalLabel(str) =>
          sp(
            "%s %s",
            bold("But its first argument is an optional named"),
            red(~bold=true, "~" ++ str ++ "?"),
          )
        | HasLabel(str) =>
          sp(
            "%s %s",
            bold("But its first argument is named"),
            red(~bold=true, "~" ++ str),
          )
        | HasNoLabel =>
          sp(
            "%s %s",
            bold("But its first argument"),
            red(~bold=true, "is not named"),
          )
        | Unknown =>
          bold("There appears to be something wrong with the named arguments")
        };
      [
        labelIssueStr,
        "",
        highlight(~color=green, functionType),
        bold("This function should have type:"),
      ];
    | File_SyntaxError({offendingString, hint}) => [
        "Note: the location indicated might not be accurate.",
        switch (offendingString) {
        | ";" => "Make sure all imperative statements, as well as let/type bindings have exactly one semicolon separating them."
        | "else" =>
          "Did you happen to have put a semicolon on the line before else?"
          ++ " Also, `then` accepts a single expression. If you've put many, wrap them in parentheses."
        | _ => ""
        },
        switch (hint) {
        | Some(a) => "This is a syntax error: " ++ a
        | None => "This is a syntax error."
        },
      ]
    | File_IllegalCharacter({character}) => [
        sp("The character `%s` is illegal.", character),
      ]
    | Type_UnboundTypeConstructor({namespacedConstructor, suggestion}) =>
      let namespacedConstructor = toReasonTypes1(namespacedConstructor);
      let main =
        sp(
          "The type %s can't be found.",
          red(~bold=true, namespacedConstructor),
        );
      switch (suggestion) {
      | None => [main]
      | Some(h) => [sp("Hint: did you mean %s?", yellow(h)), "", main]
      };
    | Type_ArgumentCannotBeAppliedWithLabel({functionType, attemptedLabel}) =>
      let formattedFunctionType = toReasonTypes1(functionType);
      [
        sp(
          "This function doesn't accept an argument named ~%s.",
          attemptedLabel,
        ),
        "",
        sp("The function has type %s", formattedFunctionType),
      ];
    | Type_UnboundValue({unboundValue, suggestions}) =>
      switch (suggestions) {
      | None => [
          sp(
            "The value named %s can't be found. Could it be a typo?",
            red(~bold=true, unboundValue),
          ),
        ]
      | Some([hint]) => [
          sp(
            "The value named %s can't be found. Did you mean %s?",
            red(~bold=true, unboundValue),
            yellow(hint),
          ),
        ]
      | Some([hint1, hint2]) => [
          sp(
            "%s can't be found. Did you mean %s or %s?",
            red(~bold=true, unboundValue),
            yellow(hint1),
            yellow(hint2),
          ),
        ]
      | Some(hints) =>
        List.concat([
          suggestifyList(hints),
          [
            sp(
              "%s can't be found. Did you mean one of these?",
              red(~bold=true, unboundValue),
            ),
          ],
        ])
      }
    | Type_UnboundRecordField({recordField, suggestion}) =>
      let recordField = toReasonTypes1(recordField);
      let main =
        switch (suggestion) {
        | None =>
          sp(
            "Record field %s can't be found in any record type.",
            red(~bold=true, recordField),
          )
        | Some(hint) =>
          sp(
            "Record field %s can't be found in any record type. Did you mean %s?",
            red(~bold=true, recordField),
            yellow(hint),
          )
        };
      [
        "Alternatively, instead of opening a module, you can prefix the record field name like {TheModule.x: 0, y: 100}.",
        "Record fields must be \"in scope\". That means you need to `open TheModule` where the record type is defined.",
        "",
        main,
      ];
    | Type_RecordFieldNotBelongPattern({
        term,
        recordType,
        recordField,
        suggestion,
      }) =>
      let recordType = toReasonTypes1(recordType);
      let termStr = term === Expression ? "expression" : "pattern";
      let main = [
        "",
        sp(
          "%s %s %s",
          bold("This field"),
          red(~bold=true, recordField),
          bold("doesn't belong to the record."),
        ),
      ];
      let main =
        hasNewline(recordType) ?
          [
            bold(recordType),
            "",
            bold("The record " ++ termStr ++ " has type:"),
            ...main,
          ] :
          [
            bold("The record " ++ termStr ++ " has type: " ++ recordType),
            ...main,
          ];
      switch (suggestion) {
      | None => main
      | Some(hint) => [sp("Did you mean %s?", yellow(hint)), "", ...main]
      };
    | Type_SomeRecordFieldsUndefined(recordField) => [
        "record is of some other type - one that does have a "
        ++ bold(recordField)
        ++ " field. Where else is it used?",
        "if you use this record in a way that would make the type checker think this",
        "If you are certain this record shouldn't have a field named "
        ++ bold(recordField)
        ++ " then check ",
        "",
        sp(
          "You forgot to include the record field named %s.",
          red(~bold=true, recordField),
        ),
      ]
    | Type_UnboundModule({unboundModule, suggestion}) =>
      let unboundModule = toReasonTypes1(unboundModule);
      let main =
        sp(
          "Module %s not found in included libraries.\n",
          red(~bold=true, unboundModule),
        );
      switch (suggestion) {
      | Some(s) => [sp("Hint: did you mean %s?", yellow(s)), main]
      | None => [
          sp(
            " - ocamlbuild: make sure you have `-pkgs libraryName` in your build command.",
          ),
          " - ocamlfind: make sure you have `-package libraryName -linkpkg` in your build command.",
          sp(
            " - For jbuilder: make sure you include the library that contains %s in your jbuild file's (libraries ...) section.",
            unboundModule,
          ),
          "You can see which libraries are available by doing `ocamlfind list` (or `esy ocamlfind list` inside your esy project)",
          sp(
            "Hint: You might need to tell your build system to depend on a library that contains %s.",
            unboundModule,
          ),
          main,
        ]
      };
    | Type_SignatureItemMismatch({missing, values, types, notes: _}) =>
      let finalMessage =
        switch (missing, values, types) {
        | ([], [], []) => "This module doesn't match its signature. See the original error output"
        | ([hd, ...tl], _, _) =>
          concatList(
            "\n",
            [
              "",
              sp(
                "%s %s",
                purple(~bold=true, "Learn:"),
                "\"Signatures\" are interfaces that modules may implement.",
              ),
              "  You can indicate that a Reason file (.re) implements a signature by",
              "  creating an \"interface file\" (.rei) of the same name.",
              "  Modules nested inside of a Reason file may also opt into being checked",
              "  against any signature using the type constraint syntax `module M : Sig = ...`",
            ],
          )
        | _ => ""
        };
      let whatStr = (
        fun
        | Type => "type"
        | Value => "value"
      );
      let missingMsg = ((what, named, declaredAtFile, declaredAtLine)) =>
        concatList(
          "\n",
          [
            sp(
              "%s %s",
              bold("This module is missing the " ++ whatStr(what) ++ " named"),
              red(~bold=true, named),
            ),
            "",
            sp(
              "  The following signature requires that %s be defined:",
              bold(named),
            ),
            sp("  %s%s", cyan(declaredAtFile), dim(":" ++ declaredAtLine)),
            "",
          ],
        );
      let badValueMsg = info => {
        let (what, named, good, goodFile, goodLn, badName, bad, badFile, badLn) = info;
        let (bad, good) = toReasonTypes2(bad, good);
        concatList(
          "\n",
          [
            sp(
              "%s %s %s %s",
              bold("This module doesn't match its signature because the"),
              bold(whatStr(what)),
              red(~bold=true, named),
              bold("has the wrong type"),
            ),
            "",
            sp("  %s %s%s", "At", cyan(goodFile), dim(":" ++ goodLn)),
            sp("  the signature required that %s be of type:", bold(named)),
            "",
            highlight(~bold=true, ~color=green, indentStr("  ", good)),
            "",
            "",
            sp("  %s %s%s", "At", cyan(badFile), dim(":" ++ badLn)),
            sp("  your module defined %s having type:", bold(named)),
            "",
            highlight(~bold=true, ~color=red, indentStr("  ", bad)),
            "",
          ],
        );
      };
      let badTypeMsg = info => {
        let (good, goodFile, goodLn, bad, badFile, badLn, arity) = info;
        let (bad, good) = toReasonTypes2(bad, good);
        concatList(
          "\n",
          [
            arity ?
              bold(
                "This module contains a type definition with the wrong number of type parameters ",
              ) :
              bold(
                "This module contains a type definition that contradicts its signature",
              ),
            "",
            sp("  %s %s%s", "At", cyan(goodFile), dim(":" ++ goodLn)),
            "  the signature required that the type be defined as:",
            "",
            "  " ++ highlight(~bold=true, ~color=green, good),
            "",
            "",
            sp("  %s %s%s", "At", cyan(badFile), dim(":" ++ badLn)),
            "  your module defined the type to be:",
            "",
            "  " ++ highlight(~bold=true, ~color=red, bad),
            "",
          ],
        );
      };
      let missingString = List.map(missingMsg, missing);
      let badValueString = List.map(badValueMsg, values);
      let badTypeString = List.map(badTypeMsg, types);
      List.concat([
        [finalMessage],
        missingString,
        badValueString,
        badTypeString,
      ]);
    | _ => ["Error beautifier not implemented for this."]
    };
  };
};
