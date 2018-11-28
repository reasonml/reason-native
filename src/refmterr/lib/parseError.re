/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open BetterErrorsTypes;

open Helpers;

/* agnostic extractors, turning err string into proper data structures */
/* TODO: don't make these raise error */
/* get the diffing portion of two incompatible types, columns are 0-indexed */
let stripCommonPrefix = ((l1, l2)) => {
  let i = ref(0);
  while (i^ < List.length(l1)
         && i^ < List.length(l2)
         && List.nth(l1, i^) == List.nth(l2, i^)) {
    i := i^ + 1;
  };
  (Helpers.listDrop(i^, l1), Helpers.listDrop(i^, l2));
};

let applyToBoth = (f, (a, b)) => (f(a), f(b));

let splitEquivalentTypes = raw =>
  switch (Helpers.stringSplit(raw, ~by="=")) {
  | exception Not_found => [String.trim(raw)]
  | (x, y) => [String.trim(x), String.trim(y)]
  };

let functionArgsCount = str => {
  /* the func type 'a -> (int -> 'b) -> string has 2 arguments */
  /* strip out false positive -> from nested function types passed as param */
  /* Fortunately, raw ocaml types don't use parenthesis for anything. */
  let nestedFunctionTypeR = Re.Pcre.regexp({|\([\s\S]+\)|});
  let cleaned =
    Re.Pcre.substitute(~rex=nestedFunctionTypeR, ~subst=(_) => "|||||", str);
  /* TODO: allow pluggable function type syntax */
  List.length(split({|->|}, cleaned)) - 1;
};

/* need: where the original expected comes from  */
/* TODO: when it's a -> b vs b, ask if whether user forgot an argument to the
   func */
let incompatR =
  Re.Pcre.regexp({|([\s\S]*?)is not compatible with type([\s\S]*)|});

let typeIncompatsR = Re.Pcre.regexp({|\s*\s\sType|});

let extractTypeIncompatsFromExtra = extra => {
  /*
   * Need to add back the two spaces before the first Type to make sure it
   * actually gets detected in this regex for the first occurence. It was too
   * hard to make the original regex capture the two leading spaces.
   */
  let extraWithTwoSpaces = "  " ++ extra;
  let splitted = Re.Pcre.full_split(~rex=typeIncompatsR, extraWithTwoSpaces);
  let folder = ((curOther, curIncompats), next) =>
    switch (next) {
    | Re.Pcre.Text(str) =>
      let (incompatA, incompatB) = (
        Re.Pcre.get_substring(Re.Pcre.exec(~rex=incompatR, str), 1),
        Re.Pcre.get_substring(Re.Pcre.exec(~rex=incompatR, str), 2),
      );
      let incompat = {
        actual: splitEquivalentTypes(incompatA),
        expected: splitEquivalentTypes(incompatB),
      };
      (curOther, [incompat, ...curIncompats]);
    | _ => (curOther, curIncompats)
    };
  let (remainingExtra, incompats) =
    List.fold_left(folder, ("", []), splitted);
  (remainingExtra, List.rev(incompats));
};

let type_IncompatibleType = (err, _, range) => {
  /* the type actual and expected might be on their own line */
  /* sometimes the error msg might equivalent types, e.g. "myType = string isn't
     compatible to bla" */
  let escapeScopeR = {|The type constructor([\s\S]*)?would escape its scope|};
  let allR =
    /* This regex query is brought to you by debuggex.com. Get your free
       real-time regex visualization today. */
    {|This (expression has type|pattern matches values of type)([\s\S]*?)|}
    ++ {|but (an expression was expected of type|a pattern was expected which matches values of type)([\s\S]*?)|}
    ++ {|(Type\b([\s\S]*?))?|}
    ++ {|(The type constructor([\s\S])*?would escape its scope)?|}
    ++ {|(The type variable[\s\S]*occurs inside([\s\S])*)?$|};
  let extraRaw = get_match_n_maybe(5, allR, err);
  let escapedScope =
    switch (get_match_n_maybe(7, allR, err)) {
    | None => None
    | Some(s) => Some(String.trim(get_match_n(1, escapeScopeR, s)))
    };
  let (extra, incompats) =
    switch (extraRaw) {
    | Some(a) =>
      if (String.trim(a) == "") {
        ("", []);
      } else {
        let extra = String.trim(a);
        extractTypeIncompatsFromExtra(extra);
      }
    | None => ("", [])
    };
  let term =
    get_match_n(1, allR, err) == "expression has type" ? Expression : Pattern;
  let actualRaw = get_match_n(2, allR, err);
  let expectedRaw = get_match_n(4, allR, err);
  let main = {
    actual: splitEquivalentTypes(actualRaw),
    expected: splitEquivalentTypes(expectedRaw),
  };
  Type_IncompatibleType({term, main, incompats, extra, escapedScope});
};

/* TODO: differing portion data structure a-la diff table */
let type_MismatchTypeArguments = (err, _, _) => {
  let allR = {|The constructor ([\w\.]*) *expects[\s]*(\d+) *argument\(s\),\s*but is applied here to (\d+) argument\(s\)|};
  let typeConstructor = get_match_n(1, allR, err);
  let expectedCount = int_of_string(get_match_n(2, allR, err));
  let actualCount = int_of_string(get_match_n(3, allR, err));
  Type_MismatchTypeArguments({typeConstructor, expectedCount, actualCount});
};

/* need: if it's e.g. a module function, which part is not found? Module?
   Function? */
let type_UnboundValue = (err, _, _) => {
  let unboundValueR = {|Unbound value ([\w\.]*)|};
  let unboundValue = get_match(unboundValueR, err);
  /* TODO: there might be more than one suggestion */
  let suggestionR = {|Unbound value [\w\.]*[\s\S]Hint: Did you mean ([\s\S]+)\?|};
  let suggestions =
    get_match_maybe(suggestionR, err)
    |> Helpers.optionMap(Re.Pcre.split(~rex=Re.Pcre.regexp({|, | or |})));
  Type_UnboundValue({unboundValue, suggestions});
};

let type_SignatureMismatch = (err, cachedContent) => raise(Not_found);

/*
 * The compiler will report missing items first before reporting mis-typed
 * items. It will report multiple missing items at once. When types are wrong:
 * It only reports one type mismatch.
 */
let wrongValR =
  {|Values do not match:[\s\S]*?|}
  ++ {|val ([a-z_][a-zA-Z0-9_\$\']+) :([\s\S]*?) is not included in[\s\S]*?|}
  ++ {|val ([a-z_][a-zA-Z0-9_\$\']+) :([\s\S]*?)|}
  ++ {|File "([\s\S]*?)", line ([0-9]*)[\s\S]*Expected declaration[\s\S]*?|}
  ++ {|File "([\s\S]*?)", line ([0-9]*)[\s\S]*Actual declaration[\s\S]*?|};

let wrongTypeR =
  {|Type declarations do not match:[\s\S]*?|}
  ++ {|([\s\S]*?)is not included in([\s\S]*)?|}
  ++ {|File "([\s\S]*?)", line ([0-9]*)[\s\S]*Expected declaration[\s\S]*?|}
  /* You can't have two "?" backtracking regexes next to each other without
   * anything in between, so we have to make the match after "Actual
   * declaration" only match white space.
   */
  ++ {|File "([\s\S]*?)", line ([0-9]*)[\s\S]*?Actual declaration[\s]*|}
  ++ {|(They have different arities)?|};

let fallbackSignatureMismatch =
  Type_SignatureItemMismatch({
    notes: "Could not extract error",
    types: [],
    values: [],
    missing: [],
  });

let type_SignatureItemMismatch = (err, cachedContent, _) => {
  let sigMismatchR = {|(Signature mismatch:([\s\S]*)|The implementation[\s\S]*does not match the interface([\s\S]*))|};
  let rest = get_match(sigMismatchR, err);
  let missingItemsR = {|[\s\S]*?(The [a-zA-Z]+ `[a-z_][a-zA-Z0-9_\$\']+' is required but not provided[\s\S]*)|};
  let missingItemsMatch = get_match_maybe(missingItemsR, rest);
  let wrongValMatch = get_match_maybe(wrongValR, rest);
  let wrongTypeMatch = get_match_maybe(wrongTypeR, rest);
  switch (missingItemsMatch, wrongValMatch, wrongTypeMatch) {
  | (Some(missingItemsText), _, _) =>
    let missingItems =
      Helpers.stringNsplit(missingItemsText, ~by="Expected declaration");
    let missing = {contents: []};
    let missingItemR = {|[\s\S]*The ([a-zA-Z]+) `([a-z_][a-zA-Z0-9_\$\']*)' is required but not provided[\s\S]*?File "([\s\S]*?)", line ([0-9]*)|};
    for (i in 0 to List.length(missingItems) - 1) {
      let itm = List.nth(missingItems, i);
      if (String.length(itm) > 0) {
        switch (
          get_match_n_maybe(1, missingItemR, itm),
          get_match_n_maybe(2, missingItemR, itm),
          get_match_n_maybe(3, missingItemR, itm),
          get_match_n_maybe(4, missingItemR, itm),
        ) {
        | (Some(fileOrType), Some(name), Some(fileName), Some(line)) =>
          let what = fileOrType == "type" ? Type : Value;
          missing.contents = [
            (what, name, fileName, line),
            ...missing.contents,
          ];
        | _ => ()
        };
      };
    };
    Type_SignatureItemMismatch({
      types: [],
      notes: "extracted error from missing items",
      values: [],
      missing: List.rev(missing.contents),
    });
  | (None, Some(_), None) =>
    let badName = get_match_n_maybe(1, wrongValR, rest);
    let bad = get_match_n_maybe(2, wrongValR, rest);
    let goodName = get_match_n_maybe(3, wrongValR, rest);
    let good = get_match_n_maybe(4, wrongValR, rest);
    let goodFile = get_match_n_maybe(5, wrongValR, rest);
    let goodLn = get_match_n_maybe(6, wrongValR, rest);
    let badFile = get_match_n_maybe(7, wrongValR, rest);
    let badLn = get_match_n_maybe(8, wrongValR, rest);
    switch (goodName, good, badName, bad, goodFile, goodLn, badFile, badLn) {
    | (
        Some(goodName),
        Some(good),
        Some(badName),
        Some(bad),
        Some(goodFile),
        Some(goodLn),
        Some(badFile),
        Some(badLn),
      ) =>
      Type_SignatureItemMismatch({
        notes: "Successfully extracted error",
        types: [],
        values: [
          (
            Value,
            String.trim(goodName),
            String.trim(good),
            goodFile,
            goodLn,
            String.trim(badName),
            String.trim(bad),
            badFile,
            badLn,
          ),
        ],
        missing: [],
      })
    | _ => fallbackSignatureMismatch
    };
  | (None, None, Some(_)) =>
    let bad = get_match_n_maybe(1, wrongTypeR, rest);
    let good = get_match_n_maybe(2, wrongTypeR, rest);
    let goodFile = get_match_n_maybe(3, wrongTypeR, rest);
    let goodLn = get_match_n_maybe(4, wrongTypeR, rest);
    let badFile = get_match_n_maybe(5, wrongTypeR, rest);
    let badLn = get_match_n_maybe(6, wrongTypeR, rest);
    let arityStatement = get_match_n_maybe(7, wrongTypeR, rest);
    switch (good, bad, goodFile, goodLn, badFile, badLn) {
    | (
        Some(good),
        Some(bad),
        Some(goodFile),
        Some(goodLn),
        Some(badFile),
        Some(badLn),
      ) =>
      Type_SignatureItemMismatch({
        notes: "Successfully extracted type definition mismatch",
        /* good, goodFile, goodln, bad, badFile, badln, arity */
        values: [],
        types: [
          (
            String.trim(good),
            String.trim(goodFile),
            String.trim(goodLn),
            String.trim(bad),
            String.trim(badFile),
            String.trim(badLn),
            arityStatement !== None,
          ),
        ],
        missing: [],
      })
    | _ => fallbackSignatureMismatch
    };
  | _ => fallbackSignatureMismatch
  };
  /* let problemsR = {|(The (value\|type))([\s\S]*)|}; */
  /* let problems = get_match(problemsR, err); */
};

let type_UnboundModule = (err, _, _) => {
  let unboundModuleR = {|Unbound module ([\w\.]*)|};
  let unboundModule = get_match(unboundModuleR, err);
  let suggestionR = {|Unbound module [\w\.]*[\s\S]Hint: Did you mean (\S+)\?|};
  let suggestion = get_match_maybe(suggestionR, err);
  Type_UnboundModule({unboundModule, suggestion});
};

/* need: if there's a hint, show which record type it is */
let type_UnboundRecordField = (err, _, _) => {
  let recordFieldR = {|Unbound record field (\w+)|};
  let recordField = get_match(recordFieldR, err);
  let suggestionR = {|Hint: Did you mean (\w+)\?|};
  let suggestion = get_match_maybe(suggestionR, err);
  Type_UnboundRecordField({recordField, suggestion});
};

let type_RecordFieldNotBelongPattern = (err, _, _) => {
  let expressionTypeR = {|This (expression has type|record expression is expected to have type|record pattern is expected to have type)([\s\S]*)The field|};
  let term =
    switch (get_match_n(1, expressionTypeR, err)) {
    | "expression has type"
    | "record expression is expected to have type" => Expression
    | "record pattern is expected to have type" => Pattern
    | _ => Expression
    };
  let recordFieldR = {|The field (\w+) does not belong to type|};
  let recordType = get_match_n(2, expressionTypeR, err);
  let recordField = get_match(recordFieldR, err);
  let suggestionR = {|Hint: Did you mean (\w+)\?|};
  let suggestion = get_match_maybe(suggestionR, err);
  Type_RecordFieldNotBelongPattern({
    term,
    recordType,
    recordField,
    suggestion,
  });
};

let type_SomeRecordFieldsUndefined = (err, _, _) => {
  let recordFieldR = {|Some record fields are undefined: (\w+)|};
  let recordField = get_match(recordFieldR, err);
  Type_SomeRecordFieldsUndefined(recordField);
};

let type_UnboundConstructor = (err, cachedContent) => raise(Not_found);

let type_UnboundTypeConstructor = (err, _, _) => {
  let constructorR = {|Unbound type constructor ([\w\.]+)|};
  let constructor = get_match(constructorR, err);
  let suggestionR = {|Hint: Did you mean ([\w\.]+)\?|};
  let suggestion = get_match_maybe(suggestionR, err);
  Type_UnboundTypeConstructor({
    namespacedConstructor: constructor,
    suggestion,
  });
};

/* need: number of arguments actually applied to it, and what they are */
/* need: number of args the function asks, and what types they are */
let type_AppliedTooMany = (err, _, _) => {
  let functionTypeR = {|This function has type([\s\S]+)It is applied to too many arguments; maybe you forgot a `;'.|};
  let functionType = String.trim(get_match(functionTypeR, err));
  Type_AppliedTooMany({
    functionType,
    expectedArgCount: functionArgsCount(functionType),
  });
};

let type_FunctionWrongLabel = (err, _, _) => {
  let functionLabelR = {|This function should have type([\s\S]+)but its first argument is([\s\S]*)|};
  let functionType = String.trim(get_match_n(1, functionLabelR, err));
  let labelIssueString = String.trim(get_match_n(2, functionLabelR, err));
  /**
   * labelled ?a
   * labelled ~a
   * not labelled
   */
  let labelledOptionalR = {|labelled \?([a-z][a-zA-Z0-9_]*)|};
  let labelledR = {|labelled ~([a-z][a-zA-Z0-9_]*)|};
  let notLabelledR = {|(not labelled)|};
  let labelIssue =
    switch (
      get_match_n_maybe(1, labelledOptionalR, labelIssueString),
      get_match_n_maybe(1, labelledR, labelIssueString),
      get_match_n_maybe(1, notLabelledR, labelIssueString),
    ) {
    | (Some(lbl), _, _) => HasOptionalLabel(lbl)
    | (_, Some(lbl), _) => HasLabel(lbl)
    | (_, _, Some(lbl)) => HasNoLabel
    | _ => Unknown
    };
  Type_FunctionWrongLabel({functionType, labelIssue});
};

let type_ArgumentCannotBeAppliedWithLabel = (err, cachedContent, range) => {
  let functionTypeR = {|The function applied to this argument has type([\s\S]+)This argument cannot be applied with label|};
  let attemptedLabelR = {|This argument cannot be applied with label ~([a-z_][a-zA-Z0-9_\$]+)|};
  let functionType = String.trim(get_match(functionTypeR, err));
  let attemptedLabel = String.trim(get_match(attemptedLabelR, err));
  Type_ArgumentCannotBeAppliedWithLabel({functionType, attemptedLabel});
};

let type_RecordFieldNotInExpression = (err, cachedContent, range) =>
  raise(Not_found);

let type_RecordFieldError = (err, cachedContent, range) => raise(Not_found);

let type_FieldNotBelong = (err, cachedContent, range) => raise(Not_found);

let type_NotAFunction = (err, _, range) => {
  let actualR = {|This expression has type([\s\S]+)This is not a function; it cannot be applied.|};
  let actual = String.trim(get_match(actualR, err));
  Type_NotAFunction({actual: actual});
};

let type_UnboundModule = (err, _, _) => {
  let unboundModuleR = {|Unbound module ([\w\.]*)|};
  let unboundModule = get_match(unboundModuleR, err);
  let suggestionR = {|Unbound module [\w\.]*[\s\S]Hint: Did you mean (\S+)\?|};
  let suggestion = get_match_maybe(suggestionR, err);
  Type_UnboundModule({unboundModule, suggestion});
};

/* TODO: apparently syntax error can be followed by more indications */
/* need: way, way more information, I can't even */
let file_SyntaxError = (err, cachedContent, range) => {
  let allR = Re.Pcre.regexp({|Syntax error|});
  let allUnknownR = Re.Pcre.regexp({|<UNKNOWN SYNTAX ERROR>|});
  /* raise the same error than if we failed to match */
  if (! Re.Pcre.pmatch(~rex=allR, err)
      && ! Re.Pcre.pmatch(~rex=allUnknownR, err)) {
    raise(Not_found);
  } else {
    let hintR = {|Syntax error:([\s\S]+)|};
    let hint = get_match_maybe(hintR, err);
    /* assuming on the same row */
    let ((startRow, startColumn), (_, endColumn)) = range;
    File_SyntaxError({
      hint: Helpers.optionMap(String.trim, hint),
      offendingString:
        Helpers.stringSlice(
          ~first=startColumn,
          ~last=endColumn,
          List.nth(cachedContent, startRow),
        ),
    });
  };
};

let build_InconsistentAssumptions = (err, cachedContent, range) =>
  raise(Not_found);

/* need: list of legal characters */
let file_IllegalCharacter = (err, _, _) => {
  let characterR = {|Illegal character \(([\s\S]+)\)|};
  let character = get_match(characterR, err);
  File_IllegalCharacter({character: character});
};

let parsers = [
  type_MismatchTypeArguments,
  type_UnboundValue,
  type_SignatureMismatch,
  type_SignatureItemMismatch,
  type_UnboundModule,
  type_UnboundRecordField,
  type_RecordFieldNotBelongPattern,
  type_SomeRecordFieldsUndefined,
  type_UnboundConstructor,
  type_UnboundTypeConstructor,
  type_AppliedTooMany,
  type_FunctionWrongLabel,
  type_ArgumentCannotBeAppliedWithLabel,
  type_RecordFieldNotInExpression,
  type_RecordFieldError,
  type_FieldNotBelong,
  type_IncompatibleType,
  type_NotAFunction,
  file_SyntaxError,
  build_InconsistentAssumptions,
  file_IllegalCharacter,
];

let goodFileNameR = Re.Pcre.regexp({|^[a-zA-Z][a-zA-Z_\d]+\.\S+$|});

let cannotFindFileRStr = {|Cannot find file ([\s\S]+)|};

let unboundModuleRStr = {|Unbound module ([\s\S]+)|};

/* not pluggable yet (unlike `customErrorParsers` below)  */
/* TODO: this doesn't work. What did I say about testing... */
let specialParserThatChecksWhetherFileEvenExists = (filePath, errorBody) =>
  switch (filePath) {
  | "_none_" =>
    switch (errorBody) {
    | None => None /* unrecognized? We're mainly trying to catch the case below */
    | Some(err) =>
      switch (get_match_maybe(cannotFindFileRStr, err)) {
      | None => None /* unrecognized again? We're mainly trying to catch the case below */
      | Some(fileName) => Some(ErrorFile(NoneFile(fileName)))
      }
    }
  | "command line" =>
    switch (errorBody) {
    | None => None /* unrecognized? We're mainly trying to catch the case below */
    | Some(err) =>
      switch (get_match_maybe(unboundModuleRStr, err)) {
      | None => None /* unrecognized? We're mainly trying to catch the case below */
      | Some(moduleName) => Some(ErrorFile(CommandLine(moduleName)))
      }
    }
  | "(stdin)" =>
    /* piping into `utop -stdin`. Can't really indicate better errors here as we can't read into stdin
       again */
    switch (errorBody) {
    | None => None /* unrecognized? We're mainly trying to catch the case below */
    | Some(err) => Some(ErrorFile(Stdin(err)))
    }
  | _ => None
  };

let parse = (~customErrorParsers, ~errorBody, ~cachedContent, ~range) =>
  /* custom parsers go first */
  try (
    Helpers.listFindMap(
      parse =>
        try (Some(parse(errorBody, cachedContent, range))) {
        | _ => None
        },
      List.append(customErrorParsers, parsers),
    )
  ) {
  | Not_found => NoErrorExtracted
  };
