type t;
let create: (string, t) => t;
let createAtom: string => t;
let createExpression: list(t) => t;
let toSexp: t => Sexplib.Sexp.t;
let ofString: string => t;
