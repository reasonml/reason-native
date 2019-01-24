open TestFramework;

describe("Pastel modes", ({test}) => {
  test("Default mode should produce ANSI terminal output", ({expect}) => {
    module DefaultPastel =
      Pastel.Make({});
    let output = <DefaultPastel color=Red> "Hello" </DefaultPastel>;
    expect.string(output).toEqual("\027[31mHello\027[39m");
  });
  test("Switching from default to human readable and back", ({expect}) => {
    module Pastel =
      Pastel.Make({});
    Pastel.setMode(HumanReadable);
    let output = <Pastel color=Red> "Hello" </Pastel>;
    expect.string(output).toEqual("<red>Hello</red>");
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
  test("Reset mode should switch back to terminal output", ({expect}) => {
    module Pastel =
      Pastel.Make({});
    Pastel.setMode(Disabled);
    let output = <Pastel color=Red> "Hello" </Pastel>;
    expect.string(output).toEqual("Hello");
    Pastel.resetMode();
    let output = <Pastel color=Red> "Hello" </Pastel>;
    expect.string(output).toEqual("\027[31mHello\027[39m");
  });
});
