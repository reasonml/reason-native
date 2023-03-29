/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
module T = QCheck.Test;
module Rely = Rely;
open Rely.MatcherUtils;
open Rely.MatcherTypes;

let seed: ref(option(int)) = ref(None);

let seed_ =
  lazy {
    let s =
      try (int_of_string @@ Sys.getenv("QCHECK_SEED")) {
      | _ =>
        Random.self_init();
        Random.int(1000000000);
      };
    seed := Some(s);
    s;
  };

let default_rand = () =>
  /* random seed, for repeatability of tests */
  Random.State.make([|Lazy.force(seed_)|]);

let long_ =
  lazy (
    switch (Sys.getenv("QCHECK_LONG")) {
    | "1"
    | "true" => true
    | _ => false
    | exception Not_found => false
    }
  );

module Matchers = {
  type extension = {
    qCheckTest:
      'a.
      (~long: bool=?, ~rand: Random.State.t=?, QCheck.Test.cell('a)) => unit,
    qCheckCell:
      'a.
      (~long: bool=?, ~rand: Random.State.t=?, QCheck.Test.cell('a)) => unit,

  };

  let seedToString = (seed: ref(option(int)), formatSeed) =>
    switch (seed^) {
    | None => None
    | Some(s) =>
      Some("qcheck random seed: " ++ formatSeed(string_of_int(s)))
    };

  let pass = (() => "", true);

  let makeTestMatcher = (createMatcher, accessorPath) =>
    createMatcher(({formatReceived, indent, _}, actualThunk, _) => {
      let (cell, rand: Random.State.t, long: bool) =
        actualThunk();

      switch (QCheck.Test.check_cell_exn(~long, ~rand, cell)) {
      | () => pass
      | exception (QCheck.Test.Test_fail(name, input)) =>
        let testNameFailureMessage = "QCheck test \"" ++ name ++ "\" failed";
        let inputMessage =
          "generated input: \n"
          ++ indent(formatReceived(String.concat("\n", input)));
        let messageParts =
          switch (seedToString(seed, formatReceived)) {
          | None => [testNameFailureMessage, "", inputMessage]
          | Some(s) => [testNameFailureMessage, "", inputMessage, s]
          };

        let message = String.concat("\n", messageParts);
        ((() => message), false);
      | exception (QCheck.Test.Test_error(name, input, e, _)) =>
        let testNameFailureMessage =
          "QCheck test \""
          ++ name
          ++ "\" failed with exception "
          ++ Printexc.to_string(e);
        let inputMessage = "generated input: " ++ formatReceived(input);

        let messageParts =
          switch (seedToString(seed, formatReceived)) {
          | None => [testNameFailureMessage, "", inputMessage]
          | Some(s) => [testNameFailureMessage, "", inputMessage, s]
          };

        let message = String.concat("\n", messageParts);
        ((() => message), false);
      };
    });

  let makeCellMatcher = (createMatcher, accessorPath) =>
    createMatcher(({formatReceived, indent, _}, actualThunk, _) => {
      let (cell, rand: Random.State.t, long: bool) = actualThunk();

      switch (QCheck.Test.check_cell_exn(~long, ~rand, cell)) {
      | () => pass
      | exception (QCheck.Test.Test_fail(name, input)) =>
        let testNameFailureMessage = "QCheck test \"" ++ name ++ "\" failed";
        let inputMessage =
          "generated input: \n"
          ++ indent(formatReceived(String.concat("\n", input)));
        let messageParts =
          switch (seedToString(seed, formatReceived)) {
          | None => [testNameFailureMessage, "", inputMessage]
          | Some(s) => [testNameFailureMessage, "", inputMessage, s]
          };

        let message = String.concat("\n", messageParts);
        ((() => message), false);
      | exception (QCheck.Test.Test_error(name, input, e, _)) =>
        let testNameFailureMessage =
          "QCheck test \""
          ++ name
          ++ "\" failed with exception "
          ++ Printexc.to_string(e);
        let inputMessage = "generated input: " ++ formatReceived(input);

        let messageParts =
          switch (seedToString(seed, formatReceived)) {
          | None => [testNameFailureMessage, "", inputMessage]
          | Some(s) => [testNameFailureMessage, "", inputMessage, s]
          };

        let message = String.concat("\n", messageParts);
        ((() => message), false);
      };
    });

  let matchers = ({createMatcher}) => {
    qCheckTest: (~long=Lazy.force(long_), ~rand=default_rand(), t) =>
      makeTestMatcher(
        createMatcher,
        ".ext.qCheckTest",
        () => (t, rand, long),
        () => (),
      ),
    qCheckCell: (~long=Lazy.force(long_), ~rand=default_rand(), c) =>
      makeCellMatcher(
        createMatcher,
        ".ext.qCheckCell",
        () => (c, rand, long),
        () => (),
      ),
  };
};
