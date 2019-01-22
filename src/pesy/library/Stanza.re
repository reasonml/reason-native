open Sexplib.Sexp;
type t = Sexplib.Sexp.t;
let createAtom = a => Atom(a);
let create = (stanza: string, expression) =>
  List([Atom(stanza), expression]);
let createExpression = atoms => List(atoms);
let ofString = s =>
  Sexplib.Parser.sexp(Sexplib.Lexer.main, Lexing.from_string(s));
let toSexp = x => x;
