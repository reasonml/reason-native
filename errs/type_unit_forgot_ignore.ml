let () =
  let r = Str.regexp "hello \\([A-Za-z]+\\)" in
  Str.replace_first r "\\1" "hello world"

(* err bc type don't match, doesn't work adding () at the end *)

let () =
  let r = Str.regexp "hello \\([A-Za-z]+\\)" in
  ignore (Str.replace_first r "\\1" "hello world")
