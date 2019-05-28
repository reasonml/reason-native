/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
open TestFramework;

describe("option matchers", ({describe}) => {
  describe("passing output", ({test}) => {
    test("expect.option.toBe None is None", ({expect}) =>
      expect.option(None).toBe(None)
    );
    test("expect.option.toBe Some(42) is Some(42)", ({expect}) =>
      expect.option(Some(42)).toBe(Some(42))
    );
    test("expect.option.not.toBe Some(42) is not None", ({expect}) =>
      expect.option(Some(42)).not.toBe(None)
    );
    test("expect.option.not.toBe None is not Some(42)", ({expect}) =>
      expect.option(None).not.toBe(Some(42))
    );
    test("expect.option.toBe custom equals", ({expect}) => {
      let equals = (f1, f2) => f1 -. f2 < 0.001;
      expect.option(Some(1.)).toBe(~equals, Some(1.000001));
    });
    test("expect.option.toBeNone", ({expect}) =>
      expect.option(None).toBeNone()
    );
    test("expect.option.not.toBeNone", ({expect}) =>
      expect.option(Some(42)).not.toBeNone()
    );
    test("expect.option.toBeSome Some(42) is Some", ({expect}) =>
      expect.option(Some(42)).toBeSome()
    );
    test("expect.option.not.toBeSome None is not Some", ({expect}) =>
      expect.option(None).not.toBeSome()
    );
  });

  describe("failing output", ({test}) => {
    test("expect.option.toBe None is Some", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils, ({expect}) =>
        expect.option(None).toBe(Some("failure :("))
      )
    );
    test("expect.option.toBe Some(42) is Some(41)", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils, ({expect}) =>
        expect.option(Some(42)).toBe(Some(41))
      )
    );
    test("expect.option.toBe custom equals", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let equals = (_, _) => false;
          expect.option(Some(42)).toBe(~equals, Some(42));
        },
      )
    );
    test("expect.option.toBeNone", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils, ({expect}) =>
        expect.option(Some(42)).toBeNone()
      )
    );
    test("expect.option.not.toBeNone", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils, ({expect}) =>
        expect.option(None).not.toBeNone()
      )
    );
    test("expect.option.toBeSome None is Some", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils, ({expect}) =>
        expect.option(None).toBeSome()
      )
    );
    test("expect.option.not.toBeSome Some(42) is not some", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils, ({expect}) =>
        expect.option(Some(42)).not.toBeSome()
      )
    );
  });
});
