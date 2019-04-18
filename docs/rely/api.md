---
id: api
title: Basic API
sidebar_label: Basic API
---

> Prefer reading code? Check out [RelyAPI.rei](https://github.com/facebookexperimental/reason-native/blob/master/src/rely/RelyAPI.rei)

## Basic Configuration

```reason
/* TestFramework.re */
include Rely.Make({
  let config =
    Rely.TestFrameworkConfig.initialize({
      snapshotDir: "path/to/test/lib/__snapshots__", /* required */
      projectDir: "path/to/your/project" /* required */
    });
});
```

**Running with default CLI config:**

```reason
/* MyLibTest.re */
TestFramework.cli(); /* default config */
```

## Testing

> For a full list of available matchers, visit the [Github Repository](https://github.com/facebookexperimental/reason-native/tree/master/src/rely/matchers)

```reason
/* MyFirstTest.re */
open TestFramework;

describe("my first test suite", ({test, testSkip, describe}) => {
  /* test suite */
  test("basic matchers", ({expect}) => {
    /* string type */
    expect.string("a").not.toEqual("b");

    /* file type */
    expect.file("hello.txt").toEqual("Hello world!");

    /* line type */
    expect.lines(["a", "b"]).toEqual("a\nb");

    /* bool type */
    expect.bool(false).toBe(false);

    /* int type */
    expect.int(1 + 1).toBe(2);

    /* float type */
    expect.float(0.1 +. 0.2).toBeCloseTo(2.0);

    /* fn type */
    expect.fn(() => {2;}).not.toThrow();

    /* list type */
    expect.list([]).toBeEmpty();
    expect.list([1,2,3]).not.toEqual([3,2,1]);

    /* array type */
    expect.array([|1, 2, 3|]).toContain(1);
    expect.array([|1,2|]).toEqual([|1, 2|]);

    /* polymorphic structural equality */
    expect.equal(1,1);

    /* polymorphic referential equality */
    expect.not.same("hello", "hello");
  });

  test("mock functions", ({expect}) => {
    let double = x => x * 2;
    /*there are constructors for up to 7 args */
    let mock = Mock.mock1(double);

    let list = List.map(Mock.fn(mock), [1, 2, 3, 4]);

    expect.mock(mock).toBeCalledTimes(4);
    expect.mock(mock).toBeCalledWith(2);
    expect.mock(mock).lastCalledWith(4);
    expect.mock(mock).toReturnTimes(4);
    expect.mock(mock).toReturn(6);
    expect.mock(mock).lastReturnedWith(8);
  });

  testSkip("incorrect test", ({expect}) => {
    expect.int(1 + 1).toBe(3);
  });

  describe("a nested test suite", ({test}) => {
    test("snapshots", ({expect}) => {
      expect.string("I 💖 Rely").toMatchSnapshot();
    });
  });
});
```

## Custom Matchers

```reason
/*UserMatchers.re*/
open Rely.MatcherTypes;

type user = {
  name: string,
  isLoggedIn: bool,
};

type userMatchers = {toBeLoggedIn: unit => unit};

type userMatchersWithNot = {
  toBeLoggedIn: unit => unit,
  not: userMatchers,
};

let makeUserMatchers = (accessorPath, user, {createMatcher}) => {
  let passMessageThunk = () => "";
  let createLoggedInMatcher = isNot =>
    createMatcher(
      ({matcherHint, formatReceived, formatExpected}, actualThunk, _) => {
      let actualUser = actualThunk();
      let pass = actualUser.isLoggedIn == !isNot;
      if (!pass) {
        let failureMessage =
          String.concat(
            "",
            [
              matcherHint(
                ~expectType=accessorPath,
                ~matcherName=".toBeLoggedIn",
                ~isNot,
                ~received="user",
                ~expected="",
                (),
              ),
              "\n\n",
              "Expected user ",
              actualUser.name,
              " to ",
              isNot ? "not " : "",
              "be logged in",
            ],
          );
        (() => failureMessage, false);
      } else {
        (passMessageThunk, true);
      };
    });
  {
    not: {
      toBeLoggedIn: () => createLoggedInMatcher(true, () => user, () => ()),
    },
    toBeLoggedIn: () => createLoggedInMatcher(false, () => user, () => ()),
  };
};

```

```reason
/* UserTest.re*/
open TestFramework;
open UserMatchers;

type customMatchers = {user: user => UserMatchers.userMatchersWithNot};

let customMatchers = extendUtils => {
  user: user => UserMatchers.makeUserMatchers(".ext.user", user, extendUtils),
};

let describe = extendDescribe(customMatchers);

describe("A test with users", ({test}) => {
  test("Logged in user should be logged in", ({expect}) => {
    let bob = {name: "Bob", isLoggedIn: true};

    expect.ext.user(bob).toBeLoggedIn();
  });
  test("Logged out user should not be logged in", ({expect}) => {
    let alice = {name: "Alice", isLoggedIn: false};

    expect.ext.user(alice).not.toBeLoggedIn();
  });
});
```
