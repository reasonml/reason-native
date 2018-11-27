/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 *
 * LICENSE file in the root directory of this source tree.
 */;
open TestFramework;
open Chalk;

describe("Chalk.length", ({test}) => {
  test("Length with no colors", ({expect}) => {
    expect.int(length("")).toBe(0);
    expect.int(length("123")).toBe(3);
    expect.int(length("123 567 901")).toBe(11);
  });
  test("Length with colors", ({expect}) => {
    expect.int("" |> red |> length).toBe(0);
    expect.int("123" |> bg.red |> white |> length).toBe(3);
    expect.int("123 567 901" |> underline |> bg.white |> length).toBe(11);
  });
  test(
    "ANSI escape sequences should be applied inside chalkers and persisted outside of them",
    ({expect}) => {
      let testCase =
        <Chalk dim=true>
          "oo"
          <Chalk color=Green> "hello \027[4mworld!" </Chalk>
          "unchalked o_O"
        </Chalk>
        ++ " reset to prevent underlines on the rest of stdout \027[0m";
      expect.string(testCase).toMatchSnapshot();
    },
  );
});
describe("Chalk.length fuzz testing", ({test}) => {
  let allChalkers = [|
    modifier.bold,
    modifier.dim,
    modifier.italic,
    modifier.underline,
    modifier.inverse,
    modifier.hidden,
    modifier.strikethrough,
    color.black,
    color.red,
    color.green,
    color.yellow,
    color.blue,
    color.magenta,
    color.cyan,
    color.white,
    color.blackBright,
    color.redBright,
    color.greenBright,
    color.yellowBright,
    color.blueBright,
    color.magentaBright,
    color.cyanBright,
    color.whiteBright,
    bg.black,
    bg.red,
    bg.green,
    bg.yellow,
    bg.blue,
    bg.magenta,
    bg.cyan,
    bg.white,
    bg.blackBright,
    bg.redBright,
    bg.greenBright,
    bg.yellowBright,
    bg.blueBright,
    bg.magentaBright,
    bg.cyanBright,
    bg.whiteBright,
  |];
  let n = Array.length(allChalkers);
  let getRandomChalker = () => allChalkers[Random.int(n)];
  /* Get a random word, apply some chalkers, then make sure length works */
  for (i in 1 to 1000) {
    test(
      "length fuzz testing: " ++ string_of_int(i),
      ({expect}) => {
        let parts =
          Array.make(Random.int(30), "")
          |> Array.map(_ => Lorem.getRandomWord())
          |> Array.to_list;
        let noColorWord = parts |> String.concat("");
        let colorWord =
          parts
          |> List.map(part => {
               let part =
                 Array.make(Random.int(5), "")
                 |> Array.to_list
                 |> List.map(_ => getRandomChalker())
                 |> List.fold_left((part, chalker) => chalker(part), part);
               part;
             })
          |> String.concat("");
        let expectedLength = noColorWord |> String.length;
        let actualNoColor = noColorWord |> Chalk.length;
        let actualColor = colorWord |> Chalk.length;
        expect.int(actualNoColor).toBe(expectedLength);
        expect.int(actualColor).toBe(expectedLength);
      },
    );
  };
});
