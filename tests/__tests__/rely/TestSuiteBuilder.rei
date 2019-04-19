/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type t;
let init: string => t;
let withNestedTestSuite: (~child: t, t) => t;
let withFailingTests: (~only: bool=?, int, t) => t;
let withPassingTests: (~only: bool=?, int, t) => t;
let withSkippedTests: (int, t) => t;
let toFunction: (t, ~describe: Rely.Describe.describeFn(unit), ~describeSkip: Rely.Describe.describeFn(unit), ~describeOnly: Rely.Describe.describeFn(unit)) => unit;
let skipSuite: (t) => t;
let only: (t) => t;
