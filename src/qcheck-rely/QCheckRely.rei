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
      (~long: bool=?, ~rand: Random.State.t=?, QCheck.Test.t) => unit,
    qCheckCell:
      'a.
      (~long: bool=?, ~rand: Random.State.t=?, QCheck.Test.cell('a)) => unit,

  };

  let matchers: Rely.MatcherTypes.extendUtils => extension;
};

type qCheckRely = {
  make: (~long: bool=?, ~rand: Random.State.t=?, QCheck.Test.t) => unit,
  makeCell:
    'a.
    (~long: bool=?, ~rand: Random.State.t=?, QCheck.Test.cell('a)) => unit,

};

/** Convert a qcheck test into a rely test
 *  @param long whether to run long tests, (default: false)
 *  @param rand the random generator to use (default: [random_state ()])
 *  @since NEXT_RELEASE
 *
 * describe("my testSuite", ({test}) => {
 *   let { make, makeCell } = QCheckRely.toRely(test);
 *   make(myQCheckTest);
 *   ();
 * })
*/
let toRely: Rely.Test.testFn('a) => qCheckRely;
