/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestFramework;

describe("expect.assertions", ({test}) => {
  test("works on happy path", ({expect}) => {
    expect.assertions(2);
    expect.bool(true).toBeTrue();
    expect.bool(false).not.toBeTrue();
  });
  test("redeclare assertion count", ({expect}) => {
    expect.assertions(3);
    expect.bool(true).toBeTrue();
    expect.bool(false).not.toBeTrue();
    expect.assertions(2);
  });
  test("expect no assertions", ({expect}) =>
    expect.assertions(0)
  );
  test("mismatched assertion count plural expect, singular call", testUtils =>
    MatcherSnapshotTestRunner.snapshotFailingTestCase(
      testUtils,
      ({expect}) => {
        expect.assertions(2);
        expect.bool(true).toBeTrue();
      },
    )
  );
  test("mismatched assertion count plural expect and call", testUtils =>
    MatcherSnapshotTestRunner.snapshotFailingTestCase(
      testUtils,
      ({expect}) => {
        expect.assertions(3);
        expect.bool(true).toBeTrue();
        expect.bool(true).toBeTrue();
      },
    )
  );
  test("mismatched assertion count singular expect, zero calls", testUtils =>
    MatcherSnapshotTestRunner.snapshotFailingTestCase(testUtils, ({expect}) =>
      expect.assertions(1)
    )
  );
});

describe("expect.hasAssertions", ({test}) => {
  test("works on happy path", ({expect}) => {
    expect.hasAssertions();
    expect.bool(true).toBeTrue();
  });
  test("no assertions", testUtils =>
    MatcherSnapshotTestRunner.snapshotFailingTestCase(testUtils, ({expect}) =>
      expect.hasAssertions()
    )
  );
});
