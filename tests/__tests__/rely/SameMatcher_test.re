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

describe("same matcher", describeUtils => {
  let describe = describeUtils.describe;

  describe("same things", ({test}) => {
    test("ints", ({expect}) =>
      expect.same(1, 1)
    );
    test("strings", ({expect}) => {
      let s = "hello";
      expect.same(s, s);
    });
    test("records", ({expect}) => {
      let record = {int: 7, string: "7"};
      expect.same(record, record);
    });
  });

  describe("not same things", ({test}) => {
    test("ints", ({expect}) =>
      expect.notSame(1, 2)
    );
    test("strings", ({expect}) =>
      expect.notSame("hello", "hello")
    );
    test("records", ({expect}) =>
      expect.notSame({int: 7, string: "7"}, {int: 7, string: "7"})
    );
  });

  testRunnerOutputSnapshotTest(
    "same failure output",
    describeUtils,
    ({test}) => {
        test("ints", ({expect}) =>
        expect.same(1, 2)
      );
      test("strings", ({expect}) =>
        expect.same("hello", "hello")
      );
      test("floats", ({expect}) =>
        expect.same(1., 2.)
      );
      test("records", ({expect}) =>
        expect.same({int: 7, string: "7"}, {int: 7, string: "7"})
      );
    },
  );

  testRunnerOutputSnapshotTest(
    "not same failure output",
    describeUtils,
    ({test}) => {
        test("ints", ({expect}) =>
        expect.notSame(1, 1)
      );
      test("strings", ({expect}) => {
        let s = "hello";
        expect.notSame(s, s);
      });
      test("records", ({expect}) => {
        let record = {int: 7, string: "7"};
        expect.notSame(record, record);
      });
    },
  );
});
