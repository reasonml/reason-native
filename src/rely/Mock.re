/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

type result('a) =
  | Return('a)
  | Exception(exn);

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
      results := results^ @ [Exception(e)];
      raise(e);
    };
  };
  {fn: wrappedFn, originalImplementation: fn, calls, results, implementation};
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
      results := results^ @ [Exception(e)];
      raise(e);
    };
  };
  {fn: wrappedFn, originalImplementation: fn, calls, results, implementation};
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
      results := results^ @ [Exception(e)];
      raise(e);
    };
  };
  {fn: wrappedFn, originalImplementation: fn, calls, results, implementation};
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
      results := results^ @ [Exception(e)];
      raise(e);
    };
  };
  {fn: wrappedFn, originalImplementation: fn, calls, results, implementation};
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
      results := results^ @ [Exception(e)];
      raise(e);
    };
  };
  {fn: wrappedFn, originalImplementation: fn, calls, results, implementation};
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
      results := results^ @ [Exception(e)];
      raise(e);
    };
  };
  {fn: wrappedFn, originalImplementation: fn, calls, results, implementation};
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
      results := results^ @ [Exception(e)];
      raise(e);
    };
  };
  {fn: wrappedFn, originalImplementation: fn, calls, results, implementation};
};
