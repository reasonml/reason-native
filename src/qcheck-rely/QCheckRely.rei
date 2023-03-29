/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
/**
  {1 Rely backend for QCheck}

    We can also use environment variables for controlling QCheck here

    - [QCHECK_SEED] if an integer, will fix the seed
    - [QCHECK_LONG] is present, will trigger long tests

    @since NEXT_RELEASE
*/;

/** Custom qcheck matchers for Rely, usage looks like:
 *
 * let { describe } = extendDescribe(QCheckRely.Matchers.matchers);
 * describe("my testSuite", ({test}) => {
 *   test("my qcheck test", ({expect}) => {
 *     expect.ext.qCheckTest(myQCheckTest);
 *     ();
 *   });
 *   ();
 * })
 *
 */
module Matchers: {
  type extension = {
    qCheckTest:
      'a.
      (~long: bool=?, ~rand: Random.State.t=?, QCheck.Test.cell('a)) => unit,
    qCheckCell:
      'a.
      (~long: bool=?, ~rand: Random.State.t=?, QCheck.Test.cell('a)) => unit,

  };

  let matchers: Rely.MatcherTypes.extendUtils => extension;
};
