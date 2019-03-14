open TestFramework;
open MatcherSnapshotTestRunner;

/* String matchers are also integration tested in depth in TestRunner_test.re*/
describe("String matchers", ({test}) =>
  test(
    "expect.string.toMatch matches shouldn't have to be at the beginning ",
    ({expect}) => {
    let myString = "TEST2, TEST, TEST3";

    expect.string(myString).toMatch("TEST");
    expect.string(myString).toMatch("TEST2");
    expect.string(myString).toMatch("TEST3");
    ();
  })
);
