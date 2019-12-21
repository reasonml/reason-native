/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

type t = {
  modifier: Decorators.modifier,
  color: Decorators.color,
  bg: Decorators.color,
  length: string => int,
  unformattedText: string => string,
  partition: (int, string) => (string, string),
  create:
    (
      ~reset: bool=?,
      ~bold: bool=?,
      ~dim: bool=?,
      ~italic: bool=?,
      ~underline: bool=?,
      ~inverse: bool=?,
      ~hidden: bool=?,
      ~strikethrough: bool=?,
      ~color: ColorName.colorName=?,
      ~backgroundColor: ColorName.colorName=?,
      list(string),
    ) =>
    string,
  emptyStyle: StateMachine.state,
  parse: string => list((StateMachine.state, string)),
  apply: list((StateMachine.state, string)) => string
};
