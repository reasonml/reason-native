/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestFramework;

module MakeInnterTestFramework = (()) =>
  Rely.Make({
    let config =
      Rely.TestFrameworkConfig.initialize({
        snapshotDir: "unused",
        projectDir: GetProjectRoot.get(),
      });
  });

open Rely.Test;

describe("Multiple TestFramework.describes", ({test}) => {
  let boolListToString = boolList =>
    boolList |> List.map(string_of_bool) |> String.concat(", ");

  let testCombinationOfPassingAndFailingDescribes = combination =>
    test(
      "Pass: ["
      ++ boolListToString(combination)
      ++ "] should all be called and exit with code 1 if any are false",
      ({expect}) => {
        module InnerTestFramework =
          MakeInnterTestFramework({});
        let numCalls = ref(0);
        let i = ref(0);
        List.iter(
          pass => {
            incr(i);
            let describeName =
              "InnerTestFramework describe " ++ string_of_int(i^);
            InnerTestFramework.describe(describeName, ({test: innerTest}) =>
              innerTest(
                "inner test " ++ string_of_int(i^),
                ({expect}) => {
                  incr(numCalls);
                  expect.bool(true).toBe(pass);
                },
              )
            );
          },
          combination,
        );

        let onFrameworkFailureCalled = ref(false);

        InnerTestFramework.run(
          Rely.RunConfig.(
            initialize()
            |> printer_internal_do_not_use({
                 printEndline: _ => (),
                 printString: _ => (),
                 printNewline: _ => (),
                 flush: _ => (),
               })
            |> onTestFrameworkFailure(() => onFrameworkFailureCalled := true)
          ),
        );

        expect.int(numCalls^).toBe(List.length(combination));
        let shouldOnFrameworkFailureBeCalled =
          combination |> List.exists(entry => entry == false);
        expect.bool(onFrameworkFailureCalled^).toBe(
          shouldOnFrameworkFailureBeCalled,
        );
      },
    );

  testCombinationOfPassingAndFailingDescribes([true, false, true]);
  testCombinationOfPassingAndFailingDescribes([true, true, true]);
  testCombinationOfPassingAndFailingDescribes([true, false, false]);
  testCombinationOfPassingAndFailingDescribes([true, true]);
  testCombinationOfPassingAndFailingDescribes([true, false]);
  testCombinationOfPassingAndFailingDescribes([false, true]);
  testCombinationOfPassingAndFailingDescribes([false, false]);
});
