let omNomNomArgs a b = (ignore (b 1)); "hello world!"

let () = print_endline (omNomNomArgs 1 (fun x -> "a") 1)
