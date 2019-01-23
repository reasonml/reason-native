type decorator = string => string;

type modifier = {
  bold: decorator,
  dim: decorator,
  italic: decorator,
  underline: decorator,
  inverse: decorator,
  hidden: decorator,
  strikethrough: decorator,
};

type color = {
  black: decorator,
  red: decorator,
  green: decorator,
  yellow: decorator,
  blue: decorator,
  magenta: decorator,
  cyan: decorator,
  white: decorator,
  blackBright: decorator,
  redBright: decorator,
  greenBright: decorator,
  yellowBright: decorator,
  blueBright: decorator,
  magentaBright: decorator,
  cyanBright: decorator,
  whiteBright: decorator,
};
