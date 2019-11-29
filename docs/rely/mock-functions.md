---
id: mock-functions
title: Mock Functions
sidebar_label: Mock Functions
---

Mock functions (or "spies") in Rely can be used to track information about the arguments and return values of functions, and allow for test-time configuration of return values. Rely has built-in matchers for dealing with mock functions that are documented extensively [here](expect.md#expectmock).

> Unlike Jest in JavaScript, module level mocking is not supported. The functionality available in Rely is analagous to what Jest offers via [`jest.fn` ](https://jestjs.io/docs/en/jest-object#jestfnimplementation) and there is no direct [`jest.mock`](https://jestjs.io/docs/en/jest-object#jestmockmodulename-factory-options) analogy. Many situations in which `jest.mock` would be useful can be addressed through dependency management and the structuring of side effects within your application. A simple example using functors is shown [below](#injecting-via-functor).

## Using Mock Functions

### Creating Mocks

There are constructors for creating mocks that take up to 7 arguments. A default implementation must be specified.

```re
open TestFramework;

let calculateInterest = Mock.mock3((p, r, t) => p *. exp(r *. t));
```

```re
module Mock: {
  type t('fn, 'returnType, 'tupledArgs);

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
```

### Calling Mock Functions

The `Mock.mockN` constructor returns a mutable opaque type for tracking call information. To access the underlying function, use `Mock.fn`.

```re
open TestFramework;

describe("Mock documentation", ({test}) => {
  test("calling a mock function", ({expect}) => {
    let mockDouble = Mock.mock1(x => x * 2);
    let mockFunction = Mock.fn(mockDouble);

    expect.int(mockFunction(2)).toBe(4);
  })
});
```

### Injecting via Functor

When testing code with side effects such as talking to a database or making network requests, it is often desirable to be able to mock out these side effects. A straightforward way of achieving this is by passing functions that perform side effects as arguments. However, this is not always desirable due to the problem of threading such arguments through deeply nested code. The use of module functions (functors) can accomplish a similar thing and somewhat alleviate this issue.

For example, suppose that we have some code that uses a logger. In production we want the logger to make HTTP requests to some endpoint, but we deem this behavior undesirable for our test.

Without abstracting away the dependendency on our HTTP logger, our code might look like this.

#### MyModule.re

```reml
module MyApp = {
  let doSomethingThatGetsLogged = () => {
    HTTPLogger.log("starting to do someting");
    SomeModule.doSomething();
    HTTPLogger.log("did the thing!");
  };
};
```

Instead we could inject the code via a functor (module function) or via an ordinary function.

#### MyModule.re

```re {
module Make = (Dependencies: {let log: string => unit;}) => {
  let doSomethingThatGetsLogged = () => {
    Dependencies.log("starting to do something");
    SomeModule.doSomething();
    Dependencies.log("did the thing!");
  };
};
/*
 * Making the "real" versions of the module could easily be handled elsewhere
 *  in the application but is shown here for simplicity
 */
include Make({
  /*
   * Generally it would be better practice to have some Logger module type and
   * specify that there is a logger module in the functor signature, but for the
   * simplicity of this example we use a function instead
   */
  let log = HTTPLogger.log;
});
```

In your tests you can do this

#### MyModuleTest.re

```re {
open TestFramework;

let mockLog = Mock.mock1(_ => ());
module MyModule =
  RelyTalk.MyModule.Make({
    let log = Mock.fn(mockLog);
  });

let {describe} =
  describeConfig
  |> withLifecycle(testLifecycle =>
       testLifecycle |> beforeEach(() => Mock.reset(mockLog))
     )
  |> build;

describe("MyModule", ({test}) => {
  test("Should log a start message", ({expect}) => {
    MyModule.doSomethingThatGetsLogged();
    expect.mock(mockLog).toBeCalledWith("starting to do something");
  });
  test("should log a completion message", ({expect}) => {
    MyModule.doSomethingThatGetsLogged();
    expect.mock(mockLog).toBeCalledWith("did the thing!");
  });
});
```

## API reference

### Mock.changeImplementation(implementation, mock)

Accepts an implementation to be used for calls to the underlying mock function. The original implementation passed to the Mock constructor can be restored via [`Mock.reset`](#mockresetmock) or [`Mock.resetImplementation`](#mockresetimplementationmock).

```re
open TestFramework;

describe("Mock.changeImplementation", ({test}) => {
  test("changes function implementation", ({expect}) => {
    let mock = Mock.mock1(() => 42);
    let fn = Mock.fn(mock);
    expect.int(fn()).toBe(42);

    Mock.changeImplementation(() => 43, mock);

    expect.int(fn()).toBe(43);
  });
});
```

### Mock.reset(mock)

Resets all information relating to stored calls and results, as well as restoring the mock to the original implementation it was constructed with.

Equivalent to calling both [`Mock.resetHistory`](#mockresethistorymock) and [`Mock.resetImplementation`](#mockresetimplementationmock)

### Mock.resetHistory(mock)

Resets all information relating to stored calls and results.

Often this is useful when you want to clean up a mock's usage data between two assertions.

### Mock.resetImplementation(mock)

Restores the original mock implementation to what it was first constructed with, regardless of how many times [`Mock.changeImplementation`](#mockchangeimplementationimplementation-mock) has been called.

### Mock.getCalls(mock)

Returns a list of the arguments that have been passed to the underlying function. The calls are ordered from most to least recent. Generally the [built in mock matchers](expect.md#expectmock) should be used instead of `Mock.getCalls`.

### Mock.getResults(mock)

Returns a list of `Mock.result` from the underlying function. The calls are ordered from most to least recent. Generally the [built in mock matchers](expect.md#expectmock) should be used instead of `Mock.getResults`.

```re
module Mock = {
  type result('a) =
    | Return('a)
    | Exception(exn, option(Printexc.location), string);
};
```
