/* These are strings, even though they must be a single character
   as char does not allow for unicode characters */
type t = {
  tableLeft: string,
  tableLeftDivider: string,
  tableRight: string,
  tableRightDivider: string,
  tableTop: string,
  tableTopDivider: string,
  tableBottom: string,
  tableBottomDivider: string,
  cellWall: string,
  cellWallDivider: string,
  cellBottom: string,
  topLeftCorner: string,
  topRightCorner: string,
  bottomLeftCorner: string,
  bottomRightCorner: string,
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
      tableLeft: "│",
      tableLeftDivider: "├",
      tableRight: "│",
      tableRightDivider: "┤",
      tableTop: "─",
      tableTopDivider: "┬",
      tableBottom: "─",
      tableBottomDivider: "┴",
      cellWall: "│",
      cellWallDivider: "┼",
      cellBottom: "─",
      topLeftCorner: "┌",
      topRightCorner: "┐",
      bottomLeftCorner: "└",
      bottomRightCorner: "┘",
    }
  | (Normal, SimpleLines) => {
      tableLeft: "|",
      tableLeftDivider: "|",
      tableRight: "|",
      tableRightDivider: "|",
      tableTop: "-",
      tableTopDivider: "-",
      tableBottom: "-",
      tableBottomDivider: "-",
      cellWall: "|",
      cellWallDivider: "+",
      cellBottom: "-",
      topLeftCorner: "-",
      topRightCorner: "-",
      bottomLeftCorner: "-",
      bottomRightCorner: "-",
    }
  | (Outer, BoxLight) => {
      tableLeft: "│",
      tableLeftDivider: "│",
      tableRight: "│",
      tableRightDivider: "│",
      tableTop: "─",
      tableTopDivider: "─",
      tableBottom: "─",
      tableBottomDivider: "─",
      cellWall: " ",
      cellWallDivider: " ",
      cellBottom: " ",
      topLeftCorner: "┌",
      topRightCorner: "┐",
      bottomLeftCorner: "└",
      bottomRightCorner: "┘",
    }
  | (Outer, SimpleLines) => {
      tableLeft: "|",
      tableLeftDivider: "|",
      tableRight: "|",
      tableRightDivider: "|",
      tableTop: "-",
      tableTopDivider: "-",
      tableBottom: "-",
      tableBottomDivider: "-",
      cellWall: " ",
      cellWallDivider: " ",
      cellBottom: " ",
      topLeftCorner: "-",
      topRightCorner: "-",
      bottomLeftCorner: "-",
      bottomRightCorner: "-",
    }
  | (Inner, BoxLight) => {
      tableLeft: " ",
      tableLeftDivider: " ",
      tableRight: " ",
      tableRightDivider: " ",
      tableTop: " ",
      tableTopDivider: " ",
      tableBottom: " ",
      tableBottomDivider: " ",
      cellWall: "│",
      cellWallDivider: "┼",
      cellBottom: "─",
      topLeftCorner: " ",
      topRightCorner: " ",
      bottomLeftCorner: " ",
      bottomRightCorner: " ",
    }
  | (Inner, SimpleLines) => {
      tableLeft: " ",
      tableLeftDivider: " ",
      tableRight: " ",
      tableRightDivider: " ",
      tableTop: " ",
      tableTopDivider: " ",
      tableBottom: " ",
      tableBottomDivider: " ",
      cellWall: "|",
      cellWallDivider: "+",
      cellBottom: "-",
      topLeftCorner: " ",
      topRightCorner: " ",
      bottomLeftCorner: " ",
      bottomRightCorner: " ",
    }
  | (None, _) => {
      tableLeft: " ",
      tableLeftDivider: " ",
      tableRight: " ",
      tableRightDivider: " ",
      tableTop: " ",
      tableTopDivider: " ",
      tableBottom: " ",
      tableBottomDivider: " ",
      cellWall: " ",
      cellWallDivider: " ",
      cellBottom: " ",
      topLeftCorner: " ",
      topRightCorner: " ",
      bottomLeftCorner: " ",
      bottomRightCorner: " ",
    }
  };
