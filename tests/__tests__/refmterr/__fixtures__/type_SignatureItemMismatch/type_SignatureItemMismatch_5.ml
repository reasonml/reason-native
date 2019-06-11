(* Test wrong type all by itself *)
module type Sig = sig
  val hasWrongType: stringParam:string -> intParam:int -> unitParam:unit -> anotherStringParam:string -> unit
end

module MyModule : Sig = struct
  let hasWrongType s = 0
end

