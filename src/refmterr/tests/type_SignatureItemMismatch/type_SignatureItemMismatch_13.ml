module type Sig = sig
  type t = {
    a: string ;
    b: string ;
    c: int ;
    d: string ;
    e: bool ;
    f: string ;
    g: int ;
    h: string
  }
end

module MyModule : Sig = struct
  type t = {
    a: string ;
    b: string ;
    c: string ;
    d: int ;
    e: bool ;
    f: int ;
    g: int ;
    h: string
  }
end
