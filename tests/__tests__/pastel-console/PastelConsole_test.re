/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open TestFramework;

type user = {
  name: string,
  age: int,
};

let captureOutput = fn =>
  Utils.withHumanReadablePastel(() =>
    IO.captureOutput(() => {
      /* Initialize pastel console before running. */
      PastelConsole.init();
      Console.ObjectPrinter.setPrintWidth(80);
      fn() /* Reset the console when we finish. */;
      Console.currentGlobalConsole.contents = Console.defaultGlobalConsole;
    })
  );

describe("Pastel Console", ({test}) => {
  test("Primitive types", ({expect}) => {
    let (stdout, stderr, _) =
      captureOutput(() => {
        Console.log(4.6);
        Console.log("This is a string");
        Console.log(a => a + 1);
      });
    let out = Utils.allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });

  test("Lists", ({expect}) => {
    let (stdout, stderr, _) =
      captureOutput(() => {
        Console.log(["this", "is", "a", "list", "of", "strings"]);
        Console.log([4.9, 5.0, 999.99]);
      });
    let out = Utils.allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });

  test("Records", ({expect}) => {
    let (stdout, stderr, _) =
      captureOutput(() => {
        Console.log({name: "joe", age: 100});
        Console.log([{name: "joe", age: 100}, {name: "sue", age: 18}]);
      });
    let out = Utils.allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });

  test("List of closures", ({expect}) => {
    let (stdout, stderr, _) =
      captureOutput(() => Console.log([_a => 1, _b => 2, _c => 3]));
    let out = Utils.allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });

  test("Array of strings", ({expect}) => {
    let (stdout, stderr, _) =
      captureOutput(() => Console.log([|"a", "b", "c"|]));
    let out = Utils.allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });

  test("Array of floats", ({expect}) => {
    let (stdout, stderr, _) =
      captureOutput(() => Console.log([|2.3, 8.9, 9.0|]));
    let out = Utils.allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });

  test("Max depth", ({expect}) => {
    let (stdout, stderr, _) =
      captureOutput(() => {
        Console.ObjectPrinter.setMaxDepth(3);
        let rec maxDepthHit = [[[[2]]], [[[]]], [[]]];
        Console.error(maxDepthHit);
      });
    let out = Utils.allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });

  test("Max length", ({expect}) => {
    let (stdout, stderr, _) =
      captureOutput(() => {
        Console.ObjectPrinter.setMaxLength(8);
        let rec maxLengthHit = [2, 3, 4, 5, 6];
        Console.error(maxLengthHit);
      });
    let out = Utils.allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });

  test("Circular list", ({expect}) => {
    let (stdout, stderr, _) =
      captureOutput(() => {
        let rec circularList = [
          "circular",
          "list",
          "should",
          "not",
          "error",
          "but",
          "detect",
          "length-exceeded",
          ...circularList,
        ];
        Console.error(circularList);
      });
    let out = Utils.allOut(stdout, stderr);
    expect.string(out).toMatchSnapshot();
  });
});
