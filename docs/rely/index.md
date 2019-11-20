---
id: index
title: Rely Introduction
sidebar_label: Introduction
---

Rely is a [Jest-inspired](https://jestjs.io/) native Reason testing framework. 

We took the things we loved about Jest (beautiful output, descriptive assertion framework, and snapshot testing), added some type safety, and built a library that can compile and run entire test suites both natively and in JS with [Js_of_ocaml](https://ocsigen.org/js_of_ocaml/3.1.0/manual/overview) in **milliseconds**.


```re
describe("Example", ({test}) => {
  test("ints", ({expect}) =>
    expect.int(1 + 1).toBe(3)
  );
  test("bools", ({expect}) =>
    expect.bool(true).not.toBe(false)
  );
});
```
```sh-stacked
Running 1 test suite

[31m[1m[7m FAIL [27m[22m[39m [97mExample[39m
[1m[31m  • Example › ints[39m[22m

    [2mexpect.int([22m[31mreceived[39m[2m).toBe([22m[32mexpected[39m[2m)[22m

    Expected: [32m3[39m
    Received: [31m2[39m

      [2m 3 ┆ [22m
      [2m 4 ┆ [22m[2mdescribe([22m[32m[2m"Example"[22m[39m[2m, ({test})[22m[31m[2m => [22m[39m[2m{[22m
      [2m 5 ┆ [22m[2m  test([22m[32m[2m"ints"[22m[39m[2m, ({expect})[22m[31m[2m => [22m[39m[2m{[22m
      [31m[2m 6 ┆ [22m[39m[2m    [22m[31m[1m[4mexpect.int(1 + 1).toBe(3)[24m[22m[39m[2m;[22m
      [2m 7 ┆ [22m[2m    ();[22m
      [2m 8 ┆ [22m[2m  });[22m
      [2m 9 ┆ [22m[2m  test([22m[32m[2m"bools"[22m[39m[2m, ({expect})[22m[31m[2m => [22m[39m[2m{[22m

      [2mRaised by primitive operation at [22m[36m./tests/ExampleTest.re:6:4[39m
      [2mCalled from [22m[36m./map.ml:291:20[39m
      [2mCalled from [22m[36m./common/CommonCollections.re:171:8[39m


[97m[1mTest Suites: [22m[39m[31m[1m1 failed[22m[39m, 0 passed, 1 total
[97m[1mTests:       [22m[39m[31m[1m1 failed[22m[39m, [32m[1m1 passed[22m[39m, 2 total
[97m[1mTime:        [22m[39m0.002s
```
