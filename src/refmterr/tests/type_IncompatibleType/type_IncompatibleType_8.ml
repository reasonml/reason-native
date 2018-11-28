
type reallyReallyLongNameWillBreak
let numbers = (0, 2, 3, 4, 5)
let thisFunction (f: (reallyReallyLongNameWillBreak * reallyReallyLongNameWillBreak * reallyReallyLongNameWillBreak * reallyReallyLongNameWillBreak * reallyReallyLongNameWillBreak) -> (reallyReallyLongNameWillBreak * reallyReallyLongNameWillBreak * reallyReallyLongNameWillBreak * reallyReallyLongNameWillBreak * reallyReallyLongNameWillBreak) -> unit list -> string list -> 'args) args = f numbers numbers [(); ()] ["anotherString"] ()

let result = thisFunction (fun aa c u -> aa + b +c)








