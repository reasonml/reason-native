/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;





/*
 * This is just a fake file her for the purposes of testing the file contents
 * parsing.  Without the file present, errors won't format correctly, and this
 * text test intends on testing formatting of build paths when interleaved with
 * other type based errors.
 */















module Describe = {
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
};
