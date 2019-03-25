/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type testUtils('ext) = {expect: DefaultMatchers.matchers('ext)};
type testFn('ext) = (string, testUtils('ext) => unit) => unit;
