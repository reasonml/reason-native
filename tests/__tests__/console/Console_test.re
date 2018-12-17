/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open TestFramework;

let allOut = (stdout, stderr) =>
  "===== Standard Out =====\n"
  ++ stdout
  ++ "\n===== Standard Err =====\n"
  ++ stderr
  ++ "\n========================\n";

describe("Console", ({test}) => {
  test("Basic output", ({expect}) => {
    let (stdout, stderr, _) =
      IO.captureOutput(() => {
        Console.log("Console.log output of a string will not print quotes");
        Console.error(
          "Console.error output of a string will not print quotes",
        );
        Console.warn("Console.warn output of a string will not print quotes");
        Console.debug(
          "Console.debug output of a string will not print quotes",
        );
      });
    let out = allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });

  test("Output without newlines", ({expect}) => {
    let (stdout, stderr, _) =
      IO.captureOutput(() => {
        Console.out(
          "Console.out of a string will not print quotes, or the newline.",
        );
        Console.out(" See how this line continues.");
      });
    let out = allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });

  test("Basic lists and variants", ({expect}) => {
    let (stdout, stderr, _) =
      IO.captureOutput(() => {
        Console.log([
          Some(["console"]),
          Some(["dot"]),
          Some(["log"]),
          None,
        ]);
        Console.warn([
          Some(["console"]),
          Some(["dot"]),
          Some(["warn"]),
          None,
        ]);
        Console.error([
          Some(["console"]),
          Some(["dot"]),
          Some(["error"]),
          None,
        ]);
      });
    let out = allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });

  test("Max depth", ({expect}) => {
    let (stdout, stderr, _) =
      IO.captureOutput(() => {
        Console.ObjectPrinter.setMaxDepth(3);
        let rec maxDepthHit = [[[[2, 3]]], [[[5, 6]]]];
        Console.error(maxDepthHit);
      });
    let out = allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });

  test("Max length", ({expect}) => {
    let (stdout, stderr, _) =
      IO.captureOutput(() => {
        Console.ObjectPrinter.setMaxLength(8);
        let rec maxLengthHit = [2, 3, 4, 5, 6];
        Console.error(maxLengthHit);
      });
    let out = allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });

  test("Circular lists", ({expect}) => {
    let (stdout, stderr, _) =
      IO.captureOutput(() => {
        let rec circularList = [
          Some(["circular"]),
          Some(["list"]),
          Some(["should"]),
          Some(["not"]),
          Some(["error"]),
          Some(["but"]),
          Some(["detect"]),
          Some(["length-exceeded"]),
          ...circularList,
        ];
        Console.error(circularList);
      });
    let out = allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });
});
