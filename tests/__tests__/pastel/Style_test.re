/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open TestFramework;
open Pastel;

describe("Pastel.style", ({test}) => {
  test("empty style should have no color or modifiers", ({expect}) => {
    expect.bool(isBold(emptyStyle)).toBeFalse();
    expect.bool(isDim(emptyStyle)).toBeFalse();
    expect.bool(isItalic(emptyStyle)).toBeFalse();
    expect.bool(isUnderline(emptyStyle)).toBeFalse();
    expect.bool(isHidden(emptyStyle)).toBeFalse();
    expect.bool(isStrikethrough(emptyStyle)).toBeFalse();
    expect.option(getColor(emptyStyle)).toBeNone();
    expect.option(getBackgroundColor(emptyStyle)).toBeNone();
  });

  let getterSetterTest = (set, get, name) => {
    test(
      "get set " ++ name,
      ({expect}) => {
        let trueResult = set(true, emptyStyle);
        expect.bool(get(trueResult)).toBeTrue();

        let falseResult = set(false, trueResult);
        expect.bool(get(falseResult)).toBeFalse();
      },
    );
  };

  getterSetterTest(setBold, isBold, "bold");
  getterSetterTest(setDim, isDim, "dim");
  getterSetterTest(setItalic, isItalic, "italic");
  getterSetterTest(setUnderline, isUnderline, "underline");
  getterSetterTest(setInverse, isInverse, "inverse");
  getterSetterTest(setHidden, isHidden, "hidden");
  getterSetterTest(setStrikethrough, isStrikethrough, "strikethrough");
});
