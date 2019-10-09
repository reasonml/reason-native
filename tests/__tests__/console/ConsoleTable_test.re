/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open TestFramework;
open Console.ConsoleTable;

describe("Console.table", ({test}) => {
  test("Basic cell", ({expect}) => {
    let actual =
      Table.createElement(
        ~columns=[4],
        ~children=[Row.createElement(~children=["Cell"], ())],
        (),
      );
    let expected = "--------\n" ++ "| Cell |\n" ++ "--------";
    expect.string(actual).toEqual(expected);
  });

  test("Basic table", ({expect}) => {
    let actual =
      Table.createElement(
        ~columns=[7, 4],
        ~children=[
          Row.createElement(~children=["testing", "1234"], ()),
          Row.createElement(~children=["a", "b"], ()),
        ],
        (),
      );
    let expected =
      ""
      ++ "------------------\n"
      ++ "| testing | 1234 |\n"
      ++ "|---------+------|\n"
      ++ "| a       | b    |\n"
      ++ "------------------";
    expect.string(actual).toEqual(expected);
  });

  test("mismatched row", ({expect}) => {
    expect.string(
      Table.createElement(
        ~columns=[7, 3],
        ~children=[
          Row.createElement(~children=["testing"], ()),
          Row.createElement(~children=["a", "b"], ()),
        ],
        (),
      ),
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
      Table.createElement(
        ~columns=[7, 4],
        ~children=[
          Row.createElement(~children=["testing", "1234"], ()),
          Row.createElement(~children=["a"], ()),
        ],
        (),
      ),
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

  test("Text wrapping", ({expect}) => {
    let actual =
      Table.createElement(
        ~columns=[7, 4],
        ~children=[
          Row.createElement(~children=["test a long column", "this"], ()),
          Row.createElement(~children=["a", "b"], ()),
        ],
        (),
      );
    let expected =
      ""
      ++ "------------------\n"
      ++ "| test a  | this |\n"
      ++ "| long    |      |\n"
      ++ "| column  |      |\n"
      ++ "|---------+------|\n"
      ++ "| a       | b    |\n"
      ++ "------------------";
    expect.string(actual).toEqual(expected);
  });

  test("Basic cell JSX", ({expect}) => {
    let actual = <Table columns=[4]> <Row> "Cell" </Row> </Table>;
    let expected = "--------\n" ++ "| Cell |\n" ++ "--------";
    expect.string(actual).toEqual(expected);
  });

  test("Basic table JSX", ({expect}) => {
    let actual =
      <Table columns=[7, 4]>
        <Row> "testing" "1234" </Row>
        <Row> "a" "b" </Row>
      </Table>;
    let expected =
      ""
      ++ "------------------\n"
      ++ "| testing | 1234 |\n"
      ++ "|---------+------|\n"
      ++ "| a       | b    |\n"
      ++ "------------------";
    expect.string(actual).toEqual(expected);
  });

  test("Basic Table 2", ({expect}) => {
    let actual =
      <Table columns=[8, 16, 8]>
        <Row> "a1" "b1" "c1" </Row>
        <Row> "a2" "b2 is a cell that needs to wrap" "c2" </Row>
        <Row> "a3" "b3" "c3" "d3" </Row>
        <Row> "a4" "b4" </Row>
        <Row> "a5" "b5" "c5" </Row>
      </Table>;
    let expected =
      ""
      ++ "------------------------------------------\n"
      ++ "| a1       | b1               | c1       |\n"
      ++ "|----------+------------------+----------|\n"
      ++ "| a2       | b2 is a cell     | c2       |\n"
      ++ "|          | that needs to    |          |\n"
      ++ "|          | wrap             |          |\n"
      ++ "|----------+------------------+----------|\n"
      ++ "| a3       | b3               | c3       |\n"
      ++ "|----------+------------------+----------|\n"
      ++ "| a4       | b4               |          |\n"
      ++ "|----------+------------------+----------|\n"
      ++ "| a5       | b5               | c5       |\n"
      ++ "------------------------------------------";
    expect.string(actual).toEqual(expected);
  });
});
