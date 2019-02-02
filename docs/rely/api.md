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

## Testing
> For a full list of available matchers, visit the [Github Repository](https://github.com/facebookexperimental/reason-native/tree/master/src/rely/matchers)
```reason
/* MyFirstTest.re */
open TestFramework;

describe("my first test suite", ({test, testSkip}) => {
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
  });
  testSkip("incorrect test", ({expect}) => {
    expect.int(1 + 1).toBe(3);
  });
});
```