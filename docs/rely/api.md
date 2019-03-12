---
id: api
title: Rely API
sidebar_label: API
---

> Prefer reading code? Check out [Rely.rei](https://github.com/facebookexperimental/reason-native/blob/master/src/rely/Rely.rei)

## Configuration

```reason
/* TestFramework.re */
include Rely.Make({
  let config =
    Rely.TestFrameworkConfig.initialize({
      snapshotDir: "path/to/test/lib/__snapshots__", /* required */
      projectDir: "path/to/your/project" /* required */
    });
});
```

**Running with default CLI config:**
```reason
/* MyLibTest.re */
TestFramework.cli(); /* default config */
```

**Advanced: Running with [custom Run Config](https://github.com/facebookexperimental/reason-native/blob/master/src/rely/RunConfig.re):**
```reason
/* MyLibTest.re */
let sampleRunConfig = RunConfig.initialize()
TestFramework.run(sampleRunConfig); /* custom config */
```

**Running with [custom reporters](https://github.com/facebookexperimental/reason-native/blob/master/src/rely/Reporter.re)**
```reason
let myReporter: Rely.Reporter.t = {
  onTestSuiteStart: (testSuite) => {...},
  onTestSuiteResult: (testSuite, aggregatedResult, testSuiteResult) => {...},
  onRunStart: (relyRunInfo) => {...},
  onRunComplete: (aggregatedResult) => {...}
};

let customReporterConfig = RunConfig.initialize() |> withReporters([
  Custom(myReporter),
  /* not required, but the default terminal reporter can also be included */
  Default
]);

TestFramework.run(customReporterConfig);
```

## Testing
> For a full list of available matchers, visit the [Github Repository](https://github.com/facebookexperimental/reason-native/tree/master/src/rely/matchers)
```reason
/* MyFirstTest.re */
open TestFramework;

describe("my first test suite", ({test, testSkip, describe}) => {
  /* test suite */
  test("all the expects", ({expect}) => {
    /* string type */
    expect.string("a").not.toEqual("b");

    /* file type */
    expect.file("hello.txt").toEqual("Hello world!");

    /* line type */
    expect.lines(["a", "b"]).toEqual("a\nb");

    /* bool type */
    expect.bool(false).toBe(false);

    /* int type */
    expect.int(1 + 1).toBe(2);

    /* float type */
    expect.float(0.1 +. 0.2).toBeCloseTo(2.0);

    /* fn type */
    expect.fn(() => {2;}).not.toThrow();

    /* list type */
    expect.list([]).toBeEmpty();

    /* array type */
    expect.array([|1, 2, 3|]).toContain(1);

    /* polymorphic structural equality */
    expect.equal(1,1);

    /* polymorphic referential equality */
    expect.not.same("hello", "hello");

    /* mock functions, there are constructors for up to 7 args */
    let mock = Mock.mock1((id) => id);
    expect.mock(mock).not.toBeCalled();
  });
  testSkip("incorrect test", ({expect}) => {
    expect.int(1 + 1).toBe(3);
  });
  describe("a nested test suite", ({test}) => {
    test("snapshots", ({expect}) => {
      expect.string("I 💖 Rely").toMatchSnapshot();
    });
  });
});
```
