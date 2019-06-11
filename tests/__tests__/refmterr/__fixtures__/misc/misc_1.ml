let pad ?(ch=' ') content n =
  (String.make (n - (String.length content)) ~ch) ^ content
  (* should be ch, not ~ch *)

let () = print_endline @@ pad "1" 2
