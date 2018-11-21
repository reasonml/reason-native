type greetings =
  | Hello
  | Goodbye
  | Hola of string
  | Nihao of int
  | LongAssGreetingInSomeSuperObscureLanguageIWannaHaveALineBreakHere

let say a = match a with
| Hello -> ()
