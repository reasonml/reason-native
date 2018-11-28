(* Test two wrong types *)
module type Sig = sig
  val hasWrongType: string -> unit
  val hasWrongType2: string -> unit
end

module MyModule : Sig = struct
  let hasWrongType s = 0
  let hasWrongType2 s = "wrong"
end

