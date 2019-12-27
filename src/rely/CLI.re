type cliArgs = {
  updateSnapshots: option(bool),
  ciMode: option(bool),
  onlyPrintDetailsForFailedSuites: option(bool),
  filter: option(Re.Pcre.regexp),
};

let filterTestsByRegex = (tests: list(TestSuite.test('ext, 'env)), regex) => {
  tests
  |> List.filter((test: TestSuite.test('ext, 'env)) =>
       Re.Pcre.pmatch(regex, test.name)
     );
};

let rec filterDescribeByRegex =
        (describeRecord: TestSuite.describeRecord('a, 'b), regex) => {
  let {TestSuite.name, tests, describes, mode} = describeRecord;

  if (Re.Pcre.pmatch(regex, name)) {
    Some(describeRecord);
  } else {
    let tests = filterTestsByRegex(tests, regex);
    let describes = filterDescribesByRegex(describes, regex);

    switch (tests, describes) {
    | ([], []) => None
    | _ =>
      Some({name, tests, describes, mode}: TestSuite.describeRecord('a, 'b))
    };
  };
}
and filterDescribesByRegex = (describes, regex) => {
  describes
  |> List.map(describe => filterDescribeByRegex(describe, regex))
  |> List.fold_left(
       (acc, optDescribe) =>
         switch (optDescribe) {
         | Some(describe) => [describe, ...acc]
         | None => acc
         },
       [],
     );
};

let filterSuiteByRegex = (testSuite, regex) => {
  let TestSuite.TestSuite(describeRecord, extensionFn, lifeCycle, context) = testSuite;
  switch (filterDescribeByRegex(describeRecord, regex)) {
  | Some(record) =>
    Some(TestSuite.TestSuite(record, extensionFn, lifeCycle, context))
  | None => None
  };
};

let filterTestSuitesByRegex =
    (testSuites: list(RelyInternal.TestSuite.t), regex: Re.Pcre.regexp) => {
  testSuites
  |> List.fold_left(
       (acc, suite) => {
         switch (filterSuiteByRegex(suite, regex)) {
         | Some(suite) => [suite, ...acc]
         | None => acc
         }
       },
       [],
     );
};
