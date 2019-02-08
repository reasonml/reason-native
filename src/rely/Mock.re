/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
let maxNumStackFrames = 3;

module type Mock = {
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

module Make = (StackTrace: StackTrace.StackTrace) => {
  type result('a) =
    | Return('a)
    | Exception(exn, option(Printexc.location), string);

  type t('a, 'b, 'c) = {
    calls: ref(list('c)),
    results: ref(list(result('b))),
    fn: 'a,
    originalImplementation: 'a,
    implementation: ref('a),
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

  let mock1 = fn => {
    let calls = ref([]);
    let results = ref([]);
    let implementation = ref(fn);
    let wrappedFn = arg => {
      calls := calls^ @ [arg];
      switch (implementation^(arg)) {
      | returnValue =>
        results := results^ @ [Return(returnValue)];
        returnValue;
      | exception e =>
        let exceptionTrace = StackTrace.getExceptionStackTrace();
        let location = StackTrace.getTopLocation(exceptionTrace);
        let stackTrace =
          StackTrace.stackTraceToString(exceptionTrace, maxNumStackFrames);
        results := results^ @ [Exception(e, location, stackTrace)];
        raise(e);
      };
    };
    {
      fn: wrappedFn,
      originalImplementation: fn,
      calls,
      results,
      implementation,
    };
  };

  let mock2 = fn => {
    let calls = ref([]);
    let results = ref([]);
    let implementation = ref(fn);
    let wrappedFn = (arg1, arg2) => {
      calls := calls^ @ [(arg1, arg2)];
      switch (implementation^(arg1, arg2)) {
      | returnValue =>
        results := results^ @ [Return(returnValue)];
        returnValue;
      | exception e =>
        let exceptionTrace = StackTrace.getExceptionStackTrace();
        let location = StackTrace.getTopLocation(exceptionTrace);
        let stackTrace =
          StackTrace.stackTraceToString(exceptionTrace, maxNumStackFrames);
        results := results^ @ [Exception(e, location, stackTrace)];
        raise(e);
      };
    };
    {
      fn: wrappedFn,
      originalImplementation: fn,
      calls,
      results,
      implementation,
    };
  };

  let mock3 = fn => {
    let calls = ref([]);
    let results = ref([]);
    let implementation = ref(fn);
    let wrappedFn = (arg1, arg2, arg3) => {
      calls := calls^ @ [(arg1, arg2, arg3)];
      switch (implementation^(arg1, arg2, arg3)) {
      | returnValue =>
        results := results^ @ [Return(returnValue)];
        returnValue;
      | exception e =>
        let exceptionTrace = StackTrace.getExceptionStackTrace();
        let location = StackTrace.getTopLocation(exceptionTrace);
        let stackTrace =
          StackTrace.stackTraceToString(exceptionTrace, maxNumStackFrames);
        results := results^ @ [Exception(e, location, stackTrace)];
        raise(e);
      };
    };
    {
      fn: wrappedFn,
      originalImplementation: fn,
      calls,
      results,
      implementation,
    };
  };
  let mock4 = fn => {
    let calls = ref([]);
    let results = ref([]);
    let implementation = ref(fn);
    let wrappedFn = (arg1, arg2, arg3, arg4) => {
      calls := calls^ @ [(arg1, arg2, arg3, arg4)];
      switch (implementation^(arg1, arg2, arg3, arg4)) {
      | returnValue =>
        results := results^ @ [Return(returnValue)];
        returnValue;
      | exception e =>
        let exceptionTrace = StackTrace.getExceptionStackTrace();
        let location = StackTrace.getTopLocation(exceptionTrace);
        let stackTrace =
          StackTrace.stackTraceToString(exceptionTrace, maxNumStackFrames);
        results := results^ @ [Exception(e, location, stackTrace)];
        raise(e);
      };
    };
    {
      fn: wrappedFn,
      originalImplementation: fn,
      calls,
      results,
      implementation,
    };
  };
  let mock5 = fn => {
    let calls = ref([]);
    let results = ref([]);
    let implementation = ref(fn);
    let wrappedFn = (arg1, arg2, arg3, arg4, arg5) => {
      calls := calls^ @ [(arg1, arg2, arg3, arg4, arg5)];
      switch (implementation^(arg1, arg2, arg3, arg4, arg5)) {
      | returnValue =>
        results := results^ @ [Return(returnValue)];
        returnValue;
      | exception e =>
        let exceptionTrace = StackTrace.getExceptionStackTrace();
        let location = StackTrace.getTopLocation(exceptionTrace);
        let stackTrace =
          StackTrace.stackTraceToString(exceptionTrace, maxNumStackFrames);
        results := results^ @ [Exception(e, location, stackTrace)];
        raise(e);
      };
    };
    {
      fn: wrappedFn,
      originalImplementation: fn,
      calls,
      results,
      implementation,
    };
  };

  let mock6 = fn => {
    let calls = ref([]);
    let results = ref([]);
    let implementation = ref(fn);
    let wrappedFn = (arg1, arg2, arg3, arg4, arg5, arg6) => {
      calls := calls^ @ [(arg1, arg2, arg3, arg4, arg5, arg6)];
      switch (implementation^(arg1, arg2, arg3, arg4, arg5, arg6)) {
      | returnValue =>
        results := results^ @ [Return(returnValue)];
        returnValue;
      | exception e =>
        let exceptionTrace = StackTrace.getExceptionStackTrace();
        let location = StackTrace.getTopLocation(exceptionTrace);
        let stackTrace =
          StackTrace.stackTraceToString(exceptionTrace, maxNumStackFrames);
        results := results^ @ [Exception(e, location, stackTrace)];
        raise(e);
      };
    };
    {
      fn: wrappedFn,
      originalImplementation: fn,
      calls,
      results,
      implementation,
    };
  };
  let mock7 = fn => {
    let calls = ref([]);
    let results = ref([]);
    let implementation = ref(fn);
    let wrappedFn = (arg1, arg2, arg3, arg4, arg5, arg6, arg7) => {
      calls := calls^ @ [(arg1, arg2, arg3, arg4, arg5, arg6, arg7)];
      switch (implementation^(arg1, arg2, arg3, arg4, arg5, arg6, arg7)) {
      | returnValue =>
        results := results^ @ [Return(returnValue)];
        returnValue;
      | exception e =>
        let exceptionTrace = StackTrace.getExceptionStackTrace();
        let location = StackTrace.getTopLocation(exceptionTrace);
        let stackTrace =
          StackTrace.stackTraceToString(exceptionTrace, maxNumStackFrames);
        results := results^ @ [Exception(e, location, stackTrace)];
        raise(e);
      };
    };
    {
      fn: wrappedFn,
      originalImplementation: fn,
      calls,
      results,
      implementation,
    };
  };
};
