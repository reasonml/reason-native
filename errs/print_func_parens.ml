let () = print_string String.capitalize "asd"

(* err bc need parens, precedence *)

let () = print_string (String.capitalize "asd")
