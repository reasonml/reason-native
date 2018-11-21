let ph = Unix.open_process_in "cmd 2&gt;&amp;1"

(* err bc Unix needs to be loaded via #load "unix.cma";; *)
(* see load_compile for further errors *)

#load "unix.cma"
