type greetings =
  | Hello
  | Goodbye

let say a = match a with
| Hello -> ()
| Goodbye -> ()
| _ -> ()
