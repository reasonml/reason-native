open TestFramework;
module HumanReadablePastel =
  Pastel.Make({});
HumanReadablePastel.setMode(HumanReadable);
module TerminalPastel = Pastel.Make({});
TerminalPastel.setMode(Terminal);

describe("Pastel.reset", ({test}) => {
  test("apply reset state to single child human readable", ({expect}) => {
    let stateRegion = (
      HumanReadablePastel.(emptyStyle |> withReset),
      "hello",
    );
    let result = HumanReadablePastel.unstable_apply([stateRegion]);

    expect.string(String.escaped(result)).toEqual("<reset>hello</reset>");
    ();
  });
  test("apply reset state terminal", ({expect}) => {
    let stateRegion = TerminalPastel.(emptyStyle |> withReset, "hello");
    let result = TerminalPastel.unstable_apply([stateRegion]);

    expect.string(String.escaped(result)).toEqual("\\027[0mhello\\027[0m\\027[0m");
    ();
  });
  test("apply reset state with other state human readable", ({expect}) => {
    let result =
      HumanReadablePastel.(
        unstable_apply([
          (emptyStyle |> withReset, "hello"),
          (emptyStyle |> withColor(Green), "world"),
        ])
      );

    expect.string(String.escaped(result)).toEqual(
      "<reset>hello</reset><green>world</resetColor>",
    );
    ();
  });
  test("Terminal E2E single child", ({expect}) => {
    let input = <TerminalPastel reset=true> "hello" </TerminalPastel>;
    expect.string(input).toEqual("\027[0mhello\027[0m\027[0m");
  });
  test("styles inside reset", ({expect}) => {
    let input = <TerminalPastel reset=true color=Green> "should be green" </TerminalPastel>;
    expect.string(input).toEqual("\027[0m\027[32mshould be green\027[0m\027[0m\027[39m");
    ();
  });
  test("Terminal E2E nested", ({expect}) => {
    let result =
      <TerminalPastel color=Green>
        "Hello"
        <TerminalPastel dim=true>
          "world"
          <TerminalPastel reset=true>
            "unstyled"
            <TerminalPastel color=Magenta> "and magenta" </TerminalPastel>
          </TerminalPastel>
          "dim and green?"
        </TerminalPastel>
      </TerminalPastel>;
    expect.string(result).toEqual(
      "\027[32mHello\027[2mworld\027[0munstyled\027[0m\027[35mand magenta\027[0m\027[0m\027[2m\027[32mdim and green?\027[22m\027[39m",
    );
  });
  test("Human readable E2E nested", ({expect}) => {
    let input =
      <HumanReadablePastel color=Green>
        "Hello"
        <HumanReadablePastel dim=true>
          "world"
          <HumanReadablePastel reset=true>
            "unstyled"
            <HumanReadablePastel color=Magenta>
              "and magenta"
            </HumanReadablePastel>
          </HumanReadablePastel>
        </HumanReadablePastel>
      </HumanReadablePastel>;
    expect.string(input).toEqual(
      "<green>Hello<dim>world<reset>unstyled<reset><magenta>and magenta</reset></resetColor>",
    );
  });
});
