/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open MatcherTypes;

/** TODO we can do call/return validation around usage of == like in
  * collection matchers and equals matcher. Not implemented because more
  * complicated for tuples (see ObjectPrinter.re for a rough guideline)
  */
type equalsFn('a) = ('a, 'a) => bool;

/*
 * toThrowException is explicitly not included on "not" as the expected behavior
 * is difficult to articulate. Jest will pass if it throws an exception other than the
 * one it is not supposed to throw as well as if no exception is thrown. I assert that
 * .not.toThrow is more intuitive and meets most use cases and as a result we should hold off
 * on adding .not for specific exceptions until compelling use cases are brought forward
 */
type matchers('tupledArgs, 'ret) = {
  toThrow: unit => unit,
  toBeCalled: unit => unit,
  toBeCalledTimes: int => unit,
  toBeCalledWith: (~equals: equalsFn('tupledArgs)=?, 'tupledArgs) => unit,
  lastCalledWith: (~equals: equalsFn('tupledArgs)=?, 'tupledArgs) => unit,
  toReturnTimes: int => unit,
  toReturnWith: (~equals: equalsFn('ret)=?, 'ret) => unit,
  lastReturnedWith: (~equals: equalsFn('ret)=?, 'ret) => unit,
};

type matchersWithNot('tupledArgs, 'ret) = {
  not: matchers('tupledArgs, 'ret),
  toThrow: unit => unit,
  toThrowException: exn => unit,
  toBeCalled: unit => unit,
  toBeCalledTimes: int => unit,
  toBeCalledWith: (~equals: equalsFn('tupledArgs)=?, 'tupledArgs) => unit,
  lastCalledWith: (~equals: equalsFn('tupledArgs)=?, 'tupledArgs) => unit,
  toReturnTimes: int => unit,
  toReturnWith: (~equals: equalsFn('ret)=?, 'ret) => unit,
  lastReturnedWith: (~equals: equalsFn('ret)=?, 'ret) => unit,
};

let passMessageThunk = () => "";
let defaultEqualityFn = (==);

module Make = (M: Mock.Sig) => {
  let split = (list, numToTake) => {
    let rec splitInternal = (list, numToTake, acc) =>
      switch (numToTake) {
      | 0 => (acc, list)
      | n =>
        switch (list) {
        | [] => (acc, [])
        | [h, ...t] => splitInternal(t, numToTake - 1, [h, ...acc])
        }
      };
    splitInternal(list, numToTake, []);
  };

  let formatExceptions = (exceptions, formatException) => {
    let maxNumExceptions = 3;
    let (toPrint, remainder) = split(exceptions, maxNumExceptions);
    let printedExceptions =
      toPrint
      |> List.rev
      |> List.map(ex => ex |> Printexc.to_string |> formatException)
      |> String.concat(", ");

    switch (remainder) {
    | [] => printedExceptions
    | l =>
      switch (List.length(l)) {
      | 1 => printedExceptions ++ formatException(" and one more exception.")
      | n =>
        printedExceptions
        ++ formatException(
             " and " ++ MatcherUtils.formatInt(n) ++ " more exceptions.",
           )
      }
    };
  };

  let formatCalls = (calls, formatCall) => {
    let maxNumCalls = 3;
    let (toPrint, remainder) = split(calls, maxNumCalls);
    let printedCalls =
      toPrint
      |> List.rev
      |> List.map(call => call |> PolymorphicPrint.print |> formatCall)
      |> String.concat(", ");
    switch (remainder) {
    | [] => printedCalls
    | l =>
      switch (List.length(l)) {
      | 1 => printedCalls ++ formatCall(" and one more call.")
      | n =>
        printedCalls
        ++ formatCall(" and " ++ MatcherUtils.formatInt(n) ++ " more calls.")
      }
    };
  };

  let formatReturns = (returns, formatReturn) => {
    let maxNumCalls = 3;
    let (toPrint, remainder) = split(returns, maxNumCalls);
    let printedRets =
      toPrint
      |> List.rev
      |> List.map(call => call |> PolymorphicPrint.print |> formatReturn)
      |> String.concat(", ");
    switch (remainder) {
    | [] => printedRets
    | l =>
      switch (List.length(l)) {
      | 1 => printedRets ++ formatReturn(" and one more value.")
      | n =>
        printedRets
        ++ formatReturn(
             " and " ++ MatcherUtils.formatInt(n) ++ " more values.",
           )
      }
    };
  };

  let numTimesString = num =>
    MatcherUtils.formatInt(num) ++ (num == 1 ? " time" : " times");
  let getReturns = mock =>
    M.getResults(mock)
    |> List.fold_left(
         (acc, r) =>
           switch (r) {
           | Mock.Return(v) => [v, ...acc]
           | Exception(_, _, _) => acc
           },
         [],
       );

  let makeMatchers = (accessorPath, {createMatcher}) => {
    let createMockMatchers = mock => {
      let toThrow =
        createMatcher(
          ({matcherHint, formatReceived, formatExpected}, actualThunk, _) => {
          let mock = actualThunk();
          let results = M.getResults(mock);
          let exceptions =
            results
            |> List.filter(r =>
                 switch (r) {
                 | Mock.Exception(_, _, _) => true
                 | Return(_) => false
                 }
               );

          switch (exceptions) {
          | [h, ...t] => (passMessageThunk, true)
          | [] =>
            let message =
              String.concat(
                "",
                [
                  matcherHint(
                    ~matcherName=".toThrow",
                    ~isNot=false,
                    ~expectType=accessorPath,
                    ~expected="",
                    ~received="mock",
                    (),
                  ),
                  "\n\n",
                  "Expected the function to throw an error.\n",
                  "But it didn't throw anything.",
                ],
              );
            ((() => message), false);
          };
        });

      let notToThrow =
        createMatcher(
          (
            {matcherHint, formatReceived, indent, formatExpected},
            actualThunk,
            _,
          ) => {
          let mock = actualThunk();
          let results = M.getResults(mock);
          let exceptions =
            results
            |> List.map(r =>
                 switch (r) {
                 | Mock.Exception(e, _, _) => Some(e)
                 | Return(_) => None
                 }
               )
            |> List.fold_left(
                 (acc, opt) =>
                   switch (opt) {
                   | Some(err) => [err, ...acc]
                   | None => acc
                   },
                 [],
               );

          switch (exceptions) {
          | [] => (passMessageThunk, true)
          | arr =>
            let message =
              String.concat(
                "",
                [
                  matcherHint(
                    ~matcherName="toThrow",
                    ~isNot=true,
                    ~expectType=accessorPath,
                    ~expected="",
                    ~received="mock",
                    (),
                  ),
                  "\n\n",
                  "Expected the function not to throw an error.\n",
                  "Instead, it threw: \n",
                  indent(formatExceptions(arr, formatReceived)),
                ],
              );
            ((() => message), false);
          };
        });

      let toThrowException =
        createMatcher(
          (
            {matcherHint, formatReceived, formatExpected, indent},
            actualThunk,
            expectedThunk,
          ) => {
          let mock = actualThunk();
          let expectedException = expectedThunk();
          let results = M.getResults(mock);
          let exceptions =
            results
            |> List.map(r =>
                 switch (r) {
                 | Mock.Exception(e, _, _) => Some(e)
                 | Return(_) => None
                 }
               )
            |> List.fold_left(
                 (acc, opt) =>
                   switch (opt) {
                   | Some(err) => [err, ...acc]
                   | None => acc
                   },
                 [],
               );
          let threwExpectedException =
            List.exists(e => e == expectedException, exceptions);

          if (threwExpectedException) {
            (passMessageThunk, true);
          } else {
            switch (exceptions) {
            | [] =>
              let message =
                String.concat(
                  "",
                  [
                    matcherHint(
                      ~matcherName=".toThrowException",
                      ~isNot=false,
                      ~expectType=accessorPath,
                      ~expected="exception",
                      ~received="function",
                      (),
                    ),
                    "\n\n",
                    "Expected the function to throw an error matching:\n",
                    indent(
                      formatExpected(Printexc.to_string(expectedException)),
                    ),
                    "\n",
                    "But it didn't throw anything.",
                  ],
                );
              ((() => message), false);
            | otherExceptions =>
              let message =
                String.concat(
                  "",
                  [
                    matcherHint(
                      ~matcherName=".toThrowException",
                      ~isNot=false,
                      ~expectType=accessorPath,
                      ~expected="exception",
                      ~received="function",
                      (),
                    ),
                    "\n\n",
                    "Expected the function to throw an error matching:\n",
                    indent(
                      formatExpected(Printexc.to_string(expectedException)),
                    ),
                    "\n",
                    "Instead, it threw:\n",
                    indent(
                      formatExceptions(otherExceptions, formatReceived),
                    ),
                  ],
                );
              ((() => message), false);
            };
          };
        });

      let toBeCalled = isNot =>
        createMatcher(
          (
            {matcherHint, formatReceived, formatExpected, indent},
            actualThunk,
            expectedThunk,
          ) => {
          let mock = actualThunk();
          let calls = M.getCalls(mock);
          let wasCalled =
            switch (calls) {
            | [] => false
            | [h, ...t] => true
            };

          switch (isNot, wasCalled) {
          | (true, false)
          | (false, true) => (passMessageThunk, true)
          | (true, true) =>
            let message =
              String.concat(
                "",
                [
                  matcherHint(
                    ~expectType=accessorPath,
                    ~isNot,
                    ~matcherName=".toBeCalled",
                    ~expected="",
                    ~received="mock",
                    (),
                  ),
                  "\n\n",
                  "Expected the mock not be called, but it was called with:\n",
                  indent(formatCalls(calls, formatReceived)),
                ],
              );
            ((() => message), false);
          | (false, false) =>
            let message =
              String.concat(
                "",
                [
                  matcherHint(
                    ~expectType=accessorPath,
                    ~isNot,
                    ~matcherName=".toBeCalled",
                    ~expected="",
                    ~received="mock",
                    (),
                  ),
                  "\n\n",
                  "Expected the mock to be called, but it was not called.",
                ],
              );
            ((() => message), false);
          };
        });
      let toBeCalledTimes = isNot =>
        createMatcher(
          (
            {matcherHint, formatReceived, formatExpected, indent},
            actualThunk,
            expectedThunk,
          ) => {
          let mock = actualThunk();
          let expectedNumCalls = expectedThunk();
          let calls = M.getCalls(mock);
          let actualNumCalls = List.length(calls);

          let wasCalledCorrectNumberOfTimes =
            expectedNumCalls == actualNumCalls;

          switch (isNot, wasCalledCorrectNumberOfTimes) {
          | (true, false)
          | (false, true) => (passMessageThunk, true)
          | (true, true) =>
            let message =
              String.concat(
                "",
                [
                  matcherHint(
                    ~expectType=accessorPath,
                    ~isNot,
                    ~matcherName=".toBeCalledTimes",
                    ~received="mock",
                    (),
                  ),
                  "\n\n",
                  "Expected the mock not be called ",
                  formatExpected(numTimesString(expectedNumCalls)),
                  ", but it was called exactly ",
                  formatReceived(numTimesString(actualNumCalls)),
                  ".",
                ],
              );
            ((() => message), false);
          | (false, false) =>
            switch (calls) {
            | [] =>
              let message =
                String.concat(
                  "",
                  [
                    matcherHint(
                      ~expectType=accessorPath,
                      ~isNot,
                      ~matcherName=".toBeCalledTimes",
                      ~received="mock",
                      (),
                    ),
                    "\n\n",
                    "Expected the mock to have been called ",
                    formatExpected(numTimesString(expectedNumCalls)),
                    ", but it was ",
                    formatReceived("not called"),
                    ".",
                  ],
                );
              ((() => message), false);
            | l =>
              let message =
                String.concat(
                  "",
                  [
                    matcherHint(
                      ~expectType=accessorPath,
                      ~isNot,
                      ~matcherName=".toBeCalledTimes",
                      ~received="mock",
                      (),
                    ),
                    "\n\n",
                    "Expected the mock to have been called ",
                    formatExpected(numTimesString(expectedNumCalls)),
                    ", but it was called ",
                    formatReceived(numTimesString(actualNumCalls)),
                    ".",
                  ],
                );
              ((() => message), false);
            }
          };
        });
      let toBeCalledWith = isNot =>
        createMatcher(
          (
            {matcherHint, formatReceived, formatExpected, indent},
            actualThunk,
            expectedThunk,
          ) => {
          let mock = actualThunk();
          let (equals, expectedArgs) = expectedThunk();

          let calls = M.getCalls(mock);
          let wasCalledWithExpected =
            List.exists(args => equals(args, expectedArgs), calls);
          let isUsingDefaultEquality = equals === defaultEqualityFn;

          switch (isNot, wasCalledWithExpected) {
          | (true, false)
          | (false, true) => (passMessageThunk, true)
          | (true, true) =>
            let message =
              String.concat(
                "",
                [
                  matcherHint(
                    ~expectType=accessorPath,
                    ~isNot,
                    ~matcherName=".toBeCalledWith",
                    ~received="mock",
                    ~options={
                      comment:
                        isUsingDefaultEquality ? Some("using ==") : None,
                    },
                    (),
                  ),
                  "\n\n",
                  "Expected the mock not to have been called with:\n",
                  indent(
                    formatExpected(PolymorphicPrint.print(expectedArgs)),
                  ),
                  "\nBut it was called with arguments equal to:\n",
                  indent(
                    formatReceived(PolymorphicPrint.print(expectedArgs)),
                  ),
                ],
              );
            ((() => message), false);
          | (false, false) =>
            switch (calls) {
            | [] =>
              let message =
                String.concat(
                  "",
                  [
                    matcherHint(
                      ~expectType=accessorPath,
                      ~isNot,
                      ~matcherName=".toBeCalledWith",
                      ~received="mock",
                      ~options={
                        comment:
                          isUsingDefaultEquality ? Some("using ==") : None,
                      },
                      (),
                    ),
                    "\n\n",
                    "Expected the mock to have been called with:\n",
                    indent(
                      formatExpected(PolymorphicPrint.print(expectedArgs)),
                    ),
                    "But it was ",
                    formatReceived("not called"),
                    ".",
                  ],
                );
              ((() => message), false);
            | calls =>
              let message =
                String.concat(
                  "",
                  [
                    matcherHint(
                      ~expectType=accessorPath,
                      ~isNot,
                      ~matcherName=".toBeCalledWith",
                      ~received="mock",
                      ~options={
                        comment:
                          isUsingDefaultEquality ? Some("using ==") : None,
                      },
                      (),
                    ),
                    "\n\n",
                    "Expected the mock to have been called with:\n",
                    indent(
                      formatExpected(PolymorphicPrint.print(expectedArgs)),
                    ),
                    "\nBut it was called with:\n",
                    indent(formatCalls(calls, formatReceived)),
                  ],
                );
              ((() => message), false);
            }
          };
        });
      let lastCalledWith = isNot =>
        createMatcher(
          (
            {matcherHint, formatReceived, formatExpected, indent},
            actualThunk,
            expectedThunk,
          ) => {
          let mock = actualThunk();
          let (equals, expectedArgs) = expectedThunk();
          let isUsingDefaultEquality = equals === defaultEqualityFn;

          let calls = M.getCalls(mock);
          let wasLastCalledWithExpected =
            switch (calls) {
            | [] => false
            | [h, ...t] => equals(h, expectedArgs)
            };

          switch (isNot, wasLastCalledWithExpected) {
          | (true, false)
          | (false, true) => (passMessageThunk, true)
          | (true, true) =>
            let message =
              String.concat(
                "",
                [
                  matcherHint(
                    ~expectType=accessorPath,
                    ~isNot,
                    ~matcherName=".lastCalledWith",
                    ~received="mock",
                    ~options={
                      comment:
                        isUsingDefaultEquality ? Some("using ==") : None,
                    },
                    (),
                  ),
                  "\n\n",
                  "Expected the mock not to have been last called with:\n",
                  indent(
                    formatExpected(PolymorphicPrint.print(expectedArgs)),
                  ),
                  "\nBut it was called with arguments equal to:\n",
                  indent(
                    formatReceived(PolymorphicPrint.print(expectedArgs)),
                  ),
                ],
              );
            ((() => message), false);
          | (false, false) =>
            switch (calls) {
            | [] =>
              let message =
                String.concat(
                  "",
                  [
                    matcherHint(
                      ~expectType=accessorPath,
                      ~isNot,
                      ~matcherName=".lastCalledWith",
                      ~received="mock",
                      ~options={
                        comment:
                          isUsingDefaultEquality ? Some("using ==") : None,
                      },
                      (),
                    ),
                    "\n\n",
                    "Expected the mock to have been last called with:\n",
                    indent(
                      formatExpected(PolymorphicPrint.print(expectedArgs)),
                    ),
                    "\nBut it was ",
                    formatReceived("not called"),
                    ".",
                  ],
                );
              ((() => message), false);
            | [lastCall, ...rest] =>
              let message =
                String.concat(
                  "",
                  [
                    matcherHint(
                      ~expectType=accessorPath,
                      ~isNot,
                      ~matcherName=".lastCalledWith",
                      ~received="mock",
                      ~options={
                        comment:
                          isUsingDefaultEquality ? Some("using ==") : None,
                      },
                      (),
                    ),
                    "\n\n",
                    "Expected the mock to have been last called with:\n",
                    indent(
                      formatExpected(PolymorphicPrint.print(expectedArgs)),
                    ),
                    "\nBut it was last called with:\n",
                    indent(formatCalls([lastCall], formatReceived)),
                  ],
                );
              ((() => message), false);
            }
          };
        });

      let toReturnTimes = isNot =>
        createMatcher(
          (
            {matcherHint, formatReceived, formatExpected, indent},
            actualThunk,
            expectedThunk,
          ) => {
          let mock = actualThunk();
          let expectedNumReturns = expectedThunk();
          let returns = getReturns(mock);
          let actualNumReturns = List.length(returns);
          let returnedCorrectNumberOfTimes =
            expectedNumReturns == actualNumReturns;

          switch (isNot, returnedCorrectNumberOfTimes) {
          | (true, false)
          | (false, true) => (passMessageThunk, true)
          | (true, true) =>
            let message =
              String.concat(
                "",
                [
                  matcherHint(
                    ~expectType=accessorPath,
                    ~isNot,
                    ~matcherName=".toReturnTimes",
                    ~received="mock",
                    (),
                  ),
                  "\n\n",
                  "Expected the mock not have returned ",
                  formatExpected(numTimesString(expectedNumReturns)),
                  ", but it returned exactly ",
                  formatReceived(numTimesString(actualNumReturns)),
                  ".",
                ],
              );
            ((() => message), false);
          | (false, false) =>
            switch (returns) {
            | [] =>
              let message =
                String.concat(
                  "",
                  [
                    matcherHint(
                      ~expectType=accessorPath,
                      ~isNot,
                      ~matcherName=".toReturnTimes",
                      ~received="mock",
                      (),
                    ),
                    "\n\n",
                    "Expected the mock to return ",
                    formatExpected(numTimesString(expectedNumReturns)),
                    ", but it didn't return.",
                  ],
                );
              ((() => message), false);
            | l =>
              let message =
                String.concat(
                  "",
                  [
                    matcherHint(
                      ~expectType=accessorPath,
                      ~isNot,
                      ~matcherName=".toReturnTimes",
                      ~received="mock",
                      (),
                    ),
                    "\n\n",
                    "Expected the mock to return ",
                    formatExpected(numTimesString(expectedNumReturns)),
                    ", but it returned ",
                    formatReceived(numTimesString(actualNumReturns)),
                    ".",
                  ],
                );
              ((() => message), false);
            }
          };
        });

      let toReturnWith = isNot =>
        createMatcher(
          (
            {matcherHint, formatReceived, formatExpected, indent},
            actualThunk,
            expectedThunk,
          ) => {
          let mock = actualThunk();
          let (equals, expectedReturn) = expectedThunk();

          let returns = getReturns(mock);

          let returnedExpected =
            List.exists(return => equals(return, expectedReturn), returns);
          let isUsingDefaultEquality = equals === defaultEqualityFn;

          switch (isNot, returnedExpected) {
          | (true, false)
          | (false, true) => (passMessageThunk, true)
          | (true, true) =>
            let message =
              String.concat(
                "",
                [
                  matcherHint(
                    ~expectType=accessorPath,
                    ~isNot,
                    ~matcherName=".toReturnWith",
                    ~received="mock",
                    ~options={
                      comment:
                        isUsingDefaultEquality ? Some("using ==") : None,
                    },
                    (),
                  ),
                  "\n\n",
                  "Expected the mock not to have returned:\n",
                  indent(
                    formatExpected(PolymorphicPrint.print(expectedReturn)),
                  ),
                  "\nBut it returned a value equal to:\n",
                  indent(
                    formatReceived(PolymorphicPrint.print(expectedReturn)),
                  ),
                ],
              );
            ((() => message), false);
          | (false, false) =>
            switch (returns) {
            | [] =>
              let message =
                String.concat(
                  "",
                  [
                    matcherHint(
                      ~expectType=accessorPath,
                      ~isNot,
                      ~matcherName=".toReturnWith",
                      ~received="mock",
                      ~options={
                        comment:
                          isUsingDefaultEquality ? Some("using ==") : None,
                      },
                      (),
                    ),
                    "\n\n",
                    "Expected the mock to have returned:\n",
                    indent(
                      formatExpected(PolymorphicPrint.print(expectedReturn)),
                    ),
                    "But it did ",
                    formatReceived("not return"),
                    ".",
                  ],
                );
              ((() => message), false);
            | calls =>
              let message =
                String.concat(
                  "",
                  [
                    matcherHint(
                      ~expectType=accessorPath,
                      ~isNot,
                      ~matcherName=".toReturnWith",
                      ~received="mock",
                      ~options={
                        comment:
                          isUsingDefaultEquality ? Some("using ==") : None,
                      },
                      (),
                    ),
                    "\n\n",
                    "Expected the mock to have returned:\n",
                    indent(
                      formatExpected(PolymorphicPrint.print(expectedReturn)),
                    ),
                    "\nBut it returned:\n",
                    indent(formatReturns(returns, formatReceived)),
                  ],
                );
              ((() => message), false);
            }
          };
        });
      let lastReturnedWith = isNot =>
        createMatcher(
          (
            {matcherHint, formatReceived, formatExpected, indent},
            actualThunk,
            expectedThunk,
          ) => {
          let mock = actualThunk();
          let (equals, expectedReturn) = expectedThunk();
          let isUsingDefaultEquality = equals === defaultEqualityFn;

          let results = M.getResults(mock);
          let lastResult =
            switch (results) {
            | [h, ...t] => Some(h)
            | [] => None
            };

          let lastReturnedExpected =
            switch (lastResult) {
            | Some(Exception(_, _, _))
            | None => false
            | Some(Mock.Return(value)) => equals(value, expectedReturn)
            };

          switch (isNot, lastReturnedExpected) {
          | (true, false)
          | (false, true) => (passMessageThunk, true)
          | (true, true) =>
            exception ShouldNeverGetHere;
            let actual =
              switch (lastResult) {
              | Some(Mock.Return(value)) => value
              | _ => raise(ShouldNeverGetHere)
              };
            let message =
              String.concat(
                "",
                [
                  matcherHint(
                    ~expectType=accessorPath,
                    ~isNot,
                    ~matcherName=".lastReturnedWith",
                    ~received="mock",
                    ~options={
                      comment:
                        isUsingDefaultEquality ? Some("using ==") : None,
                    },
                    (),
                  ),
                  "\n\n",
                  "Expected the mock to not have last returned:\n",
                  indent(
                    formatExpected(PolymorphicPrint.print(expectedReturn)),
                  ),
                  "\nBut it last returned:\n",
                  indent(formatReceived(PolymorphicPrint.print(actual))),
                ],
              );
            ((() => message), false);
          | (false, false) =>
            switch (lastResult) {
            | None =>
              let message =
                String.concat(
                  "",
                  [
                    matcherHint(
                      ~expectType=accessorPath,
                      ~isNot,
                      ~matcherName=".lastReturnedWith",
                      ~received="mock",
                      ~options={
                        comment:
                          isUsingDefaultEquality ? Some("using ==") : None,
                      },
                      (),
                    ),
                    "\n\n",
                    "Expected the mock to have last returned:\n",
                    indent(
                      formatExpected(PolymorphicPrint.print(expectedReturn)),
                    ),
                    "\nBut it did ",
                    formatReceived("not return"),
                    ".",
                  ],
                );
              ((() => message), false);
            | Some(Return(actualValue)) =>
              let message =
                String.concat(
                  "",
                  [
                    matcherHint(
                      ~expectType=accessorPath,
                      ~isNot,
                      ~matcherName=".lastReturnedWith",
                      ~received="mock",
                      ~options={
                        comment:
                          isUsingDefaultEquality ? Some("using ==") : None,
                      },
                      (),
                    ),
                    "\n\n",
                    "Expected the mock to have last returned:\n",
                    indent(
                      formatExpected(PolymorphicPrint.print(expectedReturn)),
                    ),
                    "\nBut the last call returned:\n",
                    indent(
                      formatReceived(PolymorphicPrint.print(actualValue)),
                    ),
                  ],
                );
              ((() => message), false);
            | Some(Exception(e, _, _)) =>
              let message =
                String.concat(
                  "",
                  [
                    matcherHint(
                      ~expectType=accessorPath,
                      ~isNot,
                      ~matcherName=".lastReturnedWith",
                      ~received="mock",
                      ~options={
                        comment:
                          isUsingDefaultEquality ? Some("using ==") : None,
                      },
                      (),
                    ),
                    "\n\n",
                    "Expected the mock to have last returned:\n",
                    indent(
                      formatExpected(PolymorphicPrint.print(expectedReturn)),
                    ),
                    "\nBut the last call raised:\n",
                    indent(formatReceived(Printexc.to_string(e))),
                  ],
                );
              ((() => message), false);
            }
          };
        });

      let matchers = {
        not: {
          toThrow: () => notToThrow(() => mock, () => ()),
          toBeCalled: () => toBeCalled(true, () => mock, () => ()),
          toBeCalledTimes: numTimes =>
            toBeCalledTimes(true, () => mock, () => numTimes),
          toBeCalledWith: (~equals=defaultEqualityFn, expectedArgs) =>
            toBeCalledWith(true, () => mock, () => (equals, expectedArgs)),
          lastCalledWith: (~equals=defaultEqualityFn, expectedArgs) =>
            lastCalledWith(true, () => mock, () => (equals, expectedArgs)),
          toReturnTimes: numTimes =>
            toReturnTimes(true, () => mock, () => numTimes),
          toReturnWith: (~equals=defaultEqualityFn, expectedReturn) =>
            toReturnWith(true, () => mock, () => (equals, expectedReturn)),
          lastReturnedWith: (~equals=defaultEqualityFn, expectedReturn) =>
            lastReturnedWith(
              true,
              () => mock,
              () => (equals, expectedReturn),
            ),
        },
        toThrow: () => toThrow(() => mock, () => ()),
        toThrowException: expectedException =>
          toThrowException(() => mock, () => expectedException),
        toBeCalled: () => toBeCalled(false, () => mock, () => ()),
        toBeCalledTimes: numTimes =>
          toBeCalledTimes(false, () => mock, () => numTimes),
        toBeCalledWith: (~equals=defaultEqualityFn, expectedArgs) =>
          toBeCalledWith(false, () => mock, () => (equals, expectedArgs)),
        lastCalledWith: (~equals=defaultEqualityFn, expectedArgs) =>
          lastCalledWith(false, () => mock, () => (equals, expectedArgs)),
        toReturnTimes: numTimes =>
          toReturnTimes(false, () => mock, () => numTimes),
        toReturnWith: (~equals=defaultEqualityFn, expectedReturn) =>
          toReturnWith(false, () => mock, () => (equals, expectedReturn)),
        lastReturnedWith: (~equals=defaultEqualityFn, expectedReturn) =>
          lastReturnedWith(
            false,
            () => mock,
            () => (equals, expectedReturn),
          ),
      };
      matchers;
    };
    createMockMatchers;
  };
};
