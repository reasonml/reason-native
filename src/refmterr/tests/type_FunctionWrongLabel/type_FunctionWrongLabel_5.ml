let a = 0
let thisFunction ~f args = f a ~b:(0, 2, 3, 4, 5) ~c:["anotherString"] ()

let result = thisFunction ~f:(fun ?(a=0) ~b ~c u -> a + b +c)
