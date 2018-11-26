(* GADT list that exposes the type of the head element *)
type _ hlist =
 | Nil: 'a hlist
 | Cons: ('a * 'b hlist) -> 'a hlist

let rec len (type a) (l: a hlist): int = match l with
  | Nil -> 0
  | Cons (h, t) -> 1 + len t
