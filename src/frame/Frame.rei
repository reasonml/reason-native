module BorderStyle: {
  type style =
    | BoxLight
    | SimpleLines;

  type border =
    | Normal
    | Outer
    | Inner
    | None;
};

module ColumnConfig: {
  type t = {width: int};
  let createElement: (~children: list(unit), ~width: int, unit) => t;
};

module Row: {
  type t = list(string);
  let createElement: (~children: list(string), unit) => t;
};

module Table: {
  type t = string;
  let createElement:
    (
      ~columns: list(ColumnConfig.t),
      ~children: list(Row.t),
      ~border: BorderStyle.border=?,
      ~borderStyle: BorderStyle.style=?,
      unit
    ) =>
    t;
};
