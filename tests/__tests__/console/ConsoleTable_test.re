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
        ~columns=[{width: 4}],
        ~children=[Row.createElement(~children=["Cell"], ())],
        (),
      );
    let expected = "--------\n" ++ "| Cell |\n" ++ "--------";
    expect.string(actual).toEqual(expected);
  });

  test("Basic table", ({expect}) => {
    let actual =
      Table.createElement(
        ~columns=[{width: 7}, {width: 4}],
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
        ~columns=[{width: 7}, {width: 3}],
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
        ~columns=[{width: 7}, {width: 4}],
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
        ~columns=[{width: 7}, {width: 4}],
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

  test("Basic table JSX", ({expect}) => {
    let actual =
      <Table columns=[<ColumnConfig width=7 />, <ColumnConfig width=4 />]>
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

  test("Large table", ({expect}) => {
    let actual =
      <Table
        columns=[
          <ColumnConfig width=8 />,
          <ColumnConfig width=16 />,
          <ColumnConfig width=8 />,
        ]>
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

  test("Formatted Table", ({expect}) => {
    let bold_cell = Pastel.(<Pastel bold=true> "b1" </Pastel>);
    let wrapped_blue_cell =
      Pastel.(<Pastel color=Blue> "b2 is a cell that needs to wrap" </Pastel>);
    let actual =
      <Table
        columns=[
          <ColumnConfig width=8 />,
          <ColumnConfig width=16 />,
          <ColumnConfig width=8 />,
        ]>
        <Row> "a1" bold_cell "c1" </Row>
        <Row> "a2" wrapped_blue_cell "c2" </Row>
        <Row> "a3" "b3" "c3" "d3" </Row>
        <Row> "a4" "b4" </Row>
        <Row> "a5" "b5" "c5" </Row>
      </Table>;
    /*
       Can't use Terminal mode in the printer but HumanReadable mode in the
       tests so this result is copied from the string output. It was visually
       tested to produce the correct results.
     */
    let expected =
      ""
      ++ "------------------------------------------\n"
      ++ "| a1       | \027[1mb1              \027[22m | c1       |\n"
      ++ "|----------+------------------+----------|\n"
      ++ "| a2       | \027[34mb2 is a cell    \027[39m | c2       |\n"
      ++ "|          | \027[34mthat needs to   \027[39m |          |\n"
      ++ "|          | \027[34mwrap            \027[39m |          |\n"
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

describe("Table styles", ({test}) => {
  test("SimpleLines", ({expect}) => {
    let actual =
      <Table
        borderStyle=SimpleLines
        columns=[
          <ColumnConfig width=8 />,
          <ColumnConfig width=16 />,
          <ColumnConfig width=8 />,
        ]>
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

  test("BoxLight", ({expect}) => {
    let actual =
      <Table
        borderStyle=BoxLight
        columns=[
          <ColumnConfig width=8 />,
          <ColumnConfig width=16 />,
          <ColumnConfig width=8 />,
        ]>
        <Row> "a1" "b1" "c1" </Row>
        <Row> "a2" "b2 is a cell that needs to wrap" "c2" </Row>
        <Row> "a3" "b3" "c3" "d3" </Row>
        <Row> "a4" "b4" </Row>
        <Row> "a5" "b5" "c5" </Row>
      </Table>;
    let expected =
      ""
      ++ "┌──────────┬──────────────────┬──────────┐\n"
      ++ "│ a1       │ b1               │ c1       │\n"
      ++ "├──────────┼──────────────────┼──────────┤\n"
      ++ "│ a2       │ b2 is a cell     │ c2       │\n"
      ++ "│          │ that needs to    │          │\n"
      ++ "│          │ wrap             │          │\n"
      ++ "├──────────┼──────────────────┼──────────┤\n"
      ++ "│ a3       │ b3               │ c3       │\n"
      ++ "├──────────┼──────────────────┼──────────┤\n"
      ++ "│ a4       │ b4               │          │\n"
      ++ "├──────────┼──────────────────┼──────────┤\n"
      ++ "│ a5       │ b5               │ c5       │\n"
      ++ "└──────────┴──────────────────┴──────────┘";
    expect.string(actual).toEqual(expected);
  });

  test("None", ({expect}) => {
    let actual =
      <Table
        border=None
        columns=[
          <ColumnConfig width=8 />,
          <ColumnConfig width=16 />,
          <ColumnConfig width=8 />,
        ]>
        <Row> "a1" "b1" "c1" </Row>
        <Row> "a2" "b2 is a cell that needs to wrap" "c2" </Row>
        <Row> "a3" "b3" "c3" "d3" </Row>
        <Row> "a4" "b4" </Row>
        <Row> "a5" "b5" "c5" </Row>
      </Table>;
    let expected =
      ""
      ++ "                                          \n"
      ++ "  a1         b1                 c1        \n"
      ++ "                                          \n"
      ++ "  a2         b2 is a cell       c2        \n"
      ++ "             that needs to                \n"
      ++ "             wrap                         \n"
      ++ "                                          \n"
      ++ "  a3         b3                 c3        \n"
      ++ "                                          \n"
      ++ "  a4         b4                           \n"
      ++ "                                          \n"
      ++ "  a5         b5                 c5        \n"
      ++ "                                          ";
    expect.string(actual).toEqual(expected);
  });

  test("BoxLight with outer borders", ({expect}) => {
    let actual =
      <Table
        border=Outer
        borderStyle=BoxLight
        columns=[
          <ColumnConfig width=8 />,
          <ColumnConfig width=16 />,
          <ColumnConfig width=8 />,
        ]>
        <Row> "a1" "b1" "c1" </Row>
        <Row> "a2" "b2 is a cell that needs to wrap" "c2" </Row>
        <Row> "a3" "b3" "c3" "d3" </Row>
        <Row> "a4" "b4" </Row>
        <Row> "a5" "b5" "c5" </Row>
      </Table>;
    let expected =
      ""
      ++ "┌────────────────────────────────────────┐\n"
      ++ "│ a1         b1                 c1       │\n"
      ++ "│                                        │\n"
      ++ "│ a2         b2 is a cell       c2       │\n"
      ++ "│            that needs to               │\n"
      ++ "│            wrap                        │\n"
      ++ "│                                        │\n"
      ++ "│ a3         b3                 c3       │\n"
      ++ "│                                        │\n"
      ++ "│ a4         b4                          │\n"
      ++ "│                                        │\n"
      ++ "│ a5         b5                 c5       │\n"
      ++ "└────────────────────────────────────────┘";
    expect.string(actual).toEqual(expected);
  });

  test("BoxLight with inner borders", ({expect}) => {
    let actual =
      <Table
        border=Inner
        borderStyle=BoxLight
        columns=[
          <ColumnConfig width=8 />,
          <ColumnConfig width=16 />,
          <ColumnConfig width=8 />,
        ]>
        <Row> "a1" "b1" "c1" </Row>
        <Row> "a2" "b2 is a cell that needs to wrap" "c2" </Row>
        <Row> "a3" "b3" "c3" "d3" </Row>
        <Row> "a4" "b4" </Row>
        <Row> "a5" "b5" "c5" </Row>
      </Table>;
    let expected =
      ""
      ++ "                                          \n"
      ++ "  a1       │ b1               │ c1        \n"
      ++ " ──────────┼──────────────────┼────────── \n"
      ++ "  a2       │ b2 is a cell     │ c2        \n"
      ++ "           │ that needs to    │           \n"
      ++ "           │ wrap             │           \n"
      ++ " ──────────┼──────────────────┼────────── \n"
      ++ "  a3       │ b3               │ c3        \n"
      ++ " ──────────┼──────────────────┼────────── \n"
      ++ "  a4       │ b4               │           \n"
      ++ " ──────────┼──────────────────┼────────── \n"
      ++ "  a5       │ b5               │ c5        \n"
      ++ "                                          ";
    expect.string(actual).toEqual(expected);
  });

  test("SimpleLines with outer borders", ({expect}) => {
    let actual =
      <Table
        border=Outer
        borderStyle=SimpleLines
        columns=[
          <ColumnConfig width=8 />,
          <ColumnConfig width=16 />,
          <ColumnConfig width=8 />,
        ]>
        <Row> "a1" "b1" "c1" </Row>
        <Row> "a2" "b2 is a cell that needs to wrap" "c2" </Row>
        <Row> "a3" "b3" "c3" "d3" </Row>
        <Row> "a4" "b4" </Row>
        <Row> "a5" "b5" "c5" </Row>
      </Table>;
    let expected =
      ""
      ++ "------------------------------------------\n"
      ++ "| a1         b1                 c1       |\n"
      ++ "|                                        |\n"
      ++ "| a2         b2 is a cell       c2       |\n"
      ++ "|            that needs to               |\n"
      ++ "|            wrap                        |\n"
      ++ "|                                        |\n"
      ++ "| a3         b3                 c3       |\n"
      ++ "|                                        |\n"
      ++ "| a4         b4                          |\n"
      ++ "|                                        |\n"
      ++ "| a5         b5                 c5       |\n"
      ++ "------------------------------------------";
    expect.string(actual).toEqual(expected);
  });

  test("SimpleLines with inner borders", ({expect}) => {
    let actual =
      <Table
        border=Inner
        borderStyle=SimpleLines
        columns=[
          <ColumnConfig width=8 />,
          <ColumnConfig width=16 />,
          <ColumnConfig width=8 />,
        ]>
        <Row> "a1" "b1" "c1" </Row>
        <Row> "a2" "b2 is a cell that needs to wrap" "c2" </Row>
        <Row> "a3" "b3" "c3" "d3" </Row>
        <Row> "a4" "b4" </Row>
        <Row> "a5" "b5" "c5" </Row>
      </Table>;
    let expected =
      ""
      ++ "                                          \n"
      ++ "  a1       | b1               | c1        \n"
      ++ " ----------+------------------+---------- \n"
      ++ "  a2       | b2 is a cell     | c2        \n"
      ++ "           | that needs to    |           \n"
      ++ "           | wrap             |           \n"
      ++ " ----------+------------------+---------- \n"
      ++ "  a3       | b3               | c3        \n"
      ++ " ----------+------------------+---------- \n"
      ++ "  a4       | b4               |           \n"
      ++ " ----------+------------------+---------- \n"
      ++ "  a5       | b5               | c5        \n"
      ++ "                                          ";
    expect.string(actual).toEqual(expected);
  });
});
