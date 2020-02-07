/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open CommonCollections;

[@ocaml.warning "-30"];

module Types = {
  type kind =
    | String
    | Int
    | Bool
    | Optional(kind);
  /* TODO:
     | Float
     | List(kind)
     | Repeatable(kind);
     */
  type value =
    | EmptyValue
    | InvalidValue(string)
    | StringValue(string)
    | IntValue(int)
    | BoolValue(bool);
  type container = {
    string: string => string,
    int: string => int,
    bool: string => bool,
    optionalString: string => option(string),
    optionalInt: string => option(int),
  };
  type logger = {
    debug: string => unit,
    info: string => unit,
    warn: string => unit,
    error: string => unit,
  };
  type argument = {
    name: string,
    desc: string,
    kind,
  };
  type cliOption = {
    automatic: bool,
    name: string,
    desc: string,
    aliases: StringSet.t,
    kind,
  };
  type actionFn = self => unit
  and self = {
    program,
    log: logger,
    options: container,
    args: container,
  }
  and command = {
    automatic: bool,
    name: string,
    aliases: StringSet.t,
    desc: option(string),
    arguments: list(argument),
    options: list(cliOption),
    usedOptionNames: StringMap.t(cliOption),
    action: option(actionFn),
  }
  and program = {
    name: string,
    desc: option(string),
    version: option(string),
    commands: StringMap.t(command),
    preparing: option(command),
    usedCommandNames: StringMap.t(command),
  };
};

open Types;

module Utils = {
  let defaultCommandName = "__DEFAULT_COMAND_NAME__";
  let maybePromoteDescrption = (program: program): program => {
    ();
    switch (program) {
    /*
     * If there is only one un-named command it represents the program. Here
     * we promote the description from that command to the program level if
     * necessary.
     */
    | {desc: None, commands} when StringMap.has(defaultCommandName, commands) => {
        ...program,
        desc:
          CommonOption.valuex(StringMap.getOpt(defaultCommandName, commands)).
            desc,
      }
    | _ => program
    };
  };
  let finalizePreparing = (program: program): program => {
    let program =
      switch (program) {
      | {preparing: Some(command)} =>
        /* Update the program with the new command */
        let program = {
          ...program,
          commands: StringMap.set(command.name, command, program.commands),
          usedCommandNames:
            program.usedCommandNames
            |> StringMap.set(command.name, command)
            |> (
              map =>
                StringSet.reduce(
                  (acc, alias) => StringMap.set(alias, command, acc),
                  map,
                  command.aliases,
                )
            ),
          preparing: None,
        };
        program;
      | {preparing: None} => program
      };
    program;
  };
  let automaticOptionNotice = (automatic, conflictName) =>
    automatic
      ? " (Note that option " ++ conflictName ++ " is automatically added)"
      : "";
  let automaticCommandNotice = (automatic, conflictName) =>
    automatic
      ? " (Note that command " ++ conflictName ++ " is automatically added)"
      : "";
  let isTrueBoolString = s =>
    switch (s) {
    | "true"
    | "TRUE"
    | "1"
    | "yes"
    | "y"
    | "YES"
    | "Y" => true
    | _ => false
    };
  let isFalseBoolString = s =>
    switch (s) {
    | "false"
    | "FALSE"
    | "0"
    | "no"
    | "n"
    | "NO"
    | "N" => true
    | _ => false
    };
  let validAlphaStartNameRegex =
    Re.Pcre.regexp(
      "((^[a-z]|[A-Z])(([a-z]|[A-Z]|[0-9]|-|_)*))",
    );
  let validateArgumentName = name =>
    if (!Re.execp(validAlphaStartNameRegex, name)) {
      CommonErrors.fatal(
        "Invalid argument name: "
        ++ name
        ++ ". Argument names must start with a letter then only "
        ++ "letters, digits, dashes, or underscores.",
      );
    };
  let validateCommandName = name =>
    if (!Re.execp(validAlphaStartNameRegex, name)) {
      CommonErrors.fatal(
        "Invalid command name: "
        ++ name
        ++ ". Command names must start with a letter then only "
        ++ "letters, digits, dashes, or underscores.",
      );
    };
  let validateCommandAlias = (name, alias) =>
    if (!Re.execp(validAlphaStartNameRegex, alias)) {
      CommonErrors.fatal(
        "Invalid command alias: "
        ++ alias
        ++ ". Check command named: "
        ++ name
        ++ ". Command aliases must start with a letter then only "
        ++ "letters, digits, dashes, or underscores.",
      );
    };
  let intStringRegex = Re.Pcre.regexp("([0-9]+)");
  let isIntString = s => Re.execp(intStringRegex, s);
  let collapsedOptionRegex =
    Re.Pcre.regexp("(-(([a-z]|[A-Z]|[0-9])*))");
  let isValidCollapsedOptionName = name =>
    Re.execp(collapsedOptionRegex, name);
  let validOptionNameRegex =
    Re.Pcre.regexp(
      "(((--?)([a-z]|[A-Z]))(([a-z]|[A-Z]|[0-9]|-)*))",
    );
  let isValidOptionName = name =>
    Re.execp(validOptionNameRegex, name);
  let validateOptionName = name =>
    if (!Re.execp(validOptionNameRegex, name)) {
      CommonErrors.fatal(
        "Invalid option name: "
        ++ name
        ++ ". Option names must start with one or two dashes (- or --), a "
        ++ "letter, then only letters, digits, or dashes.",
      );
    };
  let validateOptionAlias = (name, alias) =>
    if (!Re.execp(validOptionNameRegex, alias)) {
      CommonErrors.fatal(
        "Invalid option alias: "
        ++ alias
        ++ ". Check option named: "
        ++ name
        ++ ". Option aliases must start with one or two dashes (- or --), a "
        ++ "letter, then only letters, digits, or dashes.",
      );
    };
  /* We've already validated the name so this is more relaxed for extraction */
  let optionNameNormalizationRegex =
    Re.Pcre.regexp("((-*)(((.|n)*)))");
  let normalizeOptionName = name => {
    let group = Re.exec_opt(optionNameNormalizationRegex, name)
    let normalized =
      switch (group) {
      | Some(group) => Re.Group.get(group, 3);
      | None =>
        CommonErrors.fatal(
          "Internal Error. Couldn't normalize option: " ++ name,
        )
      };
    normalized;
  };
  let addOption = (name, option, usedOptionNames) => {
    let normalized = normalizeOptionName(name);
    /* Some of these additions are likely duplicates. That's okay. */
    usedOptionNames
    |> StringMap.set(name, option)
    |> StringMap.set(normalized, option)
    |> StringMap.set("-" ++ normalized, option)
    |> StringMap.set("--" ++ normalized, option);
  };
  let isOptional = kind =>
    switch (kind) {
    | Optional(_) => true
    | _ => false
    };
  let emptyTmpCommand: command = {
    automatic: false,
    name: defaultCommandName,
    aliases: StringSet.empty,
    desc: None,
    arguments: [],
    options: [],
    usedOptionNames: StringMap.empty,
    action: None,
  };
  let ensurePreparing =
      (
        ~name: option(string)=?,
        ~aliases: list(string)=[],
        ~automatic: bool=false,
        program: program,
      )
      : program =>
    switch (program) {
    | {preparing: None} => {
        ...program,
        preparing:
          Some({
            ...emptyTmpCommand,
            automatic,
            name: CommonOption.valueOr(defaultCommandName, name),
            aliases: StringSet.fromList(aliases),
          }),
      }
    | _ => program
    };
  let fatalNotPrepared = () =>
    CommonErrors.fatal(
      "Internal Error. Expected next command to be prepared.",
    );
};

/* TODO: Allow tri-column and put type information for options and args in */
module Help = {
  type fmt =
    | NewLine
    | Indent(int)
    | Line(string)
    | SplitColumns
    | Column(string, string);
  let wrapAt = (width: int, s: string): list(string) => {
    ();
    let length = Pastel.length;
    let (line, delim, lines) =
      s
      |> Re.split_full(Re.Pcre.regexp("[ ]+"))
      |> List.fold_left(
           (acc, part) => {
             let (line, delim, lines) = acc;
             switch (part) {
             /* Skip leading delimeters */
             | `Delim(value) when length(line) === 0 => acc
             /* If the delim causes a break, move to next line */
             | `Delim(value)
                 when length(line) + length(delim) + length(Re.Group.get(value, 0)) > width => (
                 "",
                 "",
                 lines @ [line],
               )
             /* Track delims that don't cause breaks on their own */
             | `Delim(value) => (line, delim ++ Re.Group.get(value, 0), lines)
             /* Skip empty text, not sure how this might happen */
             | `Text(value) when length(value) === 0 => acc
             /* Break if this is too long */
             | `Text(value)
                 when length(line) + length(delim) + length(value) > width => (
                 value,
                 "",
                 lines @ [line],
               )
             /* Otherwise add the text and delim */
             | `Text(value) => (line ++ delim ++ value, "", lines)
             };
           },
           ("", "", []),
         );
    if (length(line) > 0) {
      lines @ [line];
    } else {
      lines;
    };
  };
  let getIndent = (count: int): string => String.make(count * 2, ' ');
  let maxInList = (strings: list(string)): int =>
    List.fold_left((acc, s) => max(acc, Pastel.length(s)), 0, strings);
  let maxInColumns =
      (columns: list((list(string), list(string)))): (int, int) =>
    List.fold_left(
      ((maxLeft, maxRight), (left, right)) =>
        (max(maxLeft, maxInList(left)), max(maxRight, maxInList(right))),
      (0, 0),
      columns,
    );
  /*
   * TODO: Something with align logic is wrong. Can't get right column to align
   * at 4. Reduced alignment to 2 for now to hide the bug. Probably has to do
   * with the indentation.
   */
  let align = (tabLength: int, width: int): int =>
    width + width mod tabLength;
  let fmtToString = (~minWidth=40, ~maxWidth=80, fmt: list(fmt)): string => {
    let minWidth = max(40, minWidth);
    let maxWidth = max(minWidth, maxWidth);
    /*
     * This groups consecutive columns together in chunks for printing then
     * formats everything into a single string
     */
    let (_, lines) =
      fmt
      |> List.fold_left(
           (acc, fmt) => {
             ();
             switch (fmt, acc) {
             | (Column(_, _), [[Column(_, _), ..._] as head, ...tail]) =>
               [[fmt] @ head] @ tail
             | _ => [[fmt]] @ acc
             };
           },
           [],
         )
      |> List.map(List.rev)
      |> List.rev
      |> List.fold_left(
           (acc, parts) => {
             let (indent, lines) = acc;
             let indentLength = 2 * indent;
             switch (parts) {
             | [Column(_, _), ...rest] =>
               /* Verify that everything is a Column, otherwise grouping failed */
               let columns =
                 List.map(
                   part =>
                     switch (part) {
                     | Column(left, right) => ([left], [right])
                     | _ =>
                       CommonErrors.fatal(
                         "Internal Error: Non-columm grouped with column",
                       )
                     },
                   parts,
                 );
               /* Find the max length of left and right columns */
               let (maxLeft, maxRight) = maxInColumns(columns);
               /* Add 4 and align it to next increment of 4 to simulate tabs */
               let leftLength = align(2, maxLeft + 4);
               /* Now we check if we need to wrap anything */
               let columns =
                 if (leftLength + maxRight > maxWidth - indentLength) {
                   /* Left is allowed about max(30, 1/4th of width) + 4 */
                   let allowedLeft = max(30, maxWidth / 4) + 4;
                   let allowedLeft = align(2, allowedLeft);
                   /* Wrap left if necessary */
                   let columns =
                     if (leftLength > allowedLeft) {
                       List.map(
                         ((left, right)) =>
                           (
                             String.concat(" ", left) |> wrapAt(allowedLeft),
                             right,
                           ),
                         columns,
                       );
                     } else {
                       columns;
                     };
                   /* Recompute maximums, then check if right needs to wrap */
                   let (maxLeft, maxRight) = maxInColumns(columns);
                   let leftLength = align(2, maxLeft + 4);
                   /* Wrap right if necessary */
                   if (leftLength + maxRight > maxWidth - indentLength) {
                     let allowedRight =
                       max(30, maxWidth - leftLength - indentLength);
                     List.map(
                       ((left, right)) =>
                         (
                           left,
                           String.concat(" ", right) |> wrapAt(allowedRight),
                         ),
                       columns,
                     );
                   } else {
                     columns;
                   };
                 } else {
                   columns;
                 };
               /* Columns are all set, now print them */
               let (maxLeft, maxRight) = maxInColumns(columns);
               let leftLength = align(2, maxLeft + 4);
               let indentStr = getIndent(indent);
               let newLines =
                 columns
                 |> List.map(((left, right)) => {
                      let ln = List.length(left);
                      let rn = List.length(right);
                      let (left, right) =
                        if (ln === rn) {
                          (left, right);
                        } else if (ln < rn) {
                          (
                            left @ (Array.make(rn - ln, "") |> Array.to_list),
                            right,
                          );
                        } else {
                          (
                            left,
                            right @ (Array.make(ln - rn, "") |> Array.to_list),
                          );
                        };
                      let lines =
                        List.map2(
                          (left, right) => {
                            ();
                            let ln = Pastel.length(left);
                            let left =
                              if (ln < leftLength) {
                                left ++ String.make(leftLength - ln, ' ');
                              } else {
                                left;
                              };
                            left ++ right;
                          },
                          left,
                          right,
                        );
                      lines;
                    })
                 |> List.flatten
                 |> List.map(line => indentStr ++ line);
               (indent, lines @ newLines);
             | [_, _, ...rest] =>
               CommonErrors.fatal(
                 "Internal Error: Non-column elements were grouped",
               )
             | [] => CommonErrors.fatal("Internal Error: Empty group")
             | [NewLine] => (indent, lines @ [""])
             | [Indent(value)] => (max(0, indent + value), lines)
             | [Line(value)] =>
               let value =
                 value
                 |> Re.Pcre.split(~rex=Re.Pcre.regexp("\n"))
                 |> String.concat(" ")
                 |> wrapAt(max(maxWidth - indentLength, minWidth))
                 |> List.map(part => getIndent(indent) ++ part)
                 |> String.concat("\n");
               (indent, lines @ [value]);
             | [SplitColumns] => acc
             };
           },
           (0, []),
         );
    String.concat("\n", lines);
  };
  let printHelp = (program: program, command: command, ~useColor: bool): unit => {
    let noColor = s => s;
    let red = useColor ? Pastel.red : noColor;
    let green = useColor ? Pastel.green : noColor;
    let cyan = useColor ? Pastel.cyan : noColor;
    let blue = useColor ? Pastel.blue : noColor;
    let whiteBright = useColor ? Pastel.whiteBright : noColor;
    /* Get useful information */
    let isDefaultHelp = command.name == Utils.defaultCommandName;
    let defaultCommand =
      StringMap.getOpt(Utils.defaultCommandName, program.commands);
    let defaultCommand =
      switch (defaultCommand) {
      | Some(command) => command
      | None =>
        CommonErrors.fatal("Internal error: Couldn't find default command")
      };
    /*
     * This doesn't handle the case where an action is never set for the entire
     * program. That's probably okay, because then the program is useless.
     */
    let isCommandOptional =
      switch (defaultCommand) {
      /* If a default action is set then the program doesn't need a command */
      | {action: Some(_)} => true
      | _ => false
      };
    let nonDefaultCommands =
      program.commands
      |> StringMap.filter((value, key) => key != Utils.defaultCommandName)
      |> StringMap.values;
    let sortedUserCommands =
      nonDefaultCommands |> List.filter((c: command) => !c.automatic);
    let sortedGlobalCommands =
      nonDefaultCommands |> List.filter((c: command) => c.automatic);
    let sortedGlobalOptions =
      List.filter((o: cliOption) => o.automatic, defaultCommand.options);
    let sortedNormalOptions =
      List.filter((o: cliOption) => !o.automatic, command.options);
    let commandUsagePart =
      if (isDefaultHelp) {
        isCommandOptional ? red("[command]") : red("<command>");
      } else {
        red(command.name);
      };
    /* Leave off [options] in the usage string if none are user defined */
    let optionsUsagePart =
      if (List.length(sortedNormalOptions) > 0) {
        green(" [options]");
      } else {
        "";
      };
    /* Format things */
    let opt = CommonOption.valueOr("");
    let programNameParts = [
      Indent(1),
      NewLine,
      Line(cyan(program.name) ++ " " ++ opt(program.version)),
      NewLine,
    ];
    let descParts =
      if (isDefaultHelp) {
        switch (program.desc) {
        | Some(desc) => [Indent(1), Line(desc), Indent(-1), NewLine]
        | None => []
        };
      } else {
        switch (command.desc) {
        | None => []
        | Some(desc) => [Indent(1), Line(desc), Indent(-1), NewLine]
        };
      };
    let argumentsUsagePart =
      if (List.length(command.arguments) > 0) {
        let s =
          command.arguments
          |> List.map((arg: argument) =>
               Utils.isOptional(arg.kind)
                 ? "[" ++ arg.name ++ "]" : "<" ++ arg.name ++ ">"
             )
          |> List.map(blue)
          |> String.concat(" ");
        " " ++ s;
      } else {
        "";
      };
    let usageLine =
      program.name
      ++ " "
      ++ commandUsagePart
      ++ optionsUsagePart
      ++ argumentsUsagePart;
    let usageParts = [
      Line(whiteBright("USAGE")),
      NewLine,
      Indent(1),
      Line(usageLine),
      Indent(-1),
      NewLine,
    ];
    let getCommandBlock = commands => {
      ();
      List.map(
        (command: command) => {
          let left =
            command.aliases
            |> StringSet.reduce(
                 (acc, alias) => acc @ [alias],
                 [command.name],
               )
            |> List.map(red)
            |> String.concat(", ");
          let right = opt(command.desc);
          Column(left, right);
        },
        commands,
      );
    };
    let userCommandParts =
      if (isDefaultHelp) {
        if (isCommandOptional || List.length(sortedUserCommands) > 0) {
          [
            [Line(whiteBright("COMMANDS")), NewLine],
            [Indent(1)],
            if (isCommandOptional) {
              [
                Column(
                  red("(default)"),
                  "The default action, command is optional",
                ),
              ];
            } else {
              [];
            },
            if (List.length(sortedUserCommands) > 0) {
              getCommandBlock(sortedUserCommands);
            } else {
              [];
            },
            [Indent(-1), NewLine],
          ];
        } else {
          [];
        };
      } else {
        [];
      };
    let userCommandParts = List.flatten(userCommandParts);
    let globalCommandParts =
      if (isDefaultHelp) {
        if (List.length(sortedGlobalCommands) > 0) {
          [
            [Line(whiteBright("BUILT-IN COMMANDS")), NewLine],
            [Indent(1)],
            getCommandBlock(sortedGlobalCommands),
            [Indent(-1), NewLine],
          ];
        } else {
          [];
        };
      } else {
        [];
      };
    let globalCommandParts = List.flatten(globalCommandParts);
    let printArgument = (arg: argument) => {
      /* TODO: Also print type information */
      let left = blue(arg.name);
      let right = arg.desc;
      Column(left, right);
    };
    let argumentsParts =
      if (List.length(command.arguments) > 0) {
        List.flatten([
          [Line(whiteBright("ARGUMENTS")), NewLine, Indent(1)],
          List.map(printArgument, command.arguments),
          [Indent(-1), NewLine],
        ]);
      } else {
        [];
      };
    let printOption = (o: cliOption) => {
      /* TODO: Also print type information */
      let left =
        o.aliases
        |> StringSet.reduce((acc, alias) => acc @ [alias], [o.name])
        |> List.map(green)
        |> String.concat(", ");
      let right = o.desc;
      Column(left, right);
    };
    let optionParts =
      if (List.length(sortedNormalOptions) > 0) {
        List.flatten([
          [Line(whiteBright("OPTIONS")), NewLine, Indent(1)],
          List.map(printOption, sortedNormalOptions),
          [Indent(-1), NewLine],
        ]);
      } else {
        [];
      };
    let globalOptionParts =
      if (List.length(sortedGlobalOptions) > 0) {
        List.flatten([
          [Line(whiteBright("BUILT-IN OPTIONS")), NewLine, Indent(1)],
          List.map(printOption, sortedGlobalOptions),
          [Indent(-1), NewLine],
        ]);
      } else {
        [];
      };
    let fmt =
      List.flatten([
        programNameParts,
        descParts,
        usageParts,
        userCommandParts,
        argumentsParts,
        optionParts,
        globalCommandParts,
        globalOptionParts,
        [Indent(-1)],
      ]);
    /* TODO: Decide how to set maxWidth based on terminal width */
    let string = fmtToString(fmt);
    print_endline(string);
    ();
  };
};

/**
 * Sets the program's version to be the given string. E.g. version("1.0.0").
 */
let version = (version: string, program: program): program => {
  /* Validation */
  switch (program) {
  | {version: Some(prev)} =>
    CommonErrors.fatal(
      "Version already set as \""
      ++ prev
      ++ "\", do not set it multiple times. Recieved: \""
      ++ version
      ++ "\"",
    )
  | _ => ()
  };
  /* Add the version */
  {...program, version: Some(version)};
};

/**
 * Adds a description to the program or current sub-command.
 */
let description = (desc: string, program: program): program => {
  let program =
    switch (program) {
    | {desc: Some(_), preparing: None} =>
      CommonErrors.fatal(
        "A desciption has already been set for the program, "
        ++ "do not set it multiple times.",
      )
    | {preparing: Some({name, desc: Some(_)})}
        when name == Utils.defaultCommandName =>
      CommonErrors.fatal(
        "A desciption has already been set for the program, "
        ++ "do not set it multiple times.",
      )
    | {preparing: Some({name, desc: Some(_)})} =>
      CommonErrors.fatal(
        "A description has already been set for the command: "
        ++ name
        ++ ", do not set it multiple times.",
      )
    /* If already preparing a command attach description there. */
    | {preparing: Some(preparing)} => {
        ...program,
        preparing: Some({...preparing, desc: Some(desc)}),
      }
    /* Otherwise attach it to the program. */
    | {preparing: None} => {...program, desc: Some(desc)}
    };
  program;
};

/**
 * Starts a new sub-command with the given name.
 */
let commandInternal =
    (
      name: string,
      ~aliases: list(string)=[],
      ~automatic: bool=false,
      program: program,
    )
    : program => {
  /* Move potentially preparing command to commands */
  let program = Utils.finalizePreparing(program);
  /* Make sure the names and aliases are valid */
  Utils.validateCommandName(name);
  List.iter(alias => Utils.validateCommandAlias(name, alias), aliases);
  /* Make sure this name doesn't conflict with any other names */
  if (StringMap.has(name, program.usedCommandNames)) {
    let duplicate =
      StringMap.getOpt(name, program.usedCommandNames) |> CommonOption.valuex;
    CommonErrors.fatal(
      "Provided command name: "
      ++ name
      ++ " is the same as existing command: "
      ++ duplicate.name
      ++ " or one of its aliases."
      ++ Utils.automaticCommandNotice(duplicate.automatic, duplicate.name),
    );
  };
  List.iter(
    alias =>
      if (StringMap.has(alias, program.usedCommandNames)) {
        let duplicate =
          StringMap.getOpt(alias, program.usedCommandNames)
          |> CommonOption.valuex;
        CommonErrors.fatal(
          "Provided command alias: "
          ++ alias
          ++ " for the command named: "
          ++ name
          ++ " is the same as existing command: "
          ++ duplicate.name
          ++ " or one of its aliases."
          ++ Utils.automaticCommandNotice(duplicate.automatic, duplicate.name),
        );
      },
    aliases,
  );
  /* Now ensure preparing is set again for this command. */
  Utils.ensurePreparing(~name, ~aliases, ~automatic, program);
};

let command = (name, ~aliases=[], program) =>
  commandInternal(name, ~aliases, program);

let action = (action, program: program): program => {
  let program = Utils.ensurePreparing(program);
  let program =
    switch (program) {
    | {preparing: None} => Utils.fatalNotPrepared()
    | {preparing: Some({name, action: Some(_)})}
        when name == Utils.defaultCommandName =>
      CommonErrors.fatal(
        "An action has already been set for the program, "
        ++ "do not set it multiple times.",
      )
    | {preparing: Some({name, action: Some(_)})} =>
      CommonErrors.fatal(
        "An action has already been set for the command: "
        ++ name
        ++ ", do not set it multiple times.",
      )
    /* If already preparing a command attach description there. */
    | {preparing: Some(preparing)} => {
        ...program,
        preparing: Some({...preparing, action: Some(action)}),
      }
    };
  program;
};

/**
 * Adds an argument to the program or current sub-command.
 */
let argument =
    (name: string, desc: string, kind: kind, program: program): program => {
  /* Argument input validation */
  Utils.validateArgumentName(name);
  /* Program validation / construction */
  let program = Utils.ensurePreparing(program);
  let program =
    switch (program) {
    /* Unreachable */
    | {preparing: None} => Utils.fatalNotPrepared()
    /* Required arguments cannot appear after optional ones. */
    | {preparing: Some({arguments})}
        when
          !Utils.isOptional(kind)
          && List.exists(
               (arg: argument) => Utils.isOptional(arg.kind),
               arguments,
             ) =>
      CommonErrors.fatal(
        "Required arguments cannot appear after optional ones. Check argument: "
        ++ name,
      )
    /* Add the argument */
    | {preparing: Some(preparing)} =>
      let nextArgument: argument = {name, desc, kind};
      let arguments = preparing.arguments @ [nextArgument];
      {...program, preparing: Some({...preparing, arguments})};
    };
  program;
};

/**
 * Adds an option to the program or current sub-command.
 */
let optionInternal =
    (
      name: string,
      ~aliases: list(string)=[],
      desc: string,
      kind: kind,
      ~automatic: bool=false,
      program: program,
    )
    : program => {
  /* Option input validation */
  Utils.validateOptionName(name);
  List.iter(alias => Utils.validateOptionAlias(name, alias), aliases);
  /* Program validation / construction */
  let program = Utils.ensurePreparing(program);
  let program =
    switch (program) {
    /* Unreachable */
    | {preparing: None} => Utils.fatalNotPrepared()
    /* Add the argument */
    | {preparing: Some(preparing)} =>
      /* Test the option name for conflicts. */
      let conflict = StringMap.getOpt(name, preparing.usedOptionNames);
      switch (conflict) {
      | Some({automatic, name: conflictName}) =>
        CommonErrors.fatal(
          "Provided option name: "
          ++ name
          ++ " is too similar to existing option: "
          ++ conflictName
          ++ " or one of its aliases."
          ++ Utils.automaticOptionNotice(automatic, conflictName),
        )
      | _ => ()
      };
      /* Now test the aliases. Done separately to provide better error. */
      List.iter(
        alias => {
          let conflict = StringMap.getOpt(alias, preparing.usedOptionNames);
          switch (conflict) {
          | Some({automatic, name: conflictName}) =>
            CommonErrors.fatal(
              "Provided option alias: "
              ++ alias
              ++ " for the option: "
              ++ name
              ++ " is too similar to existing option: "
              ++ conflictName
              ++ " or one of its aliases."
              ++ Utils.automaticOptionNotice(automatic, conflictName),
            )
          | _ => ()
          };
        },
        aliases,
      );
      /* Construct the new option */
      let nextOption: cliOption = {
        automatic,
        name,
        desc,
        kind,
        aliases: StringSet.fromList(aliases),
      };
      let options = preparing.options @ [nextOption];
      /*
       * Now add name and aliases as used option names. We do not do this before
       * or during validation because this option can have aliases that conflict
       * with itself after normalization. For example --foo and an alias -foo
       * should be allowed when they refer to the same option.
       */
      let usedOptionNames = preparing.usedOptionNames;
      let usedOptionNames =
        Utils.addOption(name, nextOption, usedOptionNames);
      let usedOptionNames =
        List.fold_left(
          (m, alias) => Utils.addOption(alias, nextOption, m),
          usedOptionNames,
          aliases,
        );
      /* Build the updated program */
      {
        ...program,
        preparing: Some({...preparing, options, usedOptionNames}),
      };
    };
  program;
};

let option = (name, ~aliases=[], desc, kind, program) =>
  optionInternal(name, ~aliases, desc, kind, program);

/**
 * Creates a new program with the given name.
 */
let program = (name: string): program => {
  let program = {
    name,
    desc: None,
    version: None,
    commands: StringMap.empty,
    preparing: None,
    usedCommandNames: StringMap.empty,
  };
  program
  /* Add help command */
  |> commandInternal("help", ~automatic=true)
  |> description("Display help for a command")
  |> argument(
       "command",
       "The command to display help for",
       Optional(String),
     )
  |> action(({program, args, options}) => {
       /* TODO: Default command logic is repeated a lot, clean this up */
       let defaultCommand =
         switch (StringMap.getOpt(Utils.defaultCommandName, program.commands)) {
         | Some(command) => command
         | None =>
           CommonErrors.fatal("Internal Error: Can't find default command")
         };
       let commandName =
         args.optionalString("command")
         |> CommonOption.valueOr(Utils.defaultCommandName);
       let command = StringMap.getOpt(commandName, program.commands);
       let command = command |> CommonOption.valueOr(defaultCommand);
       Help.printHelp(
         program,
         command,
         ~useColor=!options.bool("--no-color"),
       );
     })
  |> Utils.finalizePreparing
  /* Add version */
  |> commandInternal("version", ~automatic=true)
  |> description("Display version")
  |> action(({program}) =>
       switch (program.version) {
       | Some(version) => print_endline(version)
       | None => print_endline("Version not set")
       }
     )
  |> Utils.finalizePreparing
  /* Then add top-level global options */
  |> optionInternal(
       "--help",
       ~aliases=["-h"],
       "Display help",
       Bool,
       ~automatic=true,
     )
  |> optionInternal(
       "--version",
       ~aliases=["-v"],
       "Display version",
       Bool,
       ~automatic=true,
     )
  |> optionInternal("--no-color", "Disable colors", Bool, ~automatic=true)
  |> optionInternal(
       "--quiet",
       "Quiet mode - will only display warn and error messages",
       Bool,
       ~automatic=true,
     )
  |> optionInternal(
       "--verbose",
       "Verbose mode - will also display debug messages",
       Bool,
       ~automatic=true,
     );
};

type globalOptions = {
  mutable noColor: bool,
  mutable quiet: bool,
  mutable verbose: bool,
};

/**
 * Runs the program based on given command line args. For example:
 *
 *   ```
 *   program("MyProgram") |> ... |> parseAndRun(Sys.argv);
 *   ```
 *
 * This first element in the array will be skipped, it should refer to the file
 * of this executable. If manually calling this function use __FILE__, or some
 * dummy value for the first element to avoid the argument being skipped.
 */
let parseAndRun = (args: array(string), program: program): unit => {
  let program = Utils.finalizePreparing(program);
  let program = Utils.maybePromoteDescrption(program);
  /* Extract default command */
  let defaultCommand =
    StringMap.getOpt(Utils.defaultCommandName, program.commands);
  let defaultCommand =
    switch (defaultCommand) {
    | Some(command) => command
    | None =>
      CommonErrors.fatal("Internal error: Couldn't find default command")
    };
  let args = Array.to_list(args);
  let optionValues: MStringMap.t(value) = MStringMap.empty();
  let argumentValues: MStringMap.t(value) = MStringMap.empty();
  let globalOptions = {noColor: false, quiet: false, verbose: false};
  /* TODO: Make these better */
  let logger = {
    debug: s => {
      let s = "[Debug] " ++ s;
      if (globalOptions.verbose) {
        if (globalOptions.noColor) {
          print_endline(s);
        } else {
          print_endline(Pastel.blue(s));
        };
      };
    },
    info: s =>
      if (!globalOptions.quiet) {
        print_endline(s);
      },
    warn: s => {
      let s = "[Warning] " ++ s;
      if (globalOptions.noColor) {
        print_endline(s);
      } else {
        print_endline(Pastel.yellow(s));
      };
    },
    error: s => {
      let s = "[Error] " ++ s;
      if (globalOptions.noColor) {
        print_endline(s);
      } else {
        print_endline(Pastel.red(s));
      };
    },
  };
  /*
   * TODO: These should be more robust, making sure the given names are actually
   * optional or not according to the command's spec. And better errors.
   */
  let buildContainer = (thing: string, map) => {
    string: name =>
      switch (MStringMap.getOpt(name, map)) {
      | Some(StringValue(value)) => value
      | Some(_) =>
        CommonErrors.fatal(
          "Invalid, non-string value present for " ++ thing ++ ": " ++ name,
        )
      | None =>
        CommonErrors.fatal("Missing value for " ++ thing ++ ": " ++ name)
      },
    int: name =>
      switch (MStringMap.getOpt(name, map)) {
      | Some(IntValue(value)) => value
      | Some(_) =>
        CommonErrors.fatal(
          "Invalid, non-int value present for " ++ thing ++ ": " ++ name,
        )
      | None =>
        CommonErrors.fatal("Missing value for " ++ thing ++ ": " ++ name)
      },
    bool: name =>
      switch (MStringMap.getOpt(name, map)) {
      | Some(BoolValue(value)) => value
      | Some(_) =>
        CommonErrors.fatal(
          "Invalid, non-bool value present for " ++ thing ++ ": " ++ name,
        )
      | None => false
      },
    optionalString: name =>
      switch (MStringMap.getOpt(name, map)) {
      | Some(StringValue(value)) => Some(value)
      | Some(_) =>
        CommonErrors.fatal(
          "Invalid, non-string value present for " ++ thing ++ ": " ++ name,
        )
      | None => None
      },
    optionalInt: name =>
      switch (MStringMap.getOpt(name, map)) {
      | Some(IntValue(value)) => Some(value)
      | Some(_) =>
        CommonErrors.fatal(
          "Invalid, non-int value present for " ++ thing ++ ": " ++ name,
        )
      | None => None
      },
  };
  let optionContainer = buildContainer("option", optionValues);
  let argContainer = buildContainer("argument", argumentValues);
  let self = {
    program,
    log: logger,
    options: optionContainer,
    args: argContainer,
  };
  /* First pass at determining command and consuming first few args */
  let (command, args) =
    switch (args) {
    | []
    | [_] =>
      /* Attempt default command */
      (StringMap.getOpt(Utils.defaultCommandName, program.commands), [])
    | [_, command, ...args] =>
      let maybeCommand = StringMap.getOpt(command, program.commands);
      switch (maybeCommand) {
      | Some(command) => (maybeCommand, args)
      | None => (
          StringMap.getOpt(Utils.defaultCommandName, program.commands),
          [command] @ args,
        )
      };
    };
  /* Unwrap the command we will be running */
  let command =
    switch (command) {
    | Some(command) => command
    | None => CommonErrors.fatal("Internal Error: Can't find command")
    };
  /*
   * Parse options, this should never throw so we can switch to global commands
   * from options as needed without requiring options/args to be valid
   *
   * i.e. throwing --help at the end of a command with missing options is okay
   */
  /* TODO: Add support for --foo=blah style options */
  let args = Array.of_list(args);
  let n = Array.length(args);
  let consumed = Array.make(n, false);
  let i = ref(0);
  while (i^ < n) {
    let s = args[i^];
    if (Utils.isValidOptionName(s)) {
      let name = Utils.normalizeOptionName(s);
      let o = StringMap.getOpt(name, command.usedOptionNames);
      /* Check if it's a global option */
      let o =
        switch (o) {
        | Some(_) => o
        | None => StringMap.getOpt(name, defaultCommand.usedOptionNames)
        };
      switch (o) {
      | Some(o) =>
        switch (o.kind) {
        | String
        | Optional(String) =>
          consumed[i^] = true;
          incr(i);
          if (i^ >= n) {
            MStringMap.set(o.name, EmptyValue, optionValues);
          } else {
            consumed[i^] = true;
            let value = args[i^];
            MStringMap.set(o.name, StringValue(value), optionValues);
          };
        | Int
        | Optional(Int) =>
          consumed[i^] = true;
          incr(i);
          if (i^ >= n) {
            MStringMap.set(o.name, EmptyValue, optionValues);
          } else {
            consumed[i^] = true;
            let value = String.trim(args[i^]);
            if (!Utils.isIntString(value)) {
              MStringMap.set(o.name, InvalidValue(value), optionValues);
            } else {
              let value = int_of_string(value);
              MStringMap.set(o.name, IntValue(value), optionValues);
            };
          };
        | Bool
        | Optional(Bool) =>
          consumed[i^] = true;
          MStringMap.set(o.name, BoolValue(true), optionValues);
        /* TODO: Move this validation to the definition */
        | _ =>
          CommonErrors.fatal(
            "Internal Error: Invalid option kind for command: "
            ++ command.name
            ++ ", option: "
            ++ o.name,
          )
        };
        ();
      | None => ()
      };
    };
    if (Utils.isValidCollapsedOptionName(s)) {
      /* TODO: Support collapsed bool options: -aux is -a -u -x */
      ();
      ();
    };
    incr(i);
  };
  /* This removes all args that were consumed while parsing options */
  let args =
    args
    |> Array.to_list
    |> List.mapi((index, value) => consumed[index] ? None : Some(value))
    |> List.filter(CommonOption.isSome)
    |> List.map(CommonOption.valuex)
    |> Array.of_list;
  /* Now we can parse positional arguments */
  let argSpecs = command.arguments |> Array.of_list;
  let n = min(Array.length(argSpecs), Array.length(args));
  let i = ref(0);
  while (i^ < n) {
    let value = args[i^];
    let argSpec = argSpecs[i^];
    switch (argSpec.kind) {
    | String
    | Optional(String) =>
      MStringMap.set(argSpec.name, StringValue(value), argumentValues)
    | Int
    | Optional(Int) =>
      if (!Utils.isIntString(value)) {
        MStringMap.set(argSpec.name, InvalidValue(value), argumentValues);
      } else {
        let value = int_of_string(value);
        MStringMap.set(argSpec.name, IntValue(value), argumentValues);
      }
    | Bool
    | Optional(Bool) =>
      /*
       * We have to do an explicit check because arguments take a postition
       * and get consumed. Unlike options where their presence gives a boolean
       */
      if (Utils.isTrueBoolString(value)) {
        MStringMap.set(argSpec.name, BoolValue(true), argumentValues);
      } else if (Utils.isFalseBoolString(value)) {
        MStringMap.set(argSpec.name, BoolValue(false), argumentValues);
      } else {
        MStringMap.set(argSpec.name, InvalidValue(value), argumentValues);
      }
    /* TODO: Move this validation to the definition */
    | _ =>
      CommonErrors.fatal(
        "Internal Error: Invalid argument kind for command: "
        ++ command.name
        ++ ", argument: "
        ++ argSpec.name,
      )
    };
    incr(i);
  };
  /* Set global options that will not affect which command is run */
  if (optionContainer.bool("--no-color")) {
    globalOptions.noColor = true;
  };
  if (optionContainer.bool("--verbose")) {
    globalOptions.verbose = true;
  };
  if (optionContainer.bool("--quiet")) {
    if (optionContainer.bool("--verbose")) {
      logger.warn(
        "Both options `--quiet` and `--verbose` were specified. Using "
        ++ "verbose behavior.",
      );
    } else {
      globalOptions.quiet = true;
    };
  };
  /* Test arguments for presence of globals and update command accordingly */
  let command =
    if (optionContainer.bool("--help")) {
      /* TODO: This probably shouldn't be hard-coded to "command" */
      MStringMap.set("command", StringValue(command.name), argumentValues);
      switch (StringMap.getOpt("help", program.commands)) {
      | Some(command) => command
      | None => CommonErrors.fatal("Internal Error: cannot find help command")
      };
    } else if (command.name == "help") {
      command;
    } else if (optionContainer.bool("--version")) {
      switch (StringMap.getOpt("version", program.commands)) {
      | Some(command) => command
      | None =>
        CommonErrors.fatal("Internal Error: cannot find version command")
      };
    } else {
      command;
    };
  /* TODO: Validate the parsed options and arguments against the command */
  /* Run the command */
  switch (command) {
  | {action: None} => CommonErrors.fatal("Action not set")
  | {action: Some(action)} => action(self)
  };
};
