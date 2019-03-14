let testResultToTestCase = (testResult: Reporter.testResult) => {
  let time =
    switch (testResult.duration) {
    | Some(Seconds(s)) => s
    | None => 0.
    };

  switch (testResult.testStatus) {
  | Passed =>
    Junit.Testcase.pass(
      ~name=testResult.title,
      ~classname=testResult.fullName,
      ~time,
    )
  | Skipped =>
    Junit.Testcase.skipped(
      ~name=testResult.title,
      ~classname=testResult.fullName,
      ~time,
    )
  | Failed(description, _location_opt, message) =>
    Junit.Testcase.failure(
      ~message,
      ~typ="???",
      ~name=testResult.title,
      ~classname=testResult.fullName,
      ~time,
      description,
    )
  | Exception(_e, _location_opt, message) =>
    Junit.Testcase.error(
      ~message,
      ~typ="???",
      ~name=testResult.title,
      ~classname=testResult.fullName,
      ~time,
      "description",
    )
  };
};

let createJUnitReporter = fileName: Reporter.t => {
  let suites = ref([]);

  {
    onTestSuiteStart: _ => (),
    onTestSuiteResult: ({name}, _aggregatedResult, testSuiteResult) => {
      let currentSuite =
        List.map(testResultToTestCase, testSuiteResult.testResults)
        |> (
          cases =>
            Junit.Testsuite.add_testcases(
              cases,
              Junit.Testsuite.make(~name, ()),
            )
        );

      suites := [currentSuite, ...suites^];
    },
    onRunStart: _ => (),
    onRunComplete: _ => Junit.to_file(Junit.make(suites^), fileName),
  };
};
