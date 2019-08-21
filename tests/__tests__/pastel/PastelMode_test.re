open TestFramework;

describe("Pastel modes", ({test}) => {
  test("Switching from default to human readable and back", ({expect}) => {
    module Pastel =
      Pastel.Make({});
    Pastel.setMode(HumanReadable);
    let output = <Pastel color=Red> "Hello" </Pastel>;
    expect.string(output).toEqual("<red>Hello</resetColor>");
    Pastel.setMode(Terminal);
    let output = <Pastel color=Red> "Hello" </Pastel>;
    expect.string(output).toEqual("\027[31mHello\027[39m");
  });
  test("Switching from default to disabled and back", ({expect}) => {
    module Pastel =
      Pastel.Make({});
    Pastel.setMode(Disabled);
    let output = <Pastel color=Red> "Hello" </Pastel>;
    expect.string(output).toEqual("Hello");
    Pastel.setMode(Terminal);
    let output = <Pastel color=Red> "Hello" </Pastel>;
    expect.string(output).toEqual("\027[31mHello\027[39m");
  });
  test("Use mode should switch back to previous output", ({expect}) => {
    module Pastel =
      Pastel.Make({});
    Pastel.setMode(Disabled);
    let output = <Pastel color=Red> "Hello" </Pastel>;
    expect.string(output).toEqual("Hello");
    let output =
      Pastel.useMode(Terminal, () => <Pastel color=Red> "Hello" </Pastel>);
    expect.string(output).toEqual("\027[31mHello\027[39m");
    let output = <Pastel color=Red> "Hello" </Pastel>;
    expect.string(output).toEqual("Hello");
  });
});
