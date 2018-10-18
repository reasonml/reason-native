type chalker = string => string;

type colorName =
  | Black
  | Red
  | Green
  | Yellow
  | Blue
  | Magenta
  | Cyan
  | White
  | BlackBright
  | RedBright
  | GreenBright
  | YellowBright
  | BlueBright
  | MagentaBright
  | CyanBright
  | WhiteBright;

type color = {
  black: chalker,
  red: chalker,
  green: chalker,
  yellow: chalker,
  blue: chalker,
  magenta: chalker,
  cyan: chalker,
  white: chalker,
  blackBright: chalker,
  redBright: chalker,
  greenBright: chalker,
  yellowBright: chalker,
  blueBright: chalker,
  magentaBright: chalker,
  cyanBright: chalker,
  whiteBright: chalker,
};

type modifier = {
  bold: chalker,
  dim: chalker,
  italic: chalker,
  underline: chalker,
  inverse: chalker,
  hidden: chalker,
  strikethrough: chalker,
};

let length: string => int;

let modifier: modifier;
let color: color;
let bg: color;

let bold: chalker;
let dim: chalker;
let italic: chalker;
let underline: chalker;
let inverse: chalker;
let hidden: chalker;
let strikethrough: chalker;

let black: chalker;
let red: chalker;
let green: chalker;
let yellow: chalker;
let blue: chalker;
let magenta: chalker;
let cyan: chalker;
let white: chalker;
let blackBright: chalker;
let redBright: chalker;
let greenBright: chalker;
let yellowBright: chalker;
let blueBright: chalker;
let magentaBright: chalker;
let cyanBright: chalker;
let whiteBright: chalker;

let createElement:
  (
    ~reset: bool=?,
    ~bold: bool=?,
    ~dim: bool=?,
    ~italic: bool=?,
    ~underline: bool=?,
    ~inverse: bool=?,
    ~hidden: bool=?,
    ~strikethrough: bool=?,
    ~color: colorName=?,
    ~backgroundColor: colorName=?,
    ~children: list(string),
    unit
  ) =>
  string;
