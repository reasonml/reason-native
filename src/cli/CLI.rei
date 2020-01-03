/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

 module Types: {
    type program;
    type kind =
      | String
      | Int
      | Bool
      | Optional(kind);
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
    type self = {
      program,
      log: logger,
      options: container,
      args: container,
    };
    type actionFn = self => unit;
  };
  
  open Types;
  
  /**
  Starts creation of a new program with the given name.
  
  Example:
  
  ```
  FBCore.CLI.(
    program("shipit")
    |> version("1.0.0")
    |> description("Ship an app")
    |> argument("app", "The app to deploy", String)
    |> action(({log, args}) => {
      log.info("Shipping:")'
      log.info(args.string("app"));
    })
    |> parseAndRun(Sys.argv);
  );
  ```
  */
  let program: string => program;
  
  /**
  Adds version information to the current program. There are no restrictions on
  the format of the string, but sem-ver format is typical: "3.2.1"
  */
  let version: (string, program) => program;
  
  /**
  Adds a description to the program or the most recently defined command. This
  description will show up in the help message.
  */
  let description: (string, program) => program;
  
  /**
  Defines an option for the program or most recently defined command. Options
  are not positional, and their names must start with 1 or 2 dashes.
  
  Example:
  
  ```
  programBeingBuilt
  |> option("--delay", ~aliases=["-d"], "The delay in seconds", Int)
  |> action(({options}) => {
    sleep(options.int("--delay"));
  });
  ```
  
  CLI Usage:
  
  ```
  ./program --delay 100
  
  # Or with an alias:
  ./program -d 100
  ```
  
  */
  let option:
    (string, ~aliases: list(string)=?, string, kind, program) => program;
  
  /**
  Defines an argument for the program or most recently defined command.
  Arguments are positional.
  
  After the first optional argument is defined all following arguments must also
  be optional. Attempting to define a required argument will cause an error.
  
  Example:
  
  ```
  programBeingBuilt
  |> argument("build", "Which build to ship, dev or prod", String)
  |> action(({args}) => {
    deployApp(args.string("build"));
  });
  ```
  
  CLI Usage:
  
  ```
  ./program dev
  ```
  */
  let argument: (string, string, kind, program) => program;
  
  /**
  Adds a new sub-command to the program. This sub-command can have its own
  specific set of arguments and options. However, it can't have more nested
  commands.
  
  Specifiying a command is not necessary. Simple programs can define options,
  arguments, and actions for the "default" command without ever calling this
  function.
  
  Example:
  
  ```
  programBeingBuilt
  |> command("foo")
  |> description("The foo command")
  |> argument("foo-value", "The foo value", String)
  |> action(({log, args}) => {
      log.info(args.string("foo-value"))
    })
  |> command("bar")
  |> description("The bar command")
  |> argument("--value", "The bar value", String)
  |> action(({log, args, options}) => {
      log.info(options.string("--value"))
      /* The foo-value argument doesn't exist here, even if provided */
      /* log.info(args.string("foo-value")) */
    });
  ```
  
  CLI Usage:
  
  ```
  ./program foo "some value for foo"
  ./program bar --value "some value for bar"
  ```
  
  Specific help pages for each command are generated:
  
  ```
  ./program help foo
  
  # Or using the built-in option:
  ./program bar --help
  ```
  */
  let command: (string, ~aliases: list(string)=?, program) => program;
  
  /**
  This associates an action for the program or most recently defined command. A
  collection of utilities ar pvoided to assist with: logging, argument access,
  option access, etc.
  
  Example:
  
  ```
  programBeingBuilt
  |> argument("build", "Which build to ship, dev or prod", String)
  |> action(({log, args}) => {
    log.debug("Starting to build")
    let build = args.string("build");
    if (build != "prod") {
      log.warn("This is not a production build, it may be slower!");
    };
    let result = deployApp(build);
    if (result !== Ok) {
      log.error("Something went wrong!");
    } else {
      log.info("Succesfully built: " ++ build);
    };
    log.debug("Done building");
  });
  ```
  */
  let action: (self => unit, program) => program;
  
  /**
  This executes the program and calls the appropriate actions based on the
  arguments given. Typically the arguments passed in will be from `Sys.argv`. The
  first argument is expected to be the path to the executable being run, it will
  always be skipped while parsing arguments.
  
  Example:
  
  programBeingBuilt
  |> command("foo")
  |> ...
  |> parseAndRun(Sys.argv);
  */
  let parseAndRun: (array(string), program) => unit;
  