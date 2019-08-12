open TestFramework;
open Pastel.States;

describeOnly("pastel states", ({test, testSkip}) => {
  testSkip("basic", ({expect}) => {
    let states = [
      (initialState, ""),
      ({...initialState, background: Some(Red)}, "hello "),
      ({...initialState, bold: Some(Bold), dim: Some(Dim)}, "test t"),
      (initialState, "est"),
      ({...initialState, inverse: Some(Inverse)}, " hello"),
      (initialState, "back to normal"),
    ];
    Console.log("\n");
    let result = resolveStateRegions(states);
    Console.log("\n");
    Console.log(result);
    Console.log("\n");
    Console.log(<Pastel.States color=Green> "Hello world" </Pastel.States>);
    Console.log("\n");
    Console.log(<Pastel.States color=Red> "Hello world" </Pastel.States>);
    Console.log("\n");
    let output =
      <Pastel.States dim=true>
        "oo"
        <Pastel.States color=Green> "hello world!" </Pastel.States>
        "unpasteled o_O"
      </Pastel.States>;
    Console.log(output);

    Console.log("\n");
    expect.string(result).toMatchSnapshot();
  });
  testSkip("play with state regions", ({expect}) => {
    let input =
      <Pastel dim=true>
        "oo"
        <Pastel color=Green> "hello world!" </Pastel>
        "unpasteled o_O"
      </Pastel>;
    let stateRegions = Pastel.States.parseStateRegions(input);
    Console.log("\n");
    /* Console.log(
         List.map(
           ((state, t)) => (Pastel.States.printState(state), t),
           stateRegions,
         ),
       ); */
    Console.log("actual");
    Console.log(resolveStateRegions(stateRegions));
    Console.log(String.escaped(resolveStateRegions(stateRegions)));
    Console.log("expected");
    Console.log(input);
    Console.log(String.escaped(input));
    expect.bool(true).toBeTrue();
  });
  test("play with state regions 2", ({expect}) => {
    module ToUse = Pastel.States;
    Console.log(<ToUse color=Red>"hello"</ToUse>);
    let input =
      <ToUse dim=true color=Red>
        "oo"
        <ToUse color=Green> "hello world!" </ToUse>
        "unpasteled o_O"
      </ToUse>;
    let stateRegions = Pastel.States.parseStateRegions(input);
    Console.log("\n");
    /* Console.log(
         List.map(
           ((state, t)) => (Pastel.States.printState(state), t),
           stateRegions,
         ),
       ); */
    Console.log("actual");
    Console.log(resolveStateRegions(stateRegions));
    Console.log(String.escaped(resolveStateRegions(stateRegions)));
    Console.log("expected");
    Console.log(input);
    Console.log(String.escaped(input));
    Console.log(<Pastel bold=true> "some text" <Pastel bold=false> "unbolded?" </Pastel></Pastel>);

    let problem = <Pastel bold = true> "Some text" <Pastel bold=false> "unbolded" </Pastel> "should be bold"</Pastel>;
    expect.bool(true).toBeTrue();
  });
});