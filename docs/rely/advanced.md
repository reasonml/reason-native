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

## Running with [Custom Reporters](https://github.com/facebookexperimental/reason-native/blob/master/src/rely/Reporter.re)

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

## Running with a jUnit reporter for CI

```
TestFramework.run(
  Rely.RunConfig.(
    initialize()
    |> withReporters([
         /* the Default reporter prints terminal output, the jUnit reporter outputs junit xml to the provided filepath */
         Default,
         JUnit("./junit.xml"),
       ])
  ),
);
```
