type table_printer = {
  table_left: char,
  table_right: char,
  table_top: char,
  table_bottom: char,
  cell_wall: char,
  cell_bottom: char,
  cell_divider_wall: char,
};

/**
 * Adds spaces to the right of string `s` to match the given width.
 */
let right_pad = (width, s) =>
  if (String.length(s) >= width) {
    s;
  } else {
    s ++ String.make(width - String.length(s), ' ');
  };

/**
 * Adds elements with the given value or removes elements
 * so the list matches the given length.
 */
let pad_list = (value, length, lst) =>
  if (List.length(lst) === length) {
    lst;
  } else if (List.length(lst) > length) {
    let rec take = (len, initial, lst) =>
      switch (len) {
      | 0 => List.rev(initial)
      | _ =>
        switch (lst) {
        | [] => failwith("Tried to take more elements than in lst")
        | [hd, ...tl] => take(len - 1, [hd, ...initial], tl)
        }
      };
    take(length, [], lst);
  } else {
    let rec list_of_len = (len, value, initial) =>
      switch (len) {
      | 0 => initial
      | _ => list_of_len(len - 1, value, [value, ...initial])
      };
    lst @ list_of_len(length - List.length(lst), "", []);
  };

let wrap_cell = (width: int, cell: string) =>
  /* TODO: hyphenate if single word is too long */
  /* TODO: add styling information */
  Str.split(Str.regexp(" "), cell)
  |> List.fold_left(
       (lines, word) =>
         switch (lines) {
         | [] => [word]
         | [curr, ...rest] =>
           String.length(curr) + String.length(word) + 1 > width ?
             [word, ...lines] : [curr ++ " " ++ word, ...rest]
         },
       [],
     )
  |> List.rev
  |> List.map(right_pad(width));

/**
 * Takes a list of cells with a list of lines in each cell and converts
 * it to a list of lines with a list of cells in that line.
 * e.g. [[a, b, c], [d, e, f]] -> [[a, d], [b, e], [c, f]]
 */
let lines_to_row = (columns, cells) => {
  /* The number of lines in tallest cell */
  let height =
    List.fold_left(
      (curr_max, lines) => max(curr_max, List.length(lines)),
      0,
      cells,
    );

  /* Make all cells in the row have the same number of lines. */
  let padded_cells =
    List.map(pad_list("", height), cells)
    |> List.map2(
         (width, lines) => List.map(right_pad(width), lines),
         columns,
       );

  switch (padded_cells) {
  | [] => []
  | [hd, ...tl] =>
    List.fold_left(
      (lines, col) =>
        List.map2((line, cell) => [cell, ...line], lines, col),
      List.map(cell => [cell], hd),
      tl,
    )
    |> List.map(List.rev)
  };
};

/**
 * Creates a single table row from a list of the cell contents.
 */
let sprint_row = (tp: table_printer, cells: list(string)): string => {
  let row_string =
    String.concat(" " ++ String.make(1, tp.cell_wall) ++ " ", cells);
  String.make(1, tp.table_left)
  ++ " "
  ++ row_string
  ++ " "
  ++ String.make(1, tp.table_right);
};

/**
 * Adds a divider in between each table row
 */
let add_row_dividers = (tp, columns, rows) => {
  let cell_divider_wall = String.make(1, tp.cell_divider_wall);
  /* Add 2 to divider width to account for spaces on either side of cell */
  let row_divider =
    List.map(width => String.init(width + 2, _ => tp.cell_bottom), columns)
    |> String.concat(cell_divider_wall);
  let row_divider =
    String.make(1, tp.table_left)
    ++ row_divider
    ++ String.make(1, tp.table_right);

  /* Add a divider before every row then remove first divider. */
  let table = List.map(row => [[row_divider], row], rows) |> List.flatten;
  switch (table) {
  | [] => []
  | [hd, ...tl] => tl
  };
};

let add_table_top = (tp, columns, rows) => {
  let num_column_dividers = List.length(columns) - 1;
  let num_table_walls = 2;
  let table_top =
    String.init(
      List.fold_left((acc, width) => acc + width + 2, 0, columns)
      + num_column_dividers
      + num_table_walls,
      _ =>
      tp.table_top
    );
  [[table_top], ...rows];
};

let add_table_bottom = (tp, columns, rows) => {
  let num_column_dividers = List.length(columns) - 1;
  let num_table_walls = 2;
  let table_bottom =
    String.init(
      List.fold_left((acc, width) => acc + width + 2, 0, columns)
      + num_column_dividers
      + num_table_walls,
      _ =>
      tp.table_bottom
    );
  rows @ [[table_bottom]];
};

module Row = {
  type t = list(string);
  let createElement = (~children: list(string), unit): t => children;
};

module Table = {
  type t = string;
  let createElement =
      (~columns: list(int), ~children as rows: list(Row.t), unit): t => {
    let tp = {
      table_left: '|',
      table_right: '|',
      table_top: '-',
      table_bottom: '-',
      cell_wall: '|',
      cell_bottom: '-',
      cell_divider_wall: '+',
    };

    List.map(pad_list("", List.length(columns)), rows)
    |> List.map(row =>
         List.map2((col, cell) => wrap_cell(col, cell), columns, row)
       )
    |> List.map(lines_to_row(columns))
    |> List.map(row => List.map(line => sprint_row(tp, line), row))
    |> add_row_dividers(tp, columns)
    |> add_table_top(tp, columns)
    |> add_table_bottom(tp, columns)
    |> List.flatten
    |> String.concat("\n");
  };
};
