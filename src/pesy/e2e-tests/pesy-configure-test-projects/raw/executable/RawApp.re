let ic = open_in(Filename.dirname(Sys.argv[0]) ++ "/../share/asset.txt");
let line = input_line(ic);
let%lwt foo = Lwt.return(line);
print_endline(foo);
