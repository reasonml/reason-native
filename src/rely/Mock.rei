/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
let defaultNumMaxCalls: int;

module type MockConfig = {
  let maxNumCalls: int;
}

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

module Make: (StackTrace: StackTrace.StackTrace, MockConfig: MockConfig) => Mock;
