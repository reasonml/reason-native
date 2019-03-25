/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type describeUtils('ext) = {
  describe: describeFn('ext),
  describeSkip: describeFn('ext),
  test: Test.testFn('ext),
  testSkip: Test.testFn('ext),
}
and describeFn('ext) = (string, describeUtils('ext) => unit) => unit;

type extensionResult('ext) = {
  describe: describeFn('ext),
  describeSkip: describeFn('ext),
};
