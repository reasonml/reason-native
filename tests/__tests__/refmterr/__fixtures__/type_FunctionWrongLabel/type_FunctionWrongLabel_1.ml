let thisFunction ~f args = f ~a:["hello"] ~b:(0, 2, 3, 4, 5) ~c:["anotherString"] ()

let result = thisFunction ~f:(fun ~aa ~b ~c u -> aa + b +c)
