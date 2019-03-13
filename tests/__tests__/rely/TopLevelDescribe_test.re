/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestFramework;

describe("top level describe test", ({test}) => {
  test(
    "calling top level describe within top level describe should error",
    ({expect}) => {
    module InnerTestFramework =
      Rely.Make({
        let config =
          Rely.TestFrameworkConfig.initialize({
            snapshotDir: "unused",
            projectDir: "",
          });
      });
    InnerTestFramework.describe("this is a top-level describe", ({test}) =>
      InnerTestFramework.describe(
        "invalid inner top-level describe", ({test}) =>
        test("Inner test", ({expect}) =>
          expect.bool(true).toBe(false)
        )
      )
    );
    expect.fn(() =>
      InnerTestFramework.run(
        Rely.RunConfig.(
          initialize()
          |> onTestFrameworkFailure(() => ())
          |> withReporters([])
        ),
      )
    ).
      toThrowException(
      Rely.InvalidWhileRunning(
        "TestFramework.describe cannot be nested, instead use the describe supplied by the parent describe block, e.g. describe(\"parent describe\", {test, describe} => { ... });",
      ),
    );
  });
  test(
    "calling top level describeSkip within top level describe should error",
    ({expect}) => {
    module InnerTestFramework =
      Rely.Make({
        let config =
          Rely.TestFrameworkConfig.initialize({
            snapshotDir: "unused",
            projectDir: "",
          });
      });
    InnerTestFramework.describe("this is a top-level describe", ({test}) =>
      InnerTestFramework.describeSkip(
        "invalid inner top-level describeSkip", ({test}) =>
        test("Inner test", ({expect}) =>
          expect.bool(true).toBe(false)
        )
      )
    );
    expect.fn(() =>
      InnerTestFramework.run(
        Rely.RunConfig.(
          initialize()
          |> onTestFrameworkFailure(() => ())
          |> withReporters([])
        ),
      )
    ).
      toThrowException(
      Rely.InvalidWhileRunning(
        "TestFramework.describeSkip cannot be nested, instead use the describe supplied by the parent describe block, e.g. describe(\"parent describe\", {test, describe} => { ... });",
      ),
    );
  });
  ();
});
