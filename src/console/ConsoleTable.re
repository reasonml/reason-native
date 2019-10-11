module BorderStyle = ConsoleTableBorderStyle;

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
        | [] =>
          raise(Invalid_argument("Tried to take more elements than in lst"))
        | [hd, ...tl] => take(len - 1, [hd, ...initial], tl)
        }
      };
    take(length, [], lst);
  } else {
    let rec make_list = (len, value, initial) =>
      switch (len) {
      | 0 => initial
      | _ => make_list(len - 1, value, [value, ...initial])
      };
    lst @ make_list(length - List.length(lst), value, []);
  };

/**
 * Breaks a string into multiple lines, each less than the given width.
 */
let wrap_cell = (width: int, cell: string): list(string) => {
  let split_to_lines = (s: string): list(string) =>
    Str.split(Str.regexp(" "), s)
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

  switch (Pastel.parse(cell)) {
  | [] => split_to_lines(cell)
  | [(style, text)] =>
    split_to_lines(text) |> List.map(line => Pastel.apply([(style, line)]))
  | [fst, ...rest] =>
    raise(
      Invalid_argument(
        "Cannot use more than one Pastel style in a table cell",
      ),
    )
  };
};

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
let sprint_row = (dividers: BorderStyle.t, cells) => {
  let row_string = String.concat(" " ++ dividers.cell_wall ++ " ", cells);
  dividers.table_left ++ " " ++ row_string ++ " " ++ dividers.table_right;
};

/**
 * Adds a divider in between each table row
 */
let add_row_dividers = (dividers: BorderStyle.t, columns, rows) => {
  /* Add 2 to divider width to account for spaces on either side of cell */
  let row_divider =
    List.map(
      width =>
        String.concat("", pad_list(dividers.cell_bottom, width + 2, [])),
      columns,
    )
    |> String.concat(dividers.cell_wall_divider);
  let row_divider =
    dividers.table_left_divider ++ row_divider ++ dividers.table_right_divider;

  /* Add a divider before every row then remove first divider. */
  let table = List.map(row => [[row_divider], row], rows) |> List.flatten;
  switch (table) {
  | [] => []
  | [hd, ...tl] => tl
  };
};

/**
 * Adds the top of the table to the given set of rows
 */
let add_table_top = (dividers: BorderStyle.t, columns, rows) => {
  /* Add 2 to divider width to account for spaces on either side of cell */
  let table_top =
    List.map(
      width =>
        String.concat("", pad_list(dividers.table_top, width + 2, [])),
      columns,
    )
    |> String.concat(dividers.table_top_divider);
  [
    [dividers.top_left_corner ++ table_top ++ dividers.top_right_corner],
    ...rows,
  ];
};

/**
 * Adds the bottom of the table to the given set of rows
 */
let add_table_bottom = (dividers: BorderStyle.t, columns, rows) => {
  /* Add 2 to divider width to account for spaces on either side of cell */
  let table_bottom =
    List.map(
      width =>
        String.concat("", pad_list(dividers.table_bottom, width + 2, [])),
      columns,
    )
    |> String.concat(dividers.table_bottom_divider);

  rows
  @ [
    [
      dividers.bottom_left_corner
      ++ table_bottom
      ++ dividers.bottom_right_corner,
    ],
  ];
};

module ColumnConfig = {
  type t = {width: int};
  let createElement = (~children: list(unit), ~width: int, ()): t => {
    width: width,
  };
};

module Row = {
  type t = list(string);
  let createElement = (~children: list(string), ()): t => children;
};

module Table = {
  type t = string;
  let createElement =
      (
        ~columns: list(ColumnConfig.t),
        ~children as rows: list(Row.t),
        ~border: BorderStyle.border=BorderStyle.Normal,
        ~borderStyle: BorderStyle.style=BorderStyle.SimpleLines,
        (),
      )
      : t => {
    let dividers = BorderStyle.get_dividers(border, borderStyle);
    let column_widths =
      List.map((c: ColumnConfig.t) => (c.width: int), columns);

    List.map(pad_list("", List.length(column_widths)), rows)
    |> List.map(row =>
         List.map2((col, cell) => wrap_cell(col, cell), column_widths, row)
       )
    /* At this point, the data is a list of rows, with a list of cells,
       with a list of lines in that cell */
    |> List.map(lines_to_row(column_widths))
    /* The data is now a list of rows, with a list of lines,
       with a list cells in that line */
    |> List.map(row => List.map(line => sprint_row(dividers, line), row))
    /* The data has been flattened to a list of rows, with a list of lines
       in that row. The cells have been flattened into a single string */
    |> add_row_dividers(dividers, column_widths)
    |> add_table_top(dividers, column_widths)
    |> add_table_bottom(dividers, column_widths)
    |> List.flatten
    |> String.concat("\n");
  };
};
