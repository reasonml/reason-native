let parseFromStdin
  ~(refmttypePath: string option)
  ~(customLogOutputProcessors : (string -> string) list)
  ~(customErrorParsers :  (string * string list) list) = ()

let refmttypePath = Some "hi"

let _ =
  parseFromStdin
    ~refmttypePath
    ~raiseExceptionDuringParse=true
    ~customLogOutputProcessors=[]
    ~customErrorParsers:[]
