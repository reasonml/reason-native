let xyz = 0
let thisFunction ~f args = f xyz ~a:["hello"] ~b:(0, 2, 3, 4, 5) ~c:["anotherString"] ()

let result = thisFunction ~f:(fun ~a:0 ~b ~c u -> aa + b +c)
