(* Test missing type among other missing values *)
module type Sig = sig
  val createElement: string -> unit
  type 'a missingType = 'a list
  val createElement2: string -> unit
  val hasWrongType: string -> unit
end

module MyModule : Sig = struct
  let hasWrongType s = 0
end

