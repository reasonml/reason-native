(* Test two missing *)

module type Sig = sig
  val createElement: string -> unit
  val createElement2: string -> unit
end

module MyModule : Sig = struct
  let createElement3 s = ()
end
