module BorderStyle = TableBorderStyle;

/**
 * Adds spaces to the right of string `s` to match the given width.
 */
let rightPad = (width, s) =>
  if (String.length(s) >= width) {
    s;
  } else {
    s ++ String.make(width - String.length(s), ' ');
  };

/**
 * Adds elements with the given value or removes elements
 * so the list matches the given length.
 */
let padList = (value, length, lst) =>
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
    let rec makeList = (len, value, initial) =>
      switch (len) {
      | 0 => initial
      | _ => makeList(len - 1, value, [value, ...initial])
      };
    lst @ makeList(length - List.length(lst), value, []);
  };

/**
 * Breaks a string into multiple lines, each less than the given width.
 */
let wrapCell = (width: int, cell: string): list(string) => {
  let splitToLines = (s: string): list(string) =>
    Re.split(Re.Pcre.regexp(" "), s)
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
    |> List.map(rightPad(width));

  switch (Pastel.unstable_parse(cell)) {
  | [] => splitToLines(cell)
  | [(style, text)] =>
    splitToLines(text)
    |> List.map(line => Pastel.unstable_apply([(style, line)]))
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
let linesToRow = (columns, cells) => {
  /* The number of lines in tallest cell */
  let height =
    List.fold_left(
      (currMax, lines) => max(currMax, List.length(lines)),
      0,
      cells,
    );

  /* Make all cells in the row have the same number of lines. */
  let paddedCells =
    List.map(padList("", height), cells)
    |> List.map2(
         (width, lines) => List.map(rightPad(width), lines),
         columns,
       );

  switch (paddedCells) {
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
let sprintRow = (dividers: BorderStyle.t, cells) => {
  let rowString = String.concat(" " ++ dividers.cellWall ++ " ", cells);
  dividers.tableLeft ++ " " ++ rowString ++ " " ++ dividers.tableRight;
};

/**
 * Adds a divider in between each table row
 */
let addRowDividers = (dividers: BorderStyle.t, columns, rows) => {
  /* Add 2 to divider width to account for spaces on either side of cell */
  let rowDivider =
    List.map(
      width =>
        String.concat("", padList(dividers.cellBottom, width + 2, [])),
      columns,
    )
    |> String.concat(dividers.cellWallDivider);
  let rowDivider =
    dividers.tableLeftDivider ++ rowDivider ++ dividers.tableRightDivider;

  /* Add a divider before every row then remove first divider. */
  let table = List.map(row => [[rowDivider], row], rows) |> List.flatten;
  switch (table) {
  | [] => []
  | [hd, ...tl] => tl
  };
};

/**
 * Adds the top of the table to the given set of rows
 */
let addTableTop = (dividers: BorderStyle.t, columns, rows) => {
  /* Add 2 to divider width to account for spaces on either side of cell */
  let tableTop =
    List.map(
      width => String.concat("", padList(dividers.tableTop, width + 2, [])),
      columns,
    )
    |> String.concat(dividers.tableTopDivider);
  [[dividers.topLeftCorner ++ tableTop ++ dividers.topRightCorner], ...rows];
};

/**
 * Adds the bottom of the table to the given set of rows
 */
let addTableBottom = (dividers: BorderStyle.t, columns, rows) => {
  /* Add 2 to divider width to account for spaces on either side of cell */
  let tableBottom =
    List.map(
      width =>
        String.concat("", padList(dividers.tableBottom, width + 2, [])),
      columns,
    )
    |> String.concat(dividers.tableBottomDivider);

  rows
  @ [
    [dividers.bottomLeftCorner ++ tableBottom ++ dividers.bottomRightCorner],
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
    let columnWidths =
      List.map((c: ColumnConfig.t) => (c.width: int), columns);

    List.map(padList("", List.length(columnWidths)), rows)
    |> List.map(row =>
         List.map2((col, cell) => wrapCell(col, cell), columnWidths, row)
       )
    /* At this point, the data is a list of rows, with a list of cells,
       with a list of lines in that cell */
    |> List.map(linesToRow(columnWidths))
    /* The data is now a list of rows, with a list of lines,
       with a list cells in that line */
    |> List.map(row => List.map(line => sprintRow(dividers, line), row))
    /* The data has been flattened to a list of rows, with a list of lines
       in that row. The cells have been flattened into a single string */
    |> addRowDividers(dividers, columnWidths)
    |> addTableTop(dividers, columnWidths)
    |> addTableBottom(dividers, columnWidths)
    |> List.flatten
    |> String.concat("\n");
  };
};
