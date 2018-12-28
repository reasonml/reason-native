/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestFramework;
open TestRunnerOutputSnapshotTest;

module type TestConfiguration = {
  type t('a);
  type matchersWithNot('a);
  let ofList: list('a) => t('a);
  let expectPath: (Rely.Test.testUtils('a), t('b)) => matchersWithNot('b);
  let collectionName: string;
};

type defaultEqualityTestCase =
  | DefaultEqualityTestCase(list('a), list('a), string)
    : defaultEqualityTestCase;

type customEqualityTestCase =
  | CustomEqualityTestCase(list('a), list('a), ('a, 'a) => bool, string)
    : customEqualityTestCase;

module Make =
       (
         Collection: Rely.CollectionMatchers.Collection,
         T:
           TestConfiguration with
             type t('a) = Collection.t('a) and
             type matchersWithNot('a) =
               Rely.CollectionMatchers.Make(Collection).matchersWithNot('a),
       ) => {
  let _ = "foo";
  describe(
    "Rely " ++ T.collectionName ++ " matchers",
    ({describe}) => {
      let emptyCollection = T.ofList([]);
      describe(
        "empty " ++ T.collectionName,
        ({test}) => {
          test("empty should be empty", t =>
            T.expectPath(t, emptyCollection).toBeEmpty()
          );
          test("non empty should not be empty", t =>
            T.expectPath(t, T.ofList(["something"])).not.toBeEmpty()
          );
        },
      );
      let defaultEqualityTestCases = [
        DefaultEqualityTestCase([], [], "empty " ++ T.collectionName),
        DefaultEqualityTestCase([1, 2], [1, 2], "int " ++ T.collectionName),
        DefaultEqualityTestCase(
          [1.0, 2.0],
          [1.0, 2.0],
          "float " ++ T.collectionName,
        ),
        DefaultEqualityTestCase(
          ["bacon", "delicious"],
          ["bacon", "delicious"],
          "string " ++ T.collectionName,
        ),
      ];
      describe(
        "default equality " ++ T.collectionName ++ " test cases", ({test}) =>
        defaultEqualityTestCases
        |> List.iter(testCase =>
             switch (testCase) {
             | DefaultEqualityTestCase(actual, expected, testName) =>
               test(testName, t =>
                 T.expectPath(t, T.ofList(actual)).toEqual(
                   T.ofList(expected),
                 )
               )
             }
           )
      );
      let customEqualityTestCases = [
        CustomEqualityTestCase(
          [],
          [],
          (a, b) => false,
          "empty " ++ T.collectionName,
        ),
        CustomEqualityTestCase(
          [1],
          [41],
          (a, b) => abs(a - b) < 42,
          "within 42",
        ),
      ];
      describe(
        "custom equality " ++ T.collectionName ++ " test cases", ({test}) =>
        customEqualityTestCases
        |> List.iter(testCase =>
             switch (testCase) {
             | CustomEqualityTestCase(actual, expected, equals, testName) =>
               test(testName, t =>
                 T.expectPath(t, T.ofList(actual)).toEqual(
                   ~equals,
                   T.ofList(expected),
                 )
               )
             }
           )
      );

      let unequalTestCases = [
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
      describe(
        "default inequality " ++ T.collectionName ++ " test cases", ({test}) =>
        unequalTestCases
        |> List.iter(testCase =>
             switch (testCase) {
             | DefaultEqualityTestCase(actual, expected, testName) =>
               test(testName, t =>
                 T.expectPath(t, T.ofList(actual)).not.toEqual(
                   T.ofList(expected),
                 )
               )
             }
           )
      );
      describe("Failurre output", describeUtils => {
        testRunnerOutputSnapshotTest(
          "expect." ++ T.collectionName ++ ".toBeEmpty output",
          describeUtils,
          ({test}) =>
          test("something should be empty failure output", t =>
            T.expectPath(t, T.ofList(["something"])).toBeEmpty()
          )
        );
        testRunnerOutputSnapshotTest(
          "expect." ++ T.collectionName ++ ".not.toBeEmpty output",
          describeUtils,
          ({test}) =>
          test("empty should not be empty failure output", t =>
            T.expectPath(t, T.ofList([])).not.toBeEmpty()
          )
        );
        testRunnerOutputSnapshotTest(
          "expect." ++ T.collectionName ++ ".toEqualfailure output",
          describeUtils,
          ({test}) =>
          unequalTestCases
          |> List.iter(testCase =>
               switch (testCase) {
               | DefaultEqualityTestCase(actual, expected, testName) =>
                 test(testName, t =>
                   T.expectPath(t, T.ofList(actual)).toEqual(
                     T.ofList(expected),
                   )
                 )
               }
             )
        );
        testRunnerOutputSnapshotTest(
          "expect." ++ T.collectionName ++ ".not.toEqual failure output",
          describeUtils,
          ({test}) => {
            defaultEqualityTestCases
            |> List.iter(testCase =>
                 switch (testCase) {
                 | DefaultEqualityTestCase(actual, expected, testName) =>
                   test(testName, t =>
                     T.expectPath(t, T.ofList(actual)).not.toEqual(
                       T.ofList(expected),
                     )
                   )
                 }
               );
            customEqualityTestCases
            |> List.iter(testCase =>
                 switch (testCase) {
                 | CustomEqualityTestCase(actual, expected, equals, testName) =>
                   test(testName, t =>
                     T.expectPath(t, T.ofList(actual)).not.toEqual(
                       ~equals,
                       T.ofList(expected),
                     )
                   )
                 }
               );
          },
        );
      });
    },
  );
};
