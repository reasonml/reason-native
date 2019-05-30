val min : Uchar.t

val max : Uchar.t

val errChar : Uchar.t

val encodeUchar : bytes -> Uchar.t -> int

val nextInString : string -> Uchar.t * int

val decodeUchar : int -> Bytes.t -> int -> Uchar.t * int

val decodeUcharInString : int -> string -> int -> Uchar.t * int

val decodeUtf8String : string -> Uchar.t list

val validUchar : Uchar.t -> bool

val ucharLength : Uchar.t -> int

val count : Bytes.t -> int

val countInString : string -> int

val valid : Bytes.t -> bool

val validString : string -> bool
