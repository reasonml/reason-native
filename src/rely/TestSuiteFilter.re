/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

let filterTestsByName =
    (tests: list(TestSuite.test('ext, 'env)), nameFilter) => {
  tests
  |> List.filter((test: TestSuite.test('ext, 'env)) =>
       nameFilter(test.name)
     );
};

let rec filterDescribeByName =
        (describeRecord: TestSuite.describeRecord('a, 'b), nameFilter) => {
  let {TestSuite.name, tests, describes, mode} = describeRecord;

  if (nameFilter(name)) {
    Some(describeRecord);
  } else {
    let tests = filterTestsByName(tests, nameFilter);
    let describes = filterDescribesByName(describes, nameFilter);

    switch (tests, describes) {
    | ([], []) => None
    | _ =>
      Some({name, tests, describes, mode}: TestSuite.describeRecord('a, 'b))
    };
  };
}
and filterDescribesByName = (describes, nameFilter) => {
  describes
  |> List.map(describe => filterDescribeByName(describe, nameFilter))
  |> List.fold_left(
       (acc, optDescribe) =>
         switch (optDescribe) {
         | Some(describe) => [describe, ...acc]
         | None => acc
         },
       [],
     );
};

let filterSuiteByName = (testSuite, nameFilter) => {
  let TestSuite.TestSuite(describeRecord, extensionFn, lifeCycle, context) = testSuite;
  switch (filterDescribeByName(describeRecord, nameFilter)) {
  | Some(record) =>
    Some(TestSuite.TestSuite(record, extensionFn, lifeCycle, context))
  | None => None
  };
};

let filterTestSuitesByName = (testSuites: list(TestSuite.t), nameFilter) => {
  testSuites
  |> List.fold_left(
       (acc, suite) => {
         switch (filterSuiteByName(suite, nameFilter)) {
         | Some(suite) => [suite, ...acc]
         | None => acc
         }
       },
       [],
     );
};

let filterTestSuitesByRegex = (testSuites, regex) => {
  let filterFn = name => Re.Pcre.pmatch(regex, name);
  filterTestSuitesByName(testSuites, filterFn);
};
