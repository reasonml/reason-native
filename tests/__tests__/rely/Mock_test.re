/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
open TestFramework;

module type MockTestCase = {
  type fnType;
  type returnType;
  type tupledArgs;
  exception MockException;
  let create: fnType => Mock.t(fnType, returnType, tupledArgs);
  let call: (fnType, tupledArgs) => returnType;
  let implementation1: fnType;
  let implementation2: fnType;
  let exceptionImplemtation: fnType;
  let args: tupledArgs;
  let testCaseName: string;
};

module MakeTestSuite = (TestCase: MockTestCase) => {
  open TestCase;

  describe(
    testCaseName,
    ({test}) => {
      test("calls and results should initially be empty", ({expect}) => {
        let mock = create(implementation1);

        expect.list(mock |> Mock.getCalls).toBeEmpty();
        expect.list(mock |> Mock.getResults).toBeEmpty();
      });
      let runCallAndReturnValueTestCase = numCalls =>
        test(
          "calls and return values should be stored "
          ++ string_of_int(numCalls)
          ++ " calls",
          ({expect}) => {
            let mock = create(implementation1);
            let expectedCalls = ref([]);
            let expectedResults = ref([]);

            for (i in 1 to numCalls) {
              let result = Mock.Return(call(Mock.fn(mock), args));
              expectedCalls := expectedCalls^ @ [args];
              expectedResults := expectedResults^ @ [result];
            };

            expect.list(mock |> Mock.getCalls).toEqual(expectedCalls^);
            expect.list(mock |> Mock.getResults).toEqual(expectedResults^);
          },
        );
      let _ = [0, 1, 2, 3] |> List.map(runCallAndReturnValueTestCase);

      test(
        "exceptions in implementation should be recorded in results and rethrown",
        ({expect}) => {
        let mock = create(exceptionImplemtation);

        expect.fn(() => call(Mock.fn(mock), args)).toThrowException(
          MockException,
        );

        let exceptions =
          Mock.getResults(mock)
          |> List.map(r =>
               switch (r) {
               | Mock.Exception(e, _, _) => Some(e)
               | _ => None
               }
             );

        expect.list(exceptions).toEqual([Some(MockException)]);
        expect.list(Mock.getCalls(mock)).toEqual([args]);
      });

      test("reset history should reset calls and results", ({expect}) => {
        let mock = create(implementation1);

        let _ = call(Mock.fn(mock), args);
        mock |> Mock.resetHistory;

        expect.list(mock |> Mock.getCalls).toBeEmpty();
        expect.list(mock |> Mock.getResults).toBeEmpty();
      });

      test(
        "should return the same value as the provided implementation",
        ({expect}) => {
        let mock = create(implementation1);
        let implementationResult = call(implementation1, args);

        let mockResult = call(Mock.fn(mock), args);

        expect.bool(mockResult == implementationResult).toBeTrue();
      });

      test(
        "changeImplementation should cause Mock.fn to return same value as the new implementation",
        ({expect}) => {
          let mock = create(implementation1);
          let implementation1Result = call(implementation1, args);
          let implementation2Result = call(implementation2, args);

          mock |> Mock.changeImplementation(implementation2);
          let mockResult = call(Mock.fn(mock), args);

          /* additionally requiring implementation 1 and 2 to return different
           * things, otherwise this test is silly */
          expect.bool(implementation1Result == implementation2Result).
            toBeFalse();
          expect.bool(mockResult == implementation2Result).toBeTrue();
        },
      );

      test(
        "resetImplementation should cause Mock.fn to return same value as the original implementation",
        ({expect}) => {
          let mock = create(implementation1);
          let expectedResult = call(implementation1, args);

          mock |> Mock.changeImplementation(implementation2);
          mock |> Mock.changeImplementation(exceptionImplemtation);
          mock |> Mock.resetImplementation;

          let mockResult = call(Mock.fn(mock), args);

          expect.bool(mockResult == expectedResult).toBeTrue();
        },
      );

      test(
        "reset should reset calls, returns, the implementation", ({expect}) => {
        let mock = create(implementation1);

        mock |> Mock.changeImplementation(implementation2);
        let _ = call(Mock.fn(mock), args);
        Mock.reset(mock);

        expect.list(Mock.getCalls(mock)).toBeEmpty();
        expect.list(Mock.getResults(mock)).toBeEmpty();
        let result = call(Mock.fn(mock), args);
        let expectedResult = call(implementation1, args);
        expect.bool(result == expectedResult).toBeTrue();
      });

      /** This test is relatively expensive, it could be made more performant
        * by instantiating a new TestFramework and configuring the limit to be
        * much lower. However currently for Mock1-7 it is only adding a total of
        * about 70ms, so I am leaving it as is for now.
        */
      test("excessive calls should trigger an exception", ({expect}) => {
        let mock = create(implementation1);
        let fn = Mock.fn(mock);
        expect.fn(() => {
          for(i in 1 to 15000) {
            let _ = call(fn, args);
          }
        }).toThrow();
      })
    },
  );
};

module Mock1Tests =
  MakeTestSuite({
    type fnType = int => int;
    type returnType = int;
    type tupledArgs = int;
    exception MockException;
    let create = Mock.mock1;
    let call = (fn, arg) => fn(arg);
    let implementation1 = a => a;
    let implementation2 = a => 1 + a;
    let exceptionImplemtation = a => raise(MockException);
    let args = 0;
    let testCaseName = "Rely.mock1";
  });

module Mock2Tests =
  MakeTestSuite({
    type fnType = (int, int) => int;
    type returnType = int;
    type tupledArgs = (int, int);
    exception MockException;
    let create = Mock.mock2;
    let call = (fn, (arg1, arg2)) => fn(arg1, arg2);
    let implementation1 = (a, b) => a;
    let implementation2 = (a, b) => b;
    let exceptionImplemtation = (a, b) => raise(MockException);
    let args = (1, 0);
    let testCaseName = "Rely.mock2";
  });

module Mock3Tests =
  MakeTestSuite({
    type fnType = (int, int, int) => int;
    type returnType = int;
    type tupledArgs = (int, int, int);
    exception MockException;
    let create = Mock.mock3;
    let call = (fn, (arg1, arg2, arg3)) => fn(arg1, arg2, arg3);
    let implementation1 = (a, b, c) => a;
    let implementation2 = (a, b, c) => b;
    let exceptionImplemtation = (a, b, c) => raise(MockException);
    let args = (1, 0, (-1));
    let testCaseName = "Rely.mock3";
  });

module Mock4Tests =
  MakeTestSuite({
    type fnType = (int, int, int, int) => int;
    type returnType = int;
    type tupledArgs = (int, int, int, int);
    exception MockException;
    let create = Mock.mock4;
    let call = (fn, (arg1, arg2, arg3, arg4)) => fn(arg1, arg2, arg3, arg4);
    let implementation1 = (a, b, c, d) => a;
    let implementation2 = (a, b, c, d) => b;
    let exceptionImplemtation = (a, b, c, d) => raise(MockException);
    let args = (1, 0, (-1), 42);
    let testCaseName = "Rely.mock4";
  });

module Mock5Tests =
  MakeTestSuite({
    type fnType = (int, int, int, int, int) => int;
    type returnType = int;
    type tupledArgs = (int, int, int, int, int);
    exception MockException;
    let create = Mock.mock5;
    let call = (fn, (arg1, arg2, arg3, arg4, arg5)) =>
      fn(arg1, arg2, arg3, arg4, arg5);
    let implementation1 = (a, b, c, d, e) => a;
    let implementation2 = (a, b, c, d, e) => b;
    let exceptionImplemtation = (a, b, c, d, e) => raise(MockException);
    let args = (1, 0, (-1), 42, (-42));
    let testCaseName = "Rely.mock5";
  });

module Mock6Tests =
  MakeTestSuite({
    type fnType = (int, int, int, int, int, int) => int;
    type returnType = int;
    type tupledArgs = (int, int, int, int, int, int);
    exception MockException;
    let create = Mock.mock6;
    let call = (fn, (arg1, arg2, arg3, arg4, arg5, arg6)) =>
      fn(arg1, arg2, arg3, arg4, arg5, arg6);
    let implementation1 = (a, b, c, d, e, f) => a;
    let implementation2 = (a, b, c, d, e, f) => b;
    let exceptionImplemtation = (a, b, c, d, e, f) => raise(MockException);
    let args = (1, 0, (-1), 42, (-42), 6);
    let testCaseName = "Rely.mock6";
  });

module Mock7Tests =
  MakeTestSuite({
    type fnType = (int, int, int, int, int, int, int) => int;
    type returnType = int;
    type tupledArgs = (int, int, int, int, int, int, int);
    exception MockException;
    let create = Mock.mock7;
    let call = (fn, (arg1, arg2, arg3, arg4, arg5, arg6, arg7)) =>
      fn(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    let implementation1 = (a, b, c, d, e, f, g) => a;
    let implementation2 = (a, b, c, d, e, f, g) => b;
    let exceptionImplemtation = (a, b, c, d, e, f, g) =>
      raise(MockException);
    let args = (1, 0, (-1), 42, (-42), 6, (-6));
    let testCaseName = "Rely.mock7";
  });
