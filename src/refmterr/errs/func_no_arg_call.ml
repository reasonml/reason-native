let asd () = 5;;
print_int asd;;

(* err bc func with 0 arg need to be appended () *)

let asd () = 5;;
print_int (asd ());;
