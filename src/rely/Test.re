/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type testUtils('ext, 'env) = {
  expect: DefaultMatchers.matchers('ext),
  env: 'env,
};
type testFn('ext, 'env) = (string, testUtils('ext, 'env) => unit) => unit;
