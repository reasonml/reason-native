/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestFramework;
open TestRunnerOutputSnapshotTest;

describe("FloatMatchers", describeUtils => {
  let test = describeUtils.test;
  let defaultPrecisionTestCases = [
    (0.0, 0.0),
    (0.0, 0.001),
    (1.23, 1.229),
    (1.23, 1.226),
    (1.23, 1.225),
    (1.23, 1.234),
  ];
  defaultPrecisionTestCases
  |> List.iter(((f1, f2)) =>
       test(
         string_of_float(f1) ++ " should be close to " ++ string_of_float(f2),
         ({expect}) =>
         expect.float(f1).toBeCloseTo(f2)
       )
     );

    let customPrecisionTestCases = [
        (0.0, 0.0, 0),
        (1.0, 0.96, 1),
        (1.0, 0.996, 2),
        (1.0, 0.9996, 3),
        (1.0, 0.9995, 3),
        (0.0, 0.1, 0),
        (0.0, 0.0001, 3),
        (0.0, 0.000004, 5)
    ];

    customPrecisionTestCases
    |> List.iter(((f1, f2, precision)) => test(
        string_of_float(f1) ++ " should be close to " ++ string_of_float(f2) ++ " with precision " ++ string_of_int(precision),
        ({expect}) =>
            expect.float(f1).toBeCloseTo(~digits=precision, f2)
        )
    );

    let notToBeCloseToCases = [
        (0.0, 1.0, 0),
        (1.0, 0.95, 1),
    ];

    notToBeCloseToCases
    |> List.iter(((f1, f2, precision)) => test(
        string_of_float(f1) ++ " should not be close to " ++ string_of_float(f2) ++ " with precision " ++ string_of_int(precision),
        ({expect}) =>
            expect.float(f1).not.toBeCloseTo(~digits=precision, f2)
        )
    );

    testRunnerOutputSnapshotTest(
        "failure output .not.toBeCloseTo",
        describeUtils,
        ({test}) => {
            customPrecisionTestCases
            |> List.iter(((f1, f2, precision)) => test(
                string_of_float(f1) ++ " should not be close to " ++ string_of_float(f2) ++ " with precision " ++ string_of_int(precision),
                ({expect}) =>
                    expect.float(f1).not.toBeCloseTo(~digits=precision, f2)
                )
            );
        }
    );

    testRunnerOutputSnapshotTest(
        "failure output .toBeCloseTo",
        describeUtils,
        ({test}) => {
            notToBeCloseToCases
            |> List.iter(((f1, f2, precision)) => test(
                string_of_float(f1) ++ " should be close to " ++ string_of_float(f2) ++ " with precision " ++ string_of_int(precision),
                ({expect}) =>
                    expect.float(f1).not.toBeCloseTo(~digits=precision, f2)
                )
            );
        }
    );
});
