/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
let maxNumStackFrames = 3;
let defaultNumMaxCalls = 10000;

module type Mock = {
  exception ExceededMaxNumberOfAllowableCalls(string);
  type t('fn, 'ret, 'tupledArgs);
  type result('a) =
    | Return('a)
    | Exception(exn, option(Printexc.location), string);

  let fn: t('fn, 'ret, 'tupledArgs) => 'fn;
  let getCalls: t('fn, 'ret, 'tupledArgs) => list('tupledArgs);
  let getResults: t('fn, 'ret, 'tupledArgs) => list(result('ret));
  /** Resets the internal tracking information about calls and results */
  let resetHistory: t('fn, 'ret, 'tupledArgs) => unit;
  /** resets the mock implementation to the implementation originally provided */
  let resetImplementation: t('fn, 'ret, 'tupledArgs) => unit;
  /** resets both the internal tracking information about calls and results as
* as well as restores the mock implementation to the original implementation */
  let reset: t('fn, 'ret, 'tupledArgs) => unit;
  let changeImplementation: ('fn, t('fn, 'ret, 'tupledArgs)) => unit;

  let mock1: ('args => 'ret) => t('args => 'ret, 'ret, 'args);
  let mock2:
    (('arg1, 'arg2) => 'ret) =>
    t(('arg1, 'arg2) => 'ret, 'ret, ('arg1, 'arg2));
  let mock3:
    (('arg1, 'arg2, 'arg3) => 'ret) =>
    t(('arg1, 'arg2, 'arg3) => 'ret, 'ret, ('arg1, 'arg2, 'arg3));
  let mock4:
    (('arg1, 'arg2, 'arg3, 'arg4) => 'ret) =>
    t(
      ('arg1, 'arg2, 'arg3, 'arg4) => 'ret,
      'ret,
      ('arg1, 'arg2, 'arg3, 'arg4),
    );
  let mock5:
    (('arg1, 'arg2, 'arg3, 'arg4, 'arg5) => 'ret) =>
    t(
      ('arg1, 'arg2, 'arg3, 'arg4, 'arg5) => 'ret,
      'ret,
      ('arg1, 'arg2, 'arg3, 'arg4, 'arg5),
    );
  let mock6:
    (('arg1, 'arg2, 'arg3, 'arg4, 'arg5, 'arg6) => 'ret) =>
    t(
      ('arg1, 'arg2, 'arg3, 'arg4, 'arg5, 'arg6) => 'ret,
      'ret,
      ('arg1, 'arg2, 'arg3, 'arg4, 'arg5, 'arg6),
    );
  let mock7:
    (('arg1, 'arg2, 'arg3, 'arg4, 'arg5, 'arg6, 'arg7) => 'ret) =>
    t(
      ('arg1, 'arg2, 'arg3, 'arg4, 'arg5, 'arg6, 'arg7) => 'ret,
      'ret,
      ('arg1, 'arg2, 'arg3, 'arg4, 'arg5, 'arg6, 'arg7),
    );
};

module type MockConfig = {let maxNumCalls: int;};

module Make = (StackTrace: StackTrace.StackTrace, MockConfig: MockConfig) => {
  exception ExceededMaxNumberOfAllowableCalls(string);
  type result('a) =
    | Return('a)
    | Exception(exn, option(Printexc.location), string);

  type t('a, 'b, 'c) = {
    calls: ref(list('c)),
    results: ref(list(result('b))),
    fn: 'a,
    originalImplementation: 'a,
    implementation: ref('a),
    numCalls: ref(int),
  };

  let fn = mock => mock.fn;
  let getCalls = mock => mock.calls^;
  let getResults = mock => mock.results^;
  let wasCalled = mock =>
    switch (mock.calls^) {
    | [] => false
    | [first, ...rest] => true
    };
  let resetHistory = mock => {
    mock.calls := [];
    mock.results := [];
  };
  let resetImplementation = mock =>
    mock.implementation := mock.originalImplementation;
  let reset = mock => {
    resetHistory(mock);
    resetImplementation(mock);
  };
  let changeImplementation = (fn, mock) => mock.implementation := fn;

  let wrap = (calls, numCalls, results, implementation, tupledArgs, invoke) => {
    if (numCalls^ > MockConfig.maxNumCalls) {
      raise(
        ExceededMaxNumberOfAllowableCalls(
          "This mock has been called more than "
          ++ string_of_int(MockConfig.maxNumCalls)
          ++ " times. You can change the
        limit by passing an override to TestFrameworkConfig.withMaxNumberOfMockCalls",
        ),
      );
    };
    incr(numCalls);
    calls := [tupledArgs, ...calls^];
    switch (invoke(implementation^, tupledArgs)) {
    | returnValue =>
      results := [Return(returnValue), ...results^];
      returnValue;
    | exception e =>
      let exceptionTrace = StackTrace.getExceptionStackTrace();
      let location = StackTrace.getTopLocation(exceptionTrace);
      let stackTrace =
        StackTrace.stackTraceToString(exceptionTrace, maxNumStackFrames);
      results := [Exception(e, location, stackTrace), ...results^];
      raise(e);
    };
  };

  let makeMock = (fn, argProvider) => {
    let calls = ref([]);
    let results = ref([]);
    let implementation = ref(fn);
    let numCalls = ref(0);
    let curriedWrap = wrap(calls, numCalls, results, implementation);
    let wrappedFn = argProvider(curriedWrap);
    {
      fn: wrappedFn,
      originalImplementation: fn,
      calls,
      results,
      implementation,
      numCalls,
    };
  };

  let mock1 = fn =>
    makeMock(fn, (curriedWrap, arg) =>
      curriedWrap(arg, (f, arg) => f(arg))
    );

  let mock2 = fn =>
    makeMock(fn, (curriedWrap, arg1, arg2) =>
      curriedWrap((arg1, arg2), (f, (arg1, arg2)) => f(arg1, arg2))
    );

  let mock3 = fn =>
    makeMock(fn, (curriedWrap, arg1, arg2, arg3) =>
      curriedWrap((arg1, arg2, arg3), (f, (arg1, arg2, arg3)) =>
        f(arg1, arg2, arg3)
      )
    );

  let mock4 = fn =>
    makeMock(fn, (curriedWrap, arg1, arg2, arg3, arg4) =>
      curriedWrap((arg1, arg2, arg3, arg4), (f, (arg1, arg2, arg3, arg4)) =>
        f(arg1, arg2, arg3, arg4)
      )
    );

  let mock5 = fn =>
    makeMock(fn, (curriedWrap, arg1, arg2, arg3, arg4, arg5) =>
      curriedWrap(
        (arg1, arg2, arg3, arg4, arg5), (f, (arg1, arg2, arg3, arg4, arg5)) =>
        f(arg1, arg2, arg3, arg4, arg5)
      )
    );

  let mock6 = fn =>
    makeMock(fn, (curriedWrap, arg1, arg2, arg3, arg4, arg5, arg6) =>
      curriedWrap(
        (arg1, arg2, arg3, arg4, arg5, arg6),
        (f, (arg1, arg2, arg3, arg4, arg5, arg6)) =>
        f(arg1, arg2, arg3, arg4, arg5, arg6)
      )
    );

  let mock7 = fn =>
    makeMock(fn, (curriedWrap, arg1, arg2, arg3, arg4, arg5, arg6, arg7) =>
      curriedWrap(
        (arg1, arg2, arg3, arg4, arg5, arg6, arg7),
        (f, (arg1, arg2, arg3, arg4, arg5, arg6, arg7)) =>
        f(arg1, arg2, arg3, arg4, arg5, arg6, arg7)
      )
    );
};
