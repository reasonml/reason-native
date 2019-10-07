/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open TestFramework;

describe("Console.table", ({test}) => {
  test("Basic cell", ({expect}) =>
    expect.string(Console.table(~columns=[4], ~rows=[["Cell"]])).toEqual(
      "" ++ "--------\n" ++ "| Cell |\n" ++ "--------",
    )
  );
  test("Basic table", ({expect}) =>
    expect.string(
      Console.table(
        ~columns=[7, 4],
        ~rows=[["testing", "1234"], ["a", "b"]],
      ),
    ).
      toEqual(
      ""
      ++ "------------------\n"
      ++ "| testing | 1234 |\n"
      ++ "|---------+------|\n"
      ++ "| a       | b    |\n"
      ++ "------------------",
    )
  );

  test("mismatched row", ({expect}) => {
    expect.string(
      Console.table(~columns=[7, 3], ~rows=[["testing"], ["a", "b"]]),
    ).
      toEqual(
      ""
      ++ "-----------------\n"
      ++ "| testing |     |\n"
      ++ "|---------+-----|\n"
      ++ "| a       | b   |\n"
      ++ "-----------------",
    );
    expect.string(
      Console.table(~columns=[7, 4], ~rows=[["testing", "1234"], ["a"]]),
    ).
      toEqual(
      ""
      ++ "------------------\n"
      ++ "| testing | 1234 |\n"
      ++ "|---------+------|\n"
      ++ "| a       |      |\n"
      ++ "------------------",
    );
  });

  test("Text wrapping", ({expect}) =>
    expect.string(
      Console.table(
        ~columns=[7, 4],
        ~rows=[["test a long column", "this"], ["a", "b"]],
      ),
    ).
      toEqual(
      ""
      ++ "------------------\n"
      ++ "| test a  | this |\n"
      ++ "| long    |      |\n"
      ++ "| column  |      |\n"
      ++ "|---------+------|\n"
      ++ "| a       | b    |\n"
      ++ "------------------",
    )
  );
});
