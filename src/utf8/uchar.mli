type t

val bom : t

(* unicode replacement character *)
val repl : t

val fromInt : int -> t

val toInt : t -> int

val fromChar : char -> t

val isChar : t -> bool
