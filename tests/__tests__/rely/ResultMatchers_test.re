open TestFramework;

describe("result matchers", ({describe}) => {
  describe("passing output", ({test}) => {
    test("expect.result.toBe Ok(42) is Ok(42)", ({expect}) =>
      expect.result(Ok(42)).toBe(Ok(42))
    );
    test("expect.result.toBe Error(42) is Error(42)", ({expect}) =>
      expect.result(Error(42)).toBe(Error(42))
    );
    test("expect.result.not.toBe Ok(42) is not Error(42)", ({expect}) =>
      expect.result(Ok(42)).not.toBe(Error(42))
    );
    test("expect.result.not.toBe Error(42) is not Ok(42)", ({expect}) =>
      expect.result(Error(42)).not.toBe(Ok(42))
    );
    test("expect.result.not.toBe Error(42) is not Ok('go')", ({expect}) =>
      expect.result(Error(42)).not.toBe(Ok("go"))
    );
    test("expect.result.toBe custom equals", ({expect}) => {
      let equalsOk = (f1, f2) => f1 -. f2 < 0.001;
      expect.result(Ok(1.)).toBe(~equalsOk, Ok(1.000001));
    });
    test("expect.result.toBe custom equals", ({expect}) => {
      let equalsError = (f1, f2) => f1 -. f2 < 0.001;
      expect.result(Error(1.)).toBe(~equalsError, Error(1.000001));
    });
    test("expect.result.toBeError", ({expect}) =>
      expect.result(Error(42)).toBeError()
    );
    test("expect.result.not.toBeError", ({expect}) =>
      expect.result(Ok(42)).not.toBeError()
    );
    test("expect.result.toBeOk Ok(42) is Ok", ({expect}) =>
      expect.result(Ok(42)).toBeOk()
    );
    test("expect.result.not.toBeOk Error is not Ok", ({expect}) =>
      expect.result(Error(42)).not.toBeOk()
    );
  });

  describe("failing output", ({test}) => {
    test("expect.result.toBe Error is Ok", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils, ({expect}) =>
        expect.result(Error("failure :(")).toBe(Ok("failure :("))
      )
    );
    test("expect.result.toBe Ok(42) is Ok(41)", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils, ({expect}) =>
        expect.result(Ok(42)).toBe(Ok(41))
      )
    );
    test("expect.result.toBe custom equals", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let equalsOk = (_, _) => false;
          expect.result(Ok(42)).toBe(~equalsOk, Ok(42));
        },
      )
    );
    test("expect.result.toBeError", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils, ({expect}) =>
        expect.result(Ok(42)).toBeError()
      )
    );
    test("expect.result.not.toBeError", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils, ({expect}) =>
        expect.result(Error(42)).not.toBeError()
      )
    );
    test("expect.result.toBeOk Error is Ok", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils, ({expect}) =>
        expect.result(Error(42)).toBeOk()
      )
    );
    test("expect.result.not.toBeOk Ok(42) is not Ok", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils, ({expect}) =>
        expect.result(Ok(42)).not.toBeOk()
      )
    );
  });
});
