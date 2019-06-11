(* http://stackoverflow.com/questions/16005801/why-cant-ocaml-infer-the-following-type *)
module type Foo = sig
  type t
  val do_with_t : t -> unit
end

let any_foo t (module F : Foo) = F.do_with_t t
