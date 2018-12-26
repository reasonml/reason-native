/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestFramework;
open TestRunnerOutputSnapshotTest;

type defaultEqualityTestCase =
  | DefaultEqualityTestCase(list('a), list('a), string)
    : defaultEqualityTestCase;

type customEqualityTestCase =
  | CustomEqualityTestCase(list('a), list('a), ('a, 'a) => bool, string)
    : customEqualityTestCase;

describe("Rely list matchers", ({describe}) => {
  describe("to be empty", ({test}) =>
    test("empty should be empty", ({expect}) =>
      expect.list([]).toBeEmpty()
    )
  );

  describe("not to be empty", ({test}) =>
    test("not empty should not be empty", ({expect}) =>
      expect.list(["something"]).not.toBeEmpty()
    )
  );

  let defaultEqualityTestCases = [
    DefaultEqualityTestCase([], [], "empty list"),
    DefaultEqualityTestCase([1, 2], [1, 2], "int list"),
    DefaultEqualityTestCase([1.0, 2.0], [1.0, 2.0], "float list"),
    DefaultEqualityTestCase(
      ["bacon", "delicious"],
      ["bacon", "delicious"],
      "string list",
    ),
  ];
  describe("Default equality list test cases", ({test}) =>
    defaultEqualityTestCases
    |> List.iter(testCase =>
         switch (testCase) {
         | DefaultEqualityTestCase(actual, expected, testName) =>
           test(testName, ({expect}) =>
             expect.list(actual).toEqual(expected)
           )
         }
       )
  );
  let customEqualityTestCases = [
    CustomEqualityTestCase([], [], (a, b) => false, "empty list"),
    CustomEqualityTestCase(
      [1],
      [41],
      (a, b) => abs(a - b) < 42,
      "within 42",
    ),
  ];
  describe("Custom equality list test cases", ({test}) =>
    customEqualityTestCases
    |> List.iter(testCase =>
         switch (testCase) {
         | CustomEqualityTestCase(actual, expected, equals, testName) =>
           test(testName, ({expect}) =>
             expect.list(actual).toEqual(~equals, expected)
           )
         }
       )
  );

  describe("Failure output", describeUtils => {
    testRunnerOutputSnapshotTest(
      "expect.list.toBeEmpty output", describeUtils, ({test}) =>
      test("something should be empty failure output", ({expect}) =>
        expect.list(["something"]).toBeEmpty()
      )
    );
    testRunnerOutputSnapshotTest(
      "expect.list.not.toBeEmpty output", describeUtils, ({test}) =>
      test("empty should not be empty failure output", ({expect}) =>
        expect.list([]).not.toBeEmpty()
      )
    );
    testRunnerOutputSnapshotTest(
      "expect.list.toeEualfailure output",
      describeUtils,
      ({test}) => {
        let failureTestCases = [
          DefaultEqualityTestCase(
            [1, 2],
            [1, 2, 3],
            "actual shorter than expected",
          ),
          DefaultEqualityTestCase(
            [1, 2, 3],
            [1, 2],
            "actual longer than expected",
          ),
          DefaultEqualityTestCase(
            ["mismatched"],
            ["values"],
            "mismatched values",
          ),
        ];
        failureTestCases
        |> List.iter(testCase =>
             switch (testCase) {
             | DefaultEqualityTestCase(actual, expected, testName) =>
               test(testName, ({expect}) =>
                 expect.list(actual).toEqual(expected)
               )
             }
           );
      },
    );
    testRunnerOutputSnapshotTest(
      "expect.list.not.toEqual failure output",
      describeUtils,
      ({test}) => {
        defaultEqualityTestCases
        |> List.iter(testCase =>
             switch (testCase) {
             | DefaultEqualityTestCase(actual, expected, testName) =>
               test(testName, ({expect}) =>
                 expect.list(actual).not.toEqual(expected)
               )
             }
           );
        customEqualityTestCases
        |> List.iter(testCase =>
             switch (testCase) {
             | CustomEqualityTestCase(actual, expected, equals, testName) =>
               test(testName, ({expect}) =>
                 expect.list(actual).not.toEqual(~equals, expected)
               )
             }
           );
      },
    );
  });
});
