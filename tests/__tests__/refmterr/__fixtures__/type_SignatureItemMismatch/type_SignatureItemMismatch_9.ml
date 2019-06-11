(* Test wrongly specified type *)
module type Sig = sig
  type wrongType = int
end

module MyModule : Sig = struct
  type wrongType = float
end

