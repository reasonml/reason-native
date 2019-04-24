/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestFramework;
open QCheckRely;

let {describe, _} = extendDescribe(QCheckRely.Matchers.matchers);

let seed = Random.State.make([|42|]);

describe("qcheck-rely", ({test, _}) => {
  test("passing test", ({expect}) => {
    let passing =
      QCheck.Test.make(
        ~count=1000,
        ~name="list_rev_is_involutive",
        QCheck.(list(small_int)),
        l =>
        List.rev(List.rev(l)) == l
      );
    expect.ext.qCheckTest(~rand=seed, passing);
    ();
  });
  test("failing test", testUtils =>
    MatcherSnapshotTestRunner.snapshotFailingTestCaseCustomMatchers(
      QCheckRely.Matchers.matchers,
      testUtils,
      ({expect}) => {
        let failing =
          QCheck.Test.make(
            ~count=10, ~name="fail_sort_id", QCheck.(list(small_int)), l =>
            l == List.sort(compare, l)
          );

        expect.ext.qCheckTest(~rand=seed, failing);
      },
    )
  );

  test("error test", testUtils =>
    MatcherSnapshotTestRunner.snapshotFailingTestCaseCustomMatchers(
      QCheckRely.Matchers.matchers,
      testUtils,
      ({expect}) => {
        exception Error;

        let error =
          QCheck.Test.make(~count=10, ~name="error_raise_exn", QCheck.int, _ =>
            raise(Error)
          );

        expect.ext.qCheckTest(~rand=seed, error);
        ();
      },
    )
  );

  test("simple failure test", testUtils =>
    MatcherSnapshotTestRunner.snapshotFailingTestCaseCustomMatchers(
      QCheckRely.Matchers.matchers,
      testUtils,
      ({expect}) => {
        let simple_qcheck =
          QCheck.Test.make(
            ~name="fail_check_err_message", ~count=100, QCheck.small_int, _ =>
            QCheck.Test.fail_reportf("@[<v>this@ will@ always@ fail@]")
          );

        expect.ext.qCheckTest(~rand=seed, simple_qcheck);
        ();
      },
    )
  );

  /** Alternate syntax, worse matcher output */
  let {make, _} = QCheckRely.toRely(test);

  let simplePassingTest =
    QCheck.Test.make(
      ~count=1000, ~name="list_rev_is_involutive", QCheck.(list(small_int)), l =>
      List.rev(List.rev(l)) == l
    );
  make(~rand=seed, simplePassingTest);
});
