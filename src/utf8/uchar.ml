type t = int

external fromInt : int -> t = "%identity"
external toInt : t -> int = "%identity"

let fromChar c = Char.code c

let bom = 0xFEFF

let repl = 0xFFFD

let isChar u = u < 256
