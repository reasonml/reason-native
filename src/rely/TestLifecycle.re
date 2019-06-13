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

type beforeAll('kind, 'ret);
type afterAll('kind, 'arg);
type beforeEach('kind, 'arg, 'ret);
type afterEach('kind, 'arg);

type t('beforeAll, 'afterAll, 'beforeEach, 'afterEach, 'all, 'each) = {
  beforeAll: unit => 'all,
  afterAll: 'all => unit,
  beforeEach: 'all => 'each,
  afterEach: 'each => unit,
};

type defaultLifecycle =
  t(
    beforeAll(beforeAllNotCalled, unit),
    afterAll(afterAllNotCalled, unit),
    beforeEach(beforeEachNotCalled, unit, unit),
    afterEach(afterEachNotCalled, unit),
    unit,
    unit,
  );

let default:
  t(
    beforeAll(beforeAllNotCalled, unit),
    afterAll(afterAllNotCalled, unit),
    beforeEach(beforeEachNotCalled, unit, unit),
    afterEach(afterEachNotCalled, unit),
    unit,
    unit,
  ) = {
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
        beforeAll(beforeAllNotCalled, oldAll),
        afterAll(afterAllNotCalled, oldAll),
        beforeEach(beforeEachNotCalled, oldAll, each),
        afterEach(afterEachNotCalled, each),
        oldAll,
        each,
      )
    ) =>
    t(
      beforeAll(beforeAllCalled, all),
      afterAll(afterAllNotCalled, all),
      beforeEach(beforeEachNotCalled, all, all),
      afterEach(afterEachNotCalled, all),
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
        beforeAll(a, all),
        afterAll(afterAllNotCalled, all),
        beforeEach(b, all, each),
        afterEach(c, each),
        all,
        each,
      )
    ) =>
    t(
      beforeAll(a, all),
      afterAll(afterAllCalled, all),
      beforeEach(b, all, each),
      afterEach(c, each),
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
        beforeAll(a, all),
        afterAll(b, all),
        beforeEach(beforeEachNotCalled, all, oldEach),
        afterEach(afterEachNotCalled, oldEach),
        all,
        oldEach,
      )
    ) =>
    t(
      beforeAll(a, all),
      afterAll(b, all),
      beforeEach(beforeEachCalled, all, each),
      afterEach(afterEachNotCalled, each),
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
        beforeAll(a, all),
        afterAll(b, all),
        beforeEach(c, all, each),
        afterEach(afterEachNotCalled, each),
        all,
        each,
      )
    ) =>
    t(
      beforeAll(a, all),
      afterAll(b, all),
      beforeEach(c, all, each),
      afterEach(afterEachCalled, each),
      all,
      each,
    ) =
  (afterEach, setup) => {
    beforeAll: setup.beforeAll,
    afterAll: setup.afterAll,
    beforeEach: setup.beforeEach,
    afterEach,
  };
