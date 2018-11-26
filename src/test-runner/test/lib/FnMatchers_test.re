open TestFramework;
open TestRunnerOutputSnapshotTest;

describe("FnMatchers", describeUtils => {
  let test = describeUtils.test;
  test("should work for multiple return types", ({expect}) => {
    expect.fn(() => 7).not.toThrow();
    expect.fn(() => "this should be defined as well").not.toThrow();
    expect.fn(() => 42.42).not.toThrow();
  });
  test("should catch exceptions", ({expect}) =>
    expect.fn(() => raise(Not_found)).toThrow()
  );
  test("Specific exception", ({expect}) => {
    expect.fn(() => raise(Invalid_argument("that's oddly specific"))).toThrowException(Invalid_argument("that's oddly specific"));
  });
  testRunnerOutputSnapshotTest(
    "FnMatchers failure output",
    describeUtils,
    ({test}) => {
      test("Expect to throw, but doesn't", ({expect}) =>
        expect.fn(() => 7).toThrow()
      );
      test("Expect not to throw and does", ({expect}) =>
        expect.fn(() => raise(Invalid_argument("the earth is flat"))).not.
          toThrow()
      );
      test("Expect the wrong exception", ({expect}) => {
        expect.fn(() => raise(Invalid_argument("with some message"))).toThrowException(Invalid_argument("with some other message"));
      });
      test("Expect an exception that doesn't throw", ({expect}) => {
        expect.fn(() => ()).toThrowException(Not_found);
      });
    },
  );
});
