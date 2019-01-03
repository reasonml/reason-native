/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestFramework;
open TestRunnerOutputSnapshotTest;
open Rely.Describe;

module type TestConfiguration = {
  type t('a);
  type matchersWithNot('a);
  let ofList: list('a) => t('a);
  let expectPath: (Rely.Test.testUtils('a), t('b)) => matchersWithNot('b);
};

type defaultEqualityTestCase =
  | DefaultEqualityTestCase(list('a), list('a), string)
    : defaultEqualityTestCase;

type customEqualityTestCase =
  | CustomEqualityTestCase(list('a), list('a), ('a, 'a) => bool, string)
    : customEqualityTestCase;

type defaulteEqualityContainsTestCase =
  | DefaultEqualityContainsTestCase(list('a), 'a, string)
    : defaulteEqualityContainsTestCase;
type customEqualityContainsTestCase =
  | CustomEqualityContainsTestCase(list('a), 'a, ('a, 'a) => bool, string)
    : customEqualityContainsTestCase;
type simplePersonRecord = {
  name: string,
  age: int,
};

let comparePeople = (p1, p2) => p1.name == p2.name && p1.age == p2.age;

module Make =
       (
         Collection: Rely.CollectionMatchers.Collection,
         T:
           TestConfiguration with
             type t('a) = Collection.t('a) and
             type matchersWithNot('a) =
               Rely.CollectionMatchers.Make(Collection).matchersWithNot('a),
       ) => {
  let collectionName = Collection.collectionName;
  describe(
    "Rely " ++ collectionName ++ " matchers",
    ({describe}) => {
      describe("toBeEmpty", describeUtils => {
        let {describe, test} = describeUtils;
        let emptyCollection = T.ofList([]);
        test("empty should be empty", t =>
          T.expectPath(t, emptyCollection).toBeEmpty()
        );
        test("non empty should not be empty", t =>
          T.expectPath(t, T.ofList(["something"])).not.toBeEmpty()
        );
        testRunnerOutputSnapshotTest(
          "expect." ++ collectionName ++ ".toBeEmpty output",
          describeUtils,
          ({test}) =>
          test("something should be empty failure output", t =>
            T.expectPath(t, T.ofList(["something"])).toBeEmpty()
          )
        );
        testRunnerOutputSnapshotTest(
          "expect." ++ collectionName ++ ".not.toBeEmpty output",
          describeUtils,
          ({test}) =>
          test("empty should not be empty failure output", t =>
            T.expectPath(t, T.ofList([])).not.toBeEmpty()
          )
        );
      });

      describe("toEqual", describeUtils => {
        let {describe, test} = describeUtils;
        let defaultEqualityTestCases = [
          DefaultEqualityTestCase([], [], "empty " ++ collectionName),
          DefaultEqualityTestCase([1, 2], [1, 2], "int " ++ collectionName),
          DefaultEqualityTestCase(
            [1.0, 2.0],
            [1.0, 2.0],
            "float " ++ collectionName,
          ),
          DefaultEqualityTestCase(
            ["bacon", "delicious"],
            ["bacon", "delicious"],
            "string " ++ collectionName,
          ),
        ];
        describe(
          "default equality " ++ collectionName ++ " test cases", ({test}) =>
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
            "empty " ++ collectionName,
          ),
          CustomEqualityTestCase(
            [1],
            [41],
            (a, b) => abs(a - b) < 42,
            "within 42",
          ),
        ];
        describe(
          "custom equality " ++ collectionName ++ " test cases", ({test}) =>
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
          "default inequality " ++ collectionName ++ " test cases", ({test}) =>
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
        testRunnerOutputSnapshotTest(
          "expect." ++ collectionName ++ ".toEqual failure output",
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
          "expect." ++ collectionName ++ ".not.toEqual failure output",
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

      describe("toContainEqual", describeUtils => {
        let {describe, test} = describeUtils;
        let defaultEqualityContainsTestCases = [
          DefaultEqualityContainsTestCase([1, 2, 3], 1, "integer contains"),
          DefaultEqualityContainsTestCase(
            ["foo", "bar"],
            "foo",
            "string contains",
          ),
        ];
        let customEqualityContainsTestCases = [
          CustomEqualityContainsTestCase(
            [1.0, 2.0],
            1.2,
            (a, b) => abs_float(a -. b) < 0.3,
            "float comparsion",
          ),
          CustomEqualityContainsTestCase(
            [{name: "Sally", age: 42}, {name: "Steve", age: 37}],
            {name: "Steve", age: 37},
            comparePeople,
            "record comparison",
          ),
        ];
        describe(
          "expect." ++ collectionName ++ ".toContainEqual",
          ({test}) => {
            defaultEqualityContainsTestCases
            |> List.iter(testCase =>
                 switch (testCase) {
                 | DefaultEqualityContainsTestCase(actual, item, testName) =>
                   test(testName, t =>
                     T.expectPath(t, T.ofList(actual)).toContainEqual(item)
                   )
                 }
               );
            customEqualityContainsTestCases
            |> List.iter(testCase =>
                 switch (testCase) {
                 | CustomEqualityContainsTestCase(
                     actual,
                     item,
                     equals,
                     testName,
                   ) =>
                   test(testName, t =>
                     T.expectPath(t, T.ofList(actual)).toContainEqual(
                       ~equals,
                       item,
                     )
                   )
                 }
               );
          },
        );

        let doesNotContainDefaultEqualityTestCases = [
          DefaultEqualityContainsTestCase([], 7, "empty " ++ collectionName),
          DefaultEqualityContainsTestCase([1, 2], 7, "missing element"),
        ];

        let doesNotContainCustomEqualityTestCases = [
          CustomEqualityContainsTestCase(
            [1.0, 2.0],
            1.2,
            (a, b) => abs_float(a -. b) < 0.1,
            "float comparsion",
          ),
          CustomEqualityContainsTestCase(
            [{name: "Sally", age: 42}, {name: "Steve", age: 37}],
            {name: "Eve", age: 35},
            comparePeople,
            "record comparison",
          ),
        ];
        describe(
          "expect." ++ collectionName ++ ".not.toContainEqual",
          ({test}) => {
            doesNotContainDefaultEqualityTestCases
            |> List.iter(testCase =>
                 switch (testCase) {
                 | DefaultEqualityContainsTestCase(actual, item, testName) =>
                   test(testName, t =>
                     T.expectPath(t, T.ofList(actual)).not.toContainEqual(
                       item,
                     )
                   )
                 }
               );
            doesNotContainCustomEqualityTestCases
            |> List.iter(testCase =>
                 switch (testCase) {
                 | CustomEqualityContainsTestCase(
                     actual,
                     item,
                     equals,
                     testName,
                   ) =>
                   test(testName, t =>
                     T.expectPath(t, T.ofList(actual)).not.toContainEqual(
                       ~equals,
                       item,
                     )
                   )
                 }
               );
          },
        );

        testRunnerOutputSnapshotTest(
          "expect." ++ collectionName ++ ".toContainEqual failure output",
          describeUtils,
          ({test}) =>
          doesNotContainDefaultEqualityTestCases
          |> List.iter(testCase =>
               switch (testCase) {
               | DefaultEqualityContainsTestCase(actual, item, testName) =>
                 test(testName, t =>
                   T.expectPath(t, T.ofList(actual)).toContainEqual(item)
                 )
               }
             )
        );

        testRunnerOutputSnapshotTest(
          "expect." ++ collectionName ++ ".not.toContainEqual failure output",
          describeUtils,
          ({test}) => {
            defaultEqualityContainsTestCases
            |> List.iter(testCase =>
                 switch (testCase) {
                 | DefaultEqualityContainsTestCase(actual, item, testName) =>
                   test(testName, t =>
                     T.expectPath(t, T.ofList(actual)).not.toContainEqual(
                       item,
                     )
                   )
                 }
               );
            customEqualityContainsTestCases
            |> List.iter(testCase =>
                 switch (testCase) {
                 | CustomEqualityContainsTestCase(
                     actual,
                     item,
                     equals,
                     testName,
                   ) =>
                   test(testName, t =>
                     T.expectPath(t, T.ofList(actual)).not.toContainEqual(
                       ~equals,
                       item,
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
