(* Test wrong arity *)
module type Sig = sig
  type 'a wrongArity
end

module MyModule : Sig = struct
  type wrongArity = int
end

