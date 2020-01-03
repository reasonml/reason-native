/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 *
 * @emails oncall+reason
 */;

open TestFramework;

let shipitProgram =
  CLI.(
    program("shipit")
    |> version("1.0.0")
    |> description("Ship an app")
    |> argument("app", "The app to deploy", String)
    |> argument(
         "build",
         "Which build to ship, dev or prod",
         Optional(String),
       )
    |> option(
         "--delay",
         ~aliases=["-d"],
         "How long to wait in seconds before shipping (default 0)",
         Optional(Int),
       )
    |> action(({log}) => log.info("Shipit!!!"))
  );

/* Helpful for debugging. */
let visualize = s => {
  print_newline();
  print_newline();
  print_endline("==========");
  print_endline(s);
  print_endline("==========");
  print_newline();
};
/* Supress unused warnings */
let _ = visualize;

let captureHumanReadableOutput = thunk => {
  Pastel.useMode(HumanReadable, () => {IO.captureOutput(() => {thunk()})});
};

describe("CLI", ({test}) => {
  test("Simple valid program", ({expect}) => {
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(shipitProgram |> parseAndRun([|__FILE__|]))
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
  });
  test("Help output", ({expect}) => {
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(shipitProgram |> parseAndRun([|__FILE__, "help"|]))
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
  });
  test("Help output without color", ({expect}) => {
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(shipitProgram |> parseAndRun([|__FILE__, "help", "--no-color"|]))
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
  });
  test("Duplicate version specification", ({expect}) => {
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(
          program("shipit")
          |> version("1.0.0")
          |> version("2.0.0")
          |> parseAndRun([|__FILE__|])
        )
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
  });
  test("Duplicate description specification", ({expect}) => {
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(
          program("shipit")
          |> version("1.0.0")
          |> description("Ship an app")
          |> description("Ship an app again")
          |> parseAndRun([|__FILE__|])
        )
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
  });
  test("Invalid argument names", ({expect}) => {
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(
          program("shipit")
          |> version("1.0.0")
          |> argument("_app", "The app to deploy", String)
          |> parseAndRun([|__FILE__|])
        )
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
  });
  test("Required argument after optional", ({expect}) => {
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(
          program("shipit")
          |> version("1.0.0")
          |> argument("foo", "Foo", Optional(String))
          |> argument("app", "The app to deploy", String)
          |> parseAndRun([|__FILE__|])
        )
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
  });
  test("Invalid option names", ({expect}) => {
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(
          program("shipit")
          |> version("1.0.0")
          |> option("app", "The app to deploy", String)
          |> parseAndRun([|__FILE__|])
        )
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
  });
  test("Conflicting option names", ({expect}) => {
    /* -app is too similar to --app */
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(
          program("shipit")
          |> version("1.0.0")
          |> option("--app", "The app to deploy", String)
          |> option("-app", "The app to deploy", String)
          |> parseAndRun([|__FILE__|])
        )
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
    /* -app is too similar to an alias of --application */
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(
          program("shipit")
          |> version("1.0.0")
          |> option("--application", ~aliases=["--app"], "desc", String)
          |> option("-app", "desc", String)
          |> parseAndRun([|__FILE__|])
        )
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
    /* alias of --app1 is too similar to an alias of --app2 */
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(
          program("shipit")
          |> version("1.0.0")
          |> option("--app1", ~aliases=["--app"], "desc", String)
          |> option("--app2", ~aliases=["-app"], "desc", String)
          |> parseAndRun([|__FILE__|])
        )
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
    /* -app is allowed to be an alias of --app though */
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(
          program("shipit")
          |> version("1.0.0")
          |> option("--app", ~aliases=["-app"], "desc", String)
          |> parseAndRun([|__FILE__|])
        )
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
    /* --help is an existing automatic option */
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(
          program("shipit")
          |> version("1.0.0")
          |> option("--help", "desc", String)
          |> parseAndRun([|__FILE__|])
        )
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
  });
  test("Conflicting command names", ({expect}) => {
    /* okay */
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(
          program("shipit")
          |> version("1.0.0")
          |> command("foo")
          |> parseAndRun([|__FILE__|])
        )
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
    /* same names */
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(
          program("shipit")
          |> version("1.0.0")
          |> command("foo")
          |> command("foo")
          |> parseAndRun([|__FILE__|])
        )
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
    /* conflicts with an alias */
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(
          program("shipit")
          |> version("1.0.0")
          |> command("foo")
          |> command("bar", ~aliases=["foo"])
          |> parseAndRun([|__FILE__|])
        )
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
    /* conflicts with an alias */
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(
          program("shipit")
          |> version("1.0.0")
          |> command("bar", ~aliases=["foo"])
          |> command("foo")
          |> parseAndRun([|__FILE__|])
        )
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
    /* conflicts with built in help command */
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(
          program("shipit")
          |> version("1.0.0")
          |> command("help")
          |> parseAndRun([|__FILE__|])
        )
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
  });
  test("Argument parsing", ({expect}) => {
    let (stdout, stderr, _) =
      captureHumanReadableOutput(() =>
        CLI.(
          program("foo")
          |> version("1.0.0")
          |> command("bar")
          |> option("--opt1", "Option 1", String)
          |> argument("arg1", "Argument 1", Bool)
          |> argument("arg2", "Argument 2", Bool)
          |> action(({log, options, args}) => {
               let opt1 = options.string("--opt1");
               log.info("--opt1: " ++ opt1);
               let arg1 = args.bool("arg1");
               log.info("arg1: " ++ string_of_bool(arg1));
               let arg2 = args.bool("arg2");
               log.info("arg2: " ++ string_of_bool(arg2));
             })
          |> parseAndRun([|
               __FILE__,
               "bar",
               "true",
               "--opt1",
               "option 1 value",
               "false",
             |])
        )
      );
    expect.string(stdout).toMatchSnapshot();
    expect.string(stderr).toMatchSnapshot();
  });
  test("Logging", ({expect}) => {
    let fooProgram =
      CLI.(
        program("foo")
        |> version("1.0.0")
        |> action(({log}) => {
             log.debug("debug");
             log.info("info");
             log.warn("warn");
             log.error("error");
           })
      );
    let runFoo = args => {
      let (stdout, stderr, _) =
        captureHumanReadableOutput(() => CLI.(fooProgram |> parseAndRun(args)));
      expect.string(stdout).toMatchSnapshot();
      expect.string(stderr).toMatchSnapshot();
    };
    runFoo([|__FILE__|]);
    runFoo([|__FILE__, "--quiet"|]);
    runFoo([|__FILE__, "--verbose"|]);
    runFoo([|__FILE__, "--quiet", "--verbose"|]);
    runFoo([|__FILE__, "--verbose", "--quiet"|]);
    runFoo([|__FILE__, "--no-color", "--verbose"|]);
    runFoo([|__FILE__, "--quiet", "--verbose", "--no-color"|]);
  });
});
