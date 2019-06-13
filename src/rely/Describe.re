/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type describeUtils('ext, 'env) = {
  describe: describeFn('ext, 'env),
  describeSkip: describeFn('ext, 'env),
  describeOnly: describeFn('ext, 'env),
  test: Test.testFn('ext, 'env),
  testSkip: Test.testFn('ext, 'env),
  testOnly: Test.testFn('ext, 'env),
}
and describeFn('ext, 'env) =
  (string, describeUtils('ext, 'env) => unit) => unit;

type extensionResult('ext, 'env) = {
  describe: describeFn('ext, 'env),
  describeSkip: describeFn('ext, 'env),
  describeOnly: describeFn('ext, 'env),
};
