/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestFramework;
open TestRunnerOutputSnapshotTest;

type testRecord = {
  int,
  string,
};

describe("equals matcher", describeUtils => {
  let describe = describeUtils.describe;

  describe("equal things", ({test}) => {
    test("ints", ({expect}) =>
      expect.equal(1, 1)
    );
    test("strings", ({expect}) =>
      expect.equal("hello", "hello")
    );
    test("records", ({expect}) =>
      expect.equal({int: 7, string: "7"}, {int: 7, string: "7"})
    );
    test("floats with custom equality", ({expect}) => {
      let equals = (f1, f2) => f1 -. f2 < 0.001;
      expect.equal(~equals, 1., 1.000001);
    });
  });

  describe("unequal things", ({test}) => {
    test("ints", ({expect}) =>
      expect.notEqual(1, 2)
    );
    test("strings", ({expect}) =>
      expect.notEqual("hello", "world")
    );
    test("records", ({expect}) =>
      expect.notEqual({int: 7, string: "7"}, {int: 42, string: "42"})
    );
    test("floats with custom equality", ({expect}) => {
      let equals = (f1, f2) => f1 -. f2 < 0.001;
      expect.notEqual(~equals, 1., 1.1);
    });
  });

  testRunnerOutputSnapshotTest(
    "equals failure output",
    describeUtils,
    ({test}) => {
      test("ints", ({expect}) =>
        expect.equal(1, 2)
      );
      test("strings", ({expect}) =>
        expect.equal("hello", "world")
      );
      test("records", ({expect}) =>
        expect.equal({int: 7, string: "7"}, {int: 42, string: "42"})
      );
      test("floats with custom equality", ({expect}) => {
        let equals = (f1, f2) => f1 -. f2 < 0.001;
        expect.equal(~equals, 1., 1.1);
      });
      test("floats without custom equality", ({expect}) => {
        expect.equal(1., 1.1);
      });
    },
  );

  testRunnerOutputSnapshotTest(
    "not equals failure output",
    describeUtils,
    ({test}) => {
      test("ints", ({expect}) =>
        expect.notEqual(1, 1)
      );
      test("strings", ({expect}) =>
        expect.notEqual("hello", "hello")
      );
      test("records", ({expect}) =>
        expect.notEqual({int: 7, string: "7"}, {int: 7, string: "7"})
      );
      test("floats with custom equality", ({expect}) => {
        let equals = (f1, f2) => f1 -. f2 < 0.001;
        expect.notEqual(~equals, 1., 1.000001);
      });
    },
  );
});
