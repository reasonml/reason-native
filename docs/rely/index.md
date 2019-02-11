---
id: index
title: Rely Introduction
sidebar_label: Introduction
---

Rely is a strongly typed, [Jest-like](https://jestjs.io/) native test framework with blazing fast performance 🔥🔥🔥.

It comes with built in support for both traditional and snapshot testing, and bootstraps tests in **milliseconds**.

```re
describe("Example", ({test}) => {  
  test("ints", ({expect}) =>
    expect.int(7).toBe(5)
  );
  test("bools", ({expect}) =>
    expect.bool(true).not.toBe(false)
  );
});
```
```sh-stacked
[97mExample[39m
[2m[0/2] Pending[22m  [2m[1/2] Passed[22m  [31m[1/2] Failed[39m
[1m[31m  • Example › ints
[39m[22m    [2mexpect.int([22m[31mreceived[39m[2m).toBe([22m[32mexpected[39m[2m)[22m

    Expected: [32m5[39m
    Received: [31m7[39m
```
```sh-stacked
Time: 0.00s user
      0.00s system
      82% cpu
      0.008 total
```
