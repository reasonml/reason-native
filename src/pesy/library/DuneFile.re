open Sexplib.Sexp;
let toString = (stanzas: list(Stanza.t)) =>
  List.fold_right(
    (s, acc) => to_string_hum(~indent=4, Stanza.toSexp(s)) ++ " " ++ acc,
    stanzas,
    "",
  );
