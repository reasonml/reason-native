module Row: {
  type t = list(string);
  let createElement: (~children: list(string), unit) => list(string);
};

module Table: {
  type t = string;
  let createElement:
    (~columns: list(int), ~children: list(Row.t), unit) => string;
};
