
module type Sig = sig
  val createElement: string -> unit
end

module MyModule : Sig = struct
  let createElement2 s = ()
end
