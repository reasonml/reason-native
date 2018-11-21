(* Test wrongly specified type *)
module type Sig = sig
  type 'a wrongArity = int
  type wrongType = int
end

module MyModule : Sig = struct
  type wrongArity = float
  type wrongType = float
end

