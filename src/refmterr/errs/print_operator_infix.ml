let () = print_int 5 + 6

(* err bc need parens, precedence *)

let () = print_int (5 + 6)
