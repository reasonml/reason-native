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
### `describe`

### `test`
### `expect`