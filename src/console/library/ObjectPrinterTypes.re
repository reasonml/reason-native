/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
module ObjectPrinter = {
  type t = {
    int: (t, int) => string,
    string: (t, string) => string,
    quotedString: (t, string) => string,
    float: (t, float) => string,
    maxDepthExceeded: t => string,
    maxLengthExceeded: t => string,
    unknown: (t, Obj.t) => string,
    lazy_: (t, Obj.t) => string,
    doubleArray: (t, ~depth: int=?, Obj.t) => string,
    closure: (t, Obj.t) => string,
    block: (t, ~depth: int=?, Obj.t) => string,
    list: (t, ~depth: int=?, Obj.t) => string,
    polymorphicPrint: 'o. (t, ~depth: int=?, 'o) => string,
  };
};

module type ObjectPrinter = {
  type t = {
    int: (t, int) => string,
    string: (t, string) => string,
    quotedString: (t, string) => string,
    float: (t, float) => string,
    maxDepthExceeded: t => string,
    maxLengthExceeded: t => string,
    unknown: (t, Obj.t) => string,
    lazy_: (t, Obj.t) => string,
    doubleArray: (t, ~depth: int=?, Obj.t) => string,
    closure: (t, Obj.t) => string,
    block: (t, ~depth: int=?, Obj.t) => string,
    list: (t, ~depth: int=?, Obj.t) => string,
    polymorphicPrint: 'o. (t, ~depth: int=?, 'o) => string,
  };

  /**
   * Set the print width of objects printed in any console that uses
   * `ObjectPrinter`.
   */
  let setPrintWidth: int => unit;

  /**
   * Set the maximum depth the printer will print (default 20).
   */
  let setMaxDepth: int => unit;

  /**
   * Set the maximum length of structures to print. Prevents crashing
   * on circular lists. Must be greater than 2.
   */
  let setMaxLength: int => unit;

  /**
   * The base ObjectPrinter which can be customized.
   */
  let base: t;
};
