open Lwt;
PesyPreprocess.Util.foo();

let%lwt foo = Lwt.return("world");
print_endline(foo);
