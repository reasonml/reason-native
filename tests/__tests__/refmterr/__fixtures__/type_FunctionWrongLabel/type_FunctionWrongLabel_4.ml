let thisFunction ~f args = f ?a:(Some(["hello"])) ~b:(0, 2, 3, 4, 5) ~c:["anotherString"] ()

let result = thisFunction ~f:(fun ~a ~b ~c u -> a + b +c)
