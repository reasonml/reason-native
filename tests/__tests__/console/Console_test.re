/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open TestFramework;

type mockedType =
  | Foo{a: int}
  | Bar{x: string};

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
    let out = Utils.allOut(stdout, stderr);
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
    let out = Utils.allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });

  test("Primitive types", ({expect}) => {
    let (stdout, stderr, _) =
      IO.captureOutput(() => {
        Console.log(4.6);
        Console.log("This is a string");
        Console.log(a => a + 1);
      });
    let out = Utils.allOut(stdout, stderr);
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
    let out = Utils.allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });

  test("Max depth", ({expect}) => {
    let (stdout, stderr, _) =
      IO.captureOutput(() => {
        Console.ObjectPrinter.setMaxDepth(3);
        let rec maxDepthHit = [[[[2, 3]]], [[[5, 6]]]];
        Console.error(maxDepthHit);
      });
    let out = Utils.allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });

  test("Max length", ({expect}) => {
    let (stdout, stderr, _) =
      IO.captureOutput(() => {
        Console.ObjectPrinter.setMaxLength(8);
        let rec maxLengthHit = [2, 3, 4, 5, 6];
        Console.error(maxLengthHit);
      });
    let out = Utils.allOut(stdout, stderr);
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
    let out = Utils.allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });

  test("Records", ({expect}) => {
    let (stdout, stderr, _) =
      IO.captureOutput(() => {
        Console.log(Foo({a: 0}));
        Console.log(Bar({x: "some string"}));
      });
    let out = Utils.allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });
});

describe("Console.Pipe", ({test}) =>
  test("Basic output", ({expect}) => {
    let (stdout, stderr, _) =
      IO.captureOutput(() => {
        let foo = [1, 2, 3];
        let _ =
          foo
          |> List.map(x => x - 1)
          |> Console.Pipe.log
          |> List.map(x => x * 2)
          |> Console.Pipe.log;
        ();
      });
    let out = Utils.allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  })
);
