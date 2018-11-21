module Abc = struct
  type myInt = int
  type myString = string
end

module Def = struct
  type myFloat = float
  type myBool = bool
end

let inc (n: Abc.myInt): Def.myFloat = 1. +. float_of_int n

let dec (f: Abc.myString -> Def.myBool) = 1

let () = dec inc
