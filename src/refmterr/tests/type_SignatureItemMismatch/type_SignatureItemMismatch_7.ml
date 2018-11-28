(* Test missing type all by itself *)
module type Sig = sig
  val createElement: string -> unit
  type 'a missingType = 'a list
end

module MyModule : Sig = struct
  let createElement s = ()
end

