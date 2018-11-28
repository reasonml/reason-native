let () = print_endline "asd"

let r = 5 in print_int r

(* error bc forgot semicolon and top level confusing-ness *)

let () = print_endline "asd";;

let r = 5 in print_int r
