---
id: advanced
title: Advanced Topics - Rely
sidebar_label: Advanced
---

## Running with [Custom Run Config](https://github.com/facebookexperimental/reason-native/blob/master/src/rely/RunConfig.re)

```reason
let sampleRunConfig = Rely.RunConfig.initialize()
TestFramework.run(sampleRunConfig);
```

## Running with [Custom Reporters](https://github.com/facebookexperimental/reason-native/blob/master/src/rely/reporters/Reporter.re)

```reason
let myReporter: Rely.Reporter.t = {
  onTestSuiteStart: (testSuite) => {...},
  onTestSuiteResult: (testSuite, aggregatedResult, testSuiteResult) => {...},
  onRunStart: (relyRunInfo) => {...},
  onRunComplete: (aggregatedResult) => {...}
};

let customReporterConfig = Rely.RunConfig.initialize() |> withReporters([
  Custom(myReporter),
  /* not required, but the default terminal reporter can also be included */
  Default
]);

TestFramework.run(customReporterConfig);
```

## Running in continuous integration environments

It is recommended to use something like the following run configuration in CI

```
TestFramework.run(
  Rely.RunConfig.(
    initialize()
    /* causes tests to fail if testOnly or describeOnly are used to prevent
     * accidentally disabling all other tests (as of Rely 2.1.0) */
    |> ciMode(true)
    |> withReporters([
         /* the Default reporter prints terminal output, the jUnit reporter
          * outputs junit xml to the provided filepath, most CI solutions have
          * integration with the junit xml format */
         Default,
         JUnit("./junit.xml"),
       ])
  ),
);
```
