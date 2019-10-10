/* These are strings, even though they must be a single character
   as char does not allow for unicode characters */
type t = {
  table_left: string,
  table_left_divider: string,
  table_right: string,
  table_right_divider: string,
  table_top: string,
  table_top_divider: string,
  table_bottom: string,
  table_bottom_divider: string,
  cell_wall: string,
  cell_wall_divider: string,
  cell_bottom: string,
  top_left_corner: string,
  top_right_corner: string,
  bottom_left_corner: string,
  bottom_right_corner: string,
};

type border =
  | Normal
  | Outer
  | Inner
  | None;

type style =
  | BoxLight
  | SimpleLines;

let get_dividers = (b: border, s: style): t =>
  switch (b, s) {
  | (Normal, BoxLight) => {
      table_left: "│",
      table_left_divider: "├",
      table_right: "│",
      table_right_divider: "┤",
      table_top: "─",
      table_top_divider: "┬",
      table_bottom: "─",
      table_bottom_divider: "┴",
      cell_wall: "│",
      cell_wall_divider: "┼",
      cell_bottom: "─",
      top_left_corner: "┌",
      top_right_corner: "┐",
      bottom_left_corner: "└",
      bottom_right_corner: "┘",
    }
  | (Normal, SimpleLines) => {
      table_left: "|",
      table_left_divider: "|",
      table_right: "|",
      table_right_divider: "|",
      table_top: "-",
      table_top_divider: "-",
      table_bottom: "-",
      table_bottom_divider: "-",
      cell_wall: "|",
      cell_wall_divider: "+",
      cell_bottom: "-",
      top_left_corner: "-",
      top_right_corner: "-",
      bottom_left_corner: "-",
      bottom_right_corner: "-",
    }
  | (Outer, BoxLight) => {
      table_left: "│",
      table_left_divider: "│",
      table_right: "│",
      table_right_divider: "│",
      table_top: "─",
      table_top_divider: "─",
      table_bottom: "─",
      table_bottom_divider: "─",
      cell_wall: " ",
      cell_wall_divider: " ",
      cell_bottom: " ",
      top_left_corner: "┌",
      top_right_corner: "┐",
      bottom_left_corner: "└",
      bottom_right_corner: "┘",
    }
  | (Outer, SimpleLines) => {
      table_left: "|",
      table_left_divider: "|",
      table_right: "|",
      table_right_divider: "|",
      table_top: "-",
      table_top_divider: "-",
      table_bottom: "-",
      table_bottom_divider: "-",
      cell_wall: " ",
      cell_wall_divider: " ",
      cell_bottom: " ",
      top_left_corner: "-",
      top_right_corner: "-",
      bottom_left_corner: "-",
      bottom_right_corner: "-",
    }
  | (Inner, BoxLight) => {
      table_left: " ",
      table_left_divider: " ",
      table_right: " ",
      table_right_divider: " ",
      table_top: " ",
      table_top_divider: " ",
      table_bottom: " ",
      table_bottom_divider: " ",
      cell_wall: "│",
      cell_wall_divider: "┼",
      cell_bottom: "─",
      top_left_corner: " ",
      top_right_corner: " ",
      bottom_left_corner: " ",
      bottom_right_corner: " ",
    }
  | (Inner, SimpleLines) => {
      table_left: " ",
      table_left_divider: " ",
      table_right: " ",
      table_right_divider: " ",
      table_top: " ",
      table_top_divider: " ",
      table_bottom: " ",
      table_bottom_divider: " ",
      cell_wall: "|",
      cell_wall_divider: "+",
      cell_bottom: "-",
      top_left_corner: " ",
      top_right_corner: " ",
      bottom_left_corner: " ",
      bottom_right_corner: " ",
    }
  | (None, _) => {
      table_left: " ",
      table_left_divider: " ",
      table_right: " ",
      table_right_divider: " ",
      table_top: " ",
      table_top_divider: " ",
      table_bottom: " ",
      table_bottom_divider: " ",
      cell_wall: " ",
      cell_wall_divider: " ",
      cell_bottom: " ",
      top_left_corner: " ",
      top_right_corner: " ",
      bottom_left_corner: " ",
      bottom_right_corner: " ",
    }
  };
