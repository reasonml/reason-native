/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open TestFramework;
open Chalk;

describe("Pre test runner exhaustiveness test", ({test}) =>
  test("should match snapshot", ({expect}) => {
    let outputRev = ref([]);
    let print_endline = s => outputRev := [s] @ outputRev^;
    let print_string = s => outputRev := switch(outputRev^){
    | [] => [s]
    | [first, ...rest] => [first ++ s, ...rest]
    };

    print_endline(
      <Chalk>
        <Chalk>
          "This is the Chalk tester. It tested almost every combination of "
          "modifiers and colors.\n"
          "Make sure your terminal is set to render different colors for the "
          "bright variants of each color \nand make sure your terminal is configured "
          "to *not* render bold text as bright.\n"
          "Instead bold and bright should remain independent concepts. \n"
        </Chalk>
        <Chalk bold=true color=Chalk.Yellow>
          "This tester does not test nesting of attributes. It is only meant for seeing all possible combinations."
        </Chalk>
      </Chalk>,
    );

    let intToBool = i => i === 0 ? false : true;

    let intToColor = i =>
      switch (i) {
      | 0 => Black
      | 1 => Red
      | 2 => Green
      | 3 => Yellow
      | 4 => Blue
      | 5 => Magenta
      | 6 => Cyan
      | 7 => White
      /* This color is missing for foreground. */
      | 8 => BlackBright
      | 9 => RedBright
      | 10 => GreenBright
      | 11 => YellowBright
      | 12 => BlueBright
      | 13 => MagentaBright
      | 14 => CyanBright
      | 15 => WhiteBright
      | _ => raise(Invalid_argument("Bad color index"))
      };

    let colorNameToString = i =>
      switch (i) {
      | Black => "Black         "
      | Red => "Red           "
      | Green => "Green         "
      | Yellow => "Yellow        "
      | Blue => "Blue          "
      | Magenta => "Magenta       "
      | Cyan => "Cyan          "
      | White => "White         "
      | BlackBright => "BlackBright   "
      | RedBright => "RedBright     "
      | GreenBright => "GreenBright   "
      | YellowBright => "YellowBright  "
      | BlueBright => "BlueBright    "
      | MagentaBright => "MagentaBright "
      | CyanBright => "CyanBright    "
      | WhiteBright => "WhiteBright   "
      };

    let label = (~backgroundColor, ~color, ~bold, ~italic, ~underline) =>
      (italic ? "ita " : "noI ") ++ (underline ? "und" : "noU");

    let margin = "              ";
    let getTitle1 = (~backgroundColor, ~inverse) => {
      let inverseStr =
        inverse ?
          "(inverse)                            " :
          "                                     ";
      margin
      ++ Chalk.inverse(
           "                                   Background "
           ++ colorNameToString(backgroundColor)
           ++ inverseStr,
         );
    };
    let title2 =
      margin
      ++ Chalk.inverse("            Normal             ")
      ++ "  "
      ++ Chalk.inverse("            Bold               ")
      ++ "  "
      ++ Chalk.inverse("            Dimmed             ")
      ++ "  "
      ++ Chalk.inverse("         Bold And Dimmed       ");

    for (inverse in 0 to 1) {
      let inverse = intToBool(inverse);
      for (backgroundColor in 0 to 15) {
        print_endline("");
        print_endline("");
        let backgroundColor = intToColor(backgroundColor);
        print_endline(getTitle1(~backgroundColor, ~inverse));
        print_endline("");
        print_endline(title2);
        print_endline("");
        for (color in 0 to 15) {
          let color = intToColor(color);
          print_string(colorNameToString(color));
          for (dim in 0 to 1) {
            let dim = intToBool(dim);
            for (bold in 0 to 1) {
              let bold = intToBool(bold);
              for (italic in 0 to 1) {
                let italic = intToBool(italic);
                for (underline in 0 to 1) {
                  let underline = intToBool(underline);
                  print_string(
                    <Chalk
                      backgroundColor
                      color
                      bold
                      dim
                      italic
                      underline
                      inverse
                      hidden=false>
                      {
                        label(
                          ~backgroundColor,
                          ~color,
                          ~bold,
                          ~italic,
                          ~underline,
                        )
                      }
                    </Chalk>
                    ++ " ",
                  );
                };
              };
              print_string(" ");
            };
          };
          print_endline("");
        };
        print_endline("");
      };
      print_endline("");
    };
    expect.lines(List.rev(outputRev^)).toMatchSnapshot();
  })
);
