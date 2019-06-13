/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

type beforeAllCalled;
type beforeAllNotCalled;
type afterAllCalled;
type afterAllNotCalled;
type beforeEachCalled;
type beforeEachNotCalled;
type afterEachCalled;
type afterEachNotCalled;

type t('beforeAll, 'afterAll, 'beforeEach, 'afterEach, 'all, 'each) = {
  beforeAll: unit => 'all,
  afterAll: 'all => unit,
  beforeEach: 'all => 'each,
  afterEach: 'each => unit,
};

type defaultLifecycle =
  t(
    beforeAllNotCalled,
    afterAllNotCalled,
    beforeEachNotCalled,
    afterEachNotCalled,
    unit,
    unit,
  );

let default: defaultLifecycle = {
  beforeAll: () => (),
  afterAll: () => (),
  beforeEach: () => (),
  afterEach: () => (),
};

let beforeAll:
  type all oldAll each.
    (
      unit => all,
      t(
        beforeAllNotCalled,
        afterAllNotCalled,
        beforeEachNotCalled,
        afterEachNotCalled,
        oldAll,
        each,
      )
    ) =>
    t(
      beforeAllCalled,
      afterAllNotCalled,
      beforeEachNotCalled,
      afterEachNotCalled,
      all,
      all,
    ) =
  (beforeAll, setup) => {
    beforeAll,
    afterAll: _ => (),
    beforeEach: all => all,
    afterEach: _ => (),
  };
let afterAll:
  type a b c all each.
    (
      all => unit,
      t(
        a,
        afterAllNotCalled,
        b,
        c,
        all,
        each,
      )
    ) =>
    t(
      a,
      afterAllCalled,
      b,
      c,
      all,
      each,
    ) =
  (afterAll, setup) => {
    beforeAll: setup.beforeAll,
    afterAll,
    beforeEach: setup.beforeEach,
    afterEach: setup.afterEach,
  };
let beforeEach:
  type a b all each oldEach.
    (
      all => each,
      t(
        a,
        b,
        beforeEachNotCalled,
        afterEachNotCalled,
        all,
        oldEach,
      )
    ) =>
    t(
      a,
      b,
      beforeEachCalled,
      afterEachNotCalled,
      all,
      each,
    ) =
  (beforeEach, setup) => {
    beforeAll: setup.beforeAll,
    afterAll: setup.afterAll,
    beforeEach,
    afterEach: _ => (),
  };

let afterEach:
  type a b c all each.
    (
      each => unit,
      t(
        a,
        b,
        c,
        afterEachNotCalled,
        all,
        each,
      )
    ) =>
    t(
      a,
      b,
      c,
      afterEachCalled,
      all,
      each,
    ) =
  (afterEach, setup) => {
    beforeAll: setup.beforeAll,
    afterAll: setup.afterAll,
    beforeEach: setup.beforeEach,
    afterEach,
  };
