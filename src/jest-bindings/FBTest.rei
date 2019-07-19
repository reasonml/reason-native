/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

/**
Types that are used throughout these test utilities. They should be opened in
order to allow access to the necessary record fields when using `describe`, and
`test`.
*/
module Types: {
  type matcherUtils = {tmp: unit};
  type matcherConfig('a, 'b) =
    (matcherUtils, unit => 'a, unit => 'b) => (unit => string, bool);
  type matcherResult('a, 'b) = (unit => 'a, unit => 'b) => unit;
  type createMatcher('a, 'b) =
    matcherConfig('a, 'b) => matcherResult('a, 'b);
  type extendUtils = {createMatcher: 'a 'b. createMatcher('a, 'b)};
  type stringUtils = {
    toBeEmpty: unit => unit,
    toEqual: string => unit,
  };
  type stringUtilsWithNot = {
    toBeEmpty: unit => unit,
    toEqual: string => unit,
    toMatchSnapshot: unit => unit,
    not: stringUtils,
  };
  type boolUtils = {
    toBe: bool => unit,
    toBeTrue: unit => unit,
    toBeFalse: unit => unit,
  };
  type boolUtilsWithNot = {
    toBe: bool => unit,
    toBeTrue: unit => unit,
    toBeFalse: unit => unit,
    not: boolUtils,
  };
  type intUtils = {toBe: int => unit};
  type intUtilsWithNot = {
    toBe: int => unit,
    not: intUtils,
  };
  type floatUtils = {toBeCloseTo: (~digits: int=?, float) => unit};
  type floatUtilsWithNot = {
    toBeCloseTo: (~digits: int=?, float) => unit,
    not: floatUtils,
  };
  type collectionKind('a) =
    | String: collectionKind(string)
    | Bool: collectionKind(bool)
    | Int: collectionKind(int)
    /* TODO: Accept precision here for floats. */
    | Float: collectionKind(float)
    | Custom(('a, 'a) => bool): collectionKind('a);
  type listOfUtils('a) = {toEqual: list('a) => unit};
  type listOfUtilsWithNot('a) = {
    toEqual: list('a) => unit,
    not: listOfUtils('a),
  };
  type arrayOfUtils('a) = {toEqual: array('a) => unit};
  type arrayOfUtilsWithNot('a) = {
    toEqual: array('a) => unit,
    not: arrayOfUtils('a),
  };
  type expectUtils('ext) = {
    string: string => stringUtilsWithNot,
    bool: bool => boolUtilsWithNot,
    int: int => intUtilsWithNot,
    float: float => floatUtilsWithNot,
    listOf: 'a. (collectionKind('a), list('a)) => listOfUtilsWithNot('a),
    arrayOf: 'a. (collectionKind('a), array('a)) => arrayOfUtilsWithNot('a),
    ext: 'ext,
  };
  type testUtils('ext) = {
    expect: expectUtils('ext),
    expectTrue: bool => unit,
    expectFalse: bool => unit,
    expectEqual: 'a. ('a, 'a) => unit,
    expectNotEqual: 'a. ('a, 'a) => unit,
    expectSame: 'a. ('a, 'a) => unit,
    expectNotSame: 'a. ('a, 'a) => unit,
  };
  /**
`test` is used to create a single unit test, all of the functionality being
tested in a single `test` should be related. An `expect` function is exposed
in order to make assertions.

The underlying JS implementation uses Jest's `test` funciton.

```
test("add", ({expect}) => {
  expect.int(MyMath.add(3, 4)).toBe(7);
});
```
  */
  type test('ext) = (string, testUtils('ext) => unit) => unit;
  type describeUtils('ext) = {
    beforeEach: (unit => unit) => unit,
    beforeAll: (unit => unit) => unit,
    test: test('ext),
    testOnly: test('ext),
    testSkip: test('ext),
  };
  type describeFn('ext) = (string, describeUtils('ext) => unit) => unit;

  type extensionResult('ext) = {
    describe: describeFn('ext),
  };
};

open Types;

let extendDescribe:
  (extendUtils => 'ext) => extensionResult('ext);

/**
`describe` is the entry point for creating tests. It exposes a `test` function
and other utilties that can create tests.

The underlying JS implementation uses Jest's `describe` funciton.

```
describe("MyMath", ({test}) => {
  test("add", ({expect}) => {
    expect.int(MyMath.add(3, 4)).toBe(7);
  });
});
```
*/
let describe: (string, describeUtils(unit) => unit) => unit;

/**
Resets all modules. This is exactly the same as calling `jest.resetModules()`
in JS.
*/
let resetModules: unit => unit;
