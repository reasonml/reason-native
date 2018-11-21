let () =
  let a = 5;
  print_int a

(* let binding in body needs to be let ... in ... *)

let () =
  let a = 5 in
  print_int a
