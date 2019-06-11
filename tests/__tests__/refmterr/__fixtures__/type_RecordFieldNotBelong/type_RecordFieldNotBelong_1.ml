type asd = {
  a: int;
  hello: int;
}

type lol =
  | Something of asd

let bla (Something {a; b}) = 1
