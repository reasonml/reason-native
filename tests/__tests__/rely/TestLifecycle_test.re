/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open TestFramework;

type testRecord = {foo: int};
let uniqueInstance = {foo: 42};
let executeTestFramework = runFn =>
  runFn(
    Rely.RunConfig.(
      initialize() |> withReporters([]) |> onTestFrameworkFailure(() => ())
    ),
  );

describe("test lifecycle methods", ({test}) => {
  test(
    "env should be equal to ret of beforeAll if no beforeEach", ({expect}) => {
    module TestFrameworkInternal =
      TestFrameworkBuilder.Build({
        let action = a => a;
      });
    let beforeAllMock = Mock.mock1(() => uniqueInstance);
    let expectExternal = expect;
    let {describe} =
      TestFrameworkInternal.(
        describeConfig
        |> withLifecycle(l => l |> beforeAll(Mock.fn(beforeAllMock)))
        |> build
      );

    describe("beforeAll with no other methods", ({test, describe}) => {
      test("should bind env to ret of beforeAll", ({env}) =>
        expectExternal.equal(env, uniqueInstance)
      );
      describe("nested describe", ({test}) =>
        test("should bind env to ret of beforeAll", ({env, expect}) =>
          expectExternal.equal(env, uniqueInstance)
        )
      );
    });
    executeTestFramework(TestFrameworkInternal.run);
    expectExternal.assertions(2);
  });
  test("before all should be called exactly once", ({expect}) => {
    let beforeAllMock = Mock.mock1(() => uniqueInstance);
    module TestFrameworkInternal =
      TestFrameworkBuilder.Build({
        let action =
          TestFrameworkBuilder.(
            a =>
              a
              |> withTestSuite(
                   TestSuiteBuilder.(
                     init("my test suite")
                     |> withLifecycle(l =>
                          l |> beforeAll(Mock.fn(beforeAllMock))
                        )
                     |> withPassingTests(3)
                     |> withFailingTests(2)
                     |> withNestedTestSuite(~child=c =>
                          c |> withPassingTests(2)
                        )
                     |> build
                   ),
                 )
          );
      });
    executeTestFramework(TestFrameworkInternal.run);
    expect.mock(beforeAllMock).toBeCalledTimes(1);
  });
  test(
    "afterAll should be called once after all tests in the describe with the value returned by beforeAll",
    ({expect}) => {
      module TestFrameworkInternal =
        TestFrameworkBuilder.Build({
          let action = a => a;
        });
      let afterAllMock = Mock.mock1(_ => ());
      let expectExternal = expect;
      let {describe} =
        TestFrameworkInternal.(
          describeConfig
          |> withLifecycle(l =>
               l
               |> beforeAll(() => uniqueInstance)
               |> afterAll(Mock.fn(afterAllMock))
             )
          |> build
        );

      describe(
        "afterAll should not be called in this suite", ({test, describe}) => {
        test("after all should not be called", ({env}) =>
          expectExternal.mock(afterAllMock).not.toBeCalled()
        );
        describe("nested describe", ({test}) =>
          test(
            "after all should not be called in a nested test",
            ({env, expect}) =>
            expectExternal.mock(afterAllMock).not.toBeCalled()
          )
        );
      });
      executeTestFramework(TestFrameworkInternal.run);
      expectExternal.mock(afterAllMock).toBeCalledWith(uniqueInstance);
      expectExternal.mock(afterAllMock).toBeCalled();
      expectExternal.assertions(4);
    },
  );
  test(
    "before each should be called once before each test in the describe with the value returned by the call",
    ({expect}) => {
      let beforeEachMock = Mock.mock1(() => uniqueInstance);
      module TestFrameworkInternal =
        TestFrameworkBuilder.Build({
          let action = a => a;
        });
      let expectExternal = expect;
      let {describe} =
        TestFrameworkInternal.(
          describeConfig
          |> withLifecycle(l => l |> beforeEach(Mock.fn(beforeEachMock)))
          |> build
        );

      describe(
        "beforeEach should be called before each test in this suite",
        ({test, describe}) => {
        test(
          "before each should be called and equla to ret of beforeEach",
          ({env}) => {
          expectExternal.mock(beforeEachMock).toBeCalledTimes(1);
          expectExternal.equal(uniqueInstance, env);
          Mock.resetHistory(beforeEachMock);
        });
        describe("nested suite", ({test}) =>
          test(
            "before each should still be called and be equal to ret of beforeEach",
            ({env}) => {
            expectExternal.mock(beforeEachMock).toBeCalledTimes(1);
            expectExternal.equal(uniqueInstance, env);
            Mock.resetHistory(beforeEachMock);
          })
        );
      });
      executeTestFramework(TestFrameworkInternal.run);
      expect.assertions(4);
    },
  );
  test(
    "after each should be called once after each test in the describe",
    ({expect}) => {
    module TestFrameworkInternal =
      TestFrameworkBuilder.Build({
        let action = a => a;
      });
    let beforeEachMock = Mock.mock1(() => uniqueInstance);
    let afterEachMock = Mock.mock1(_ => Mock.resetHistory(beforeEachMock));
    let expectExternal = expect;
    let {describe} =
      TestFrameworkInternal.(
        describeConfig
        |> withLifecycle(l =>
             l
             |> beforeEach(Mock.fn(beforeEachMock))
             |> afterEach(Mock.fn(afterEachMock))
           )
        |> build
      );

    describe(
      "afterEach should reset beforeEachMock so it only claims to be called once",
      ({test, describe}) => {
        test(
          "before each should be called once as it should have been reset or not been called",
          ({env}) => {
            expectExternal.mock(beforeEachMock).toBeCalledTimes(1);
            ();
          },
        );
        test(
          "before each should be called once as it should have been reset or not been called",
          ({env}) => {
            expectExternal.mock(beforeEachMock).toBeCalledTimes(1);
            ();
          },
        );
        describe("nested suite", ({test}) => {
          test(
            "before each should be called once as it should have been reset or not been called",
            ({env}) => {
              expectExternal.mock(beforeEachMock).toBeCalledTimes(1);
              ();
            },
          );
          test(
            "before each should be called once as it should have been reset or not been called",
            ({env}) => {
              expectExternal.mock(beforeEachMock).toBeCalledTimes(1);
              ();
            },
          );
        });
      },
    );
    executeTestFramework(TestFrameworkInternal.run);
    expect.mock(afterEachMock).toBeCalledTimes(4);
    expect.assertions(5);
    ();
  });
  test("ret of beforeAll passed as arg to beforeEach", ({expect}) => {
    let beforeAllMock = Mock.mock1(() => uniqueInstance);
    module TestFrameworkInternal =
      TestFrameworkBuilder.Build({
        let action =
          TestFrameworkBuilder.(
            a =>
              a
              |> withTestSuite(
                   TestSuiteBuilder.(
                     init("my test suite")
                     |> withLifecycle(l =>
                          l
                          |> beforeAll(Mock.fn(beforeAllMock))
                          |> beforeEach(arg => {
                               expect.equal(arg, uniqueInstance);
                               ();
                             })
                        )
                     |> withPassingTests(3)
                     |> withFailingTests(2)
                     |> withNestedTestSuite(~child=c =>
                          c |> withPassingTests(2)
                        )
                     |> build
                   ),
                 )
          );
      });
    executeTestFramework(TestFrameworkInternal.run);
    /*ensure inner framework actually ran*/
    expect.mock(beforeAllMock).toBeCalledTimes(1);
  });
  test("ret of beforeEach passed as arg to afterEach", ({expect}) => {
    let beforeEachMock = Mock.mock1(() => uniqueInstance);
    module TestFrameworkInternal =
      TestFrameworkBuilder.Build({
        let action =
          TestFrameworkBuilder.(
            a =>
              a
              |> withTestSuite(
                   TestSuiteBuilder.(
                     init("my test suite")
                     |> withLifecycle(l =>
                          l
                          |> beforeEach(Mock.fn(beforeEachMock))
                          |> afterEach(arg => {
                               expect.equal(arg, uniqueInstance);
                               ();
                             })
                        )
                     |> withPassingTests(3)
                     |> withFailingTests(2)
                     |> withNestedTestSuite(~child=c =>
                          c |> withPassingTests(2)
                        )
                     |> build
                   ),
                 )
          );
      });
    executeTestFramework(TestFrameworkInternal.run);
    /*ensure inner framework actually ran*/
    expect.mock(beforeEachMock).toBeCalledTimes(7);
  });
  test(
    "exception in test, afterEach, afterAll should still be called",
    ({expect}) => {
    let afterEachMock = Mock.mock1(() => ());
    let afterAllMock = Mock.mock1(() => ());
    module TestFrameworkInternal =
      TestFrameworkBuilder.Build({
        let action = a => a;
      });
    let {describe} =
      TestFrameworkInternal.(
        describeConfig
        |> withLifecycle(l =>
             l
             |> afterEach(Mock.fn(afterEachMock))
             |> afterAll(Mock.fn(afterAllMock))
           )
        |> build
      );

    describe("afterEach and afterAll should be called", ({test}) =>
      test("error", _ =>
        raise(Not_found)
      )
    );
    executeTestFramework(TestFrameworkInternal.run);
    expect.mock(afterEachMock).toBeCalled();
    expect.mock(afterAllMock).toBeCalled();
    ();
  });
});
