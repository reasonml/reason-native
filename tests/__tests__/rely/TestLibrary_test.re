/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestFramework;
open TestFrameworkBuilder;

let runCombineResult = (combineResult: Rely.combineResult) => {
  let numRunTests = ref(0);

  module Reporter =
    TestReporter.Make({});

  Reporter.onRunComplete(res => {
    numRunTests := numRunTests^ + res.numFailedTests;
    numRunTests := numRunTests^ + res.numPassedTests;
    numRunTests := numRunTests^ + res.numSkippedTests;
  });

  combineResult.run(
    Rely.RunConfig.(
      initialize()
      |> withReporters([Custom(Reporter.reporter)])
      |> onTestFrameworkFailure(() => ())
    ),
  );
  numRunTests^;
};

describe("Rely Test Library", ({test}) => {
  test("combine result with empty list should not run anything", ({expect}) => {
    module TestFrameworkInternal =
      TestFrameworkBuilder.Build({
        let action = b => b;
      });

    let testLibrary = TestFrameworkInternal.toLibrary();
    let combineResult = Rely.combine([testLibrary]);

    let numRunTests = runCombineResult(combineResult);

    expect.int(numRunTests).toBe(0);
    ();
  });
  test(
    ".combineresult with a single testLibrary should run all tests from that library",
    ({expect}) => {
      module TestFrameworkInternal =
        TestFrameworkBuilder.Build({
          let action = builder =>
            builder
            |> withTestSuite(
                 TestSuiteBuilder.(
                   TestSuite(
                     init("test suite")
                     |> withPassingTests(2)
                     |> withFailingTests(1),
                   )
                 ),
               );
        });

      let testLibrary = TestFrameworkInternal.toLibrary();

      let combineResult = Rely.combine([testLibrary]);

      let numRunTests = runCombineResult(combineResult);
      expect.int(numRunTests).toBe(3);
      ();
    },
  );
  test(
    ".combineresult with multiple testLibraries should run all tests from all library",
    ({expect}) => {
      module TestFramework1 =
        TestFrameworkBuilder.Build({
          let action = builder =>
            builder
            |> withTestSuite(
                 TestSuiteBuilder.(
                   init("test suite")
                   |> withPassingTests(2)
                   |> withFailingTests(1)
                   |> build
                 ),
               );
        });

      module TestFramework2 =
        TestFrameworkBuilder.Build({
          let action = builder =>
            builder
            |> withTestSuite(
                 TestSuiteBuilder.(
                   init("test suite")
                   |> withPassingTests(5)
                   |> withFailingTests(2)
                   |> build
                 ),
               );
        });

      module TestFramework3 =
        TestFrameworkBuilder.Build({
          let action = builder =>
            builder
            |> withTestSuite(
                 TestSuiteBuilder.(
                   init("test suite")
                   |> withPassingTests(2)
                   |> withFailingTests(3)
                   |> build
                 ),
               );
        });

      let combineResult =
        Rely.combine([
          TestFramework1.toLibrary(),
          TestFramework2.toLibrary(),
          TestFramework3.toLibrary(),
        ]);

      let numRunTests = runCombineResult(combineResult);
      expect.int(numRunTests).toBe(15);
      ();
    },
  );
  test("testLibrary from combine result should be combinable", ({expect}) => {
    module TestFramework1 =
      TestFrameworkBuilder.Build({
        let action = builder =>
          builder
          |> withTestSuite(
               TestSuiteBuilder.(
                 init("test suite")
                 |> withPassingTests(2)
                 |> withFailingTests(1)
                 |> build
               ),
             );
      });

    module TestFramework2 =
      TestFrameworkBuilder.Build({
        let action = builder =>
          builder
          |> withTestSuite(
               TestSuiteBuilder.(
                 init("test suite")
                 |> withPassingTests(5)
                 |> withFailingTests(2)
                 |> build
               ),
             );
      });

    module TestFramework3 =
      TestFrameworkBuilder.Build({
        let action = builder =>
          builder
          |> withTestSuite(
               TestSuiteBuilder.(
                 init("test suite")
                 |> withPassingTests(2)
                 |> withFailingTests(3)
                 |> build
               ),
             );
      });

    let combineResult1 =
      Rely.combine([TestFramework1.toLibrary(), TestFramework2.toLibrary()]);

    let combineResult2 =
      Rely.combine([combineResult1.testLibrary, TestFramework3.toLibrary()]);

    let numRunTests = runCombineResult(combineResult2);
    expect.int(numRunTests).toBe(15);
    ();
  });
});
