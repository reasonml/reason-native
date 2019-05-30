(* https://tools.ietf.org/html/rfc3629#section-10 *)
let min = Uchar.fromInt 0x0000
let max = Uchar.fromInt 0x10FFFF

let surrogateMin = Uchar.fromInt 0xD800
let surrogateMax = Uchar.fromInt 0xDFFF

let errChar = Uchar.repl

(*
 * Char. number range  |        UTF-8 octet sequence
 *       (hexadecimal)    |              (binary)
 *    --------------------+---------------------------------------------
 *    0000 0000-0000 007F | 0xxxxxxx
 *    0000 0080-0000 07FF | 110xxxxx 10xxxxxx
 *    0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
 *    0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 *)
let h2 = 0b1100_0000
let h3 = 0b1110_0000
let h4 = 0b1111_0000

let cont_mask = 0b0011_1111

let encodeUchar bytes c =
  let c = Uchar.toInt c in
  (* TODO check negative? *)
  if c <= 127 then begin
    Bytes.set bytes 0 (Char.unsafe_chr c);
     1
  end else if c <= 2047 then begin
    Bytes.set bytes 0 (Char.unsafe_chr (h2 lor (c lsr 6)));
    Bytes.set bytes 1 (Char.unsafe_chr (0b1000_0000 lor (c land cont_mask)));
    2
  end else if c <= 65535 then begin
    Bytes.set bytes 0 (Char.unsafe_chr (h3 lor (c lsr 12)));
    Bytes.set bytes 1 (Char.unsafe_chr (0b1000_0000 lor ((c lsr 6) land cont_mask)));
    Bytes.set bytes 2 (Char.unsafe_chr (0b1000_0000 lor (c land cont_mask)));
    3
  end else if c <= Uchar.toInt max then begin
    Bytes.set bytes 0 (Char.unsafe_chr (h4 lor (c lsr 18)));
    Bytes.set bytes 1 (Char.unsafe_chr (0b1000_0000 lor ((c lsr 12) land cont_mask)));
    Bytes.set bytes 2 (Char.unsafe_chr (0b1000_0000 lor ((c lsr 6) land cont_mask)));
    Bytes.set bytes 3 (Char.unsafe_chr (0b1000_0000 lor (c land cont_mask)));
    4
  end else -1

type category = {
  low: int;
  high: int;
  size: int;
}

let locb = 0b1000_0000
let hicb = 0b1011_1111

let categoryTable = [|
(* 0 *) {low = -1; high= -1; size= 1}; (* invalid *)
(* 1 *) {low = 1; high= -1; size= 1}; (* ascii *)
(* 2 *) {low = locb; high= hicb; size= 2};
(* 3 *) {low = 0xA0; high= hicb; size= 3};
(* 4 *) {low = locb; high= hicb; size= 3};
(* 5 *) {low = locb; high= 0x9F; size= 3};
(* 6 *) {low = 0x90; high= hicb;  size= 4};
(* 7 *) {low = locb; high= hicb; size= 4};
(* 8 *) {low = locb; high= 0x8F; size= 4};

|]

let categories = [|
  1; 1; 1; 1; 1; 1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1;
  1; 1; 1; 1; 1; 1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1;
  1; 1; 1; 1; 1; 1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1;
  1; 1; 1; 1; 1; 1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1;
  1; 1; 1; 1; 1; 1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1;
  1; 1; 1; 1; 1; 1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1;
  1; 1; 1; 1; 1; 1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1;
  1; 1; 1; 1; 1; 1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1 ;1;

  0; 0; 0; 0;0; 0; 0; 0;0; 0; 0; 0;0; 0; 0; 0;
  0; 0; 0; 0;0; 0; 0; 0;0; 0; 0; 0;0; 0; 0; 0;
  0; 0; 0; 0;0; 0; 0; 0;0; 0; 0; 0;0; 0; 0; 0;
  0; 0; 0; 0;0; 0; 0; 0;0; 0; 0; 0;0; 0; 0; 0;
  (* surrogate range U+D800 - U+DFFFF = 55296 - 917503 *)
  0; 0; 2; 2;2; 2; 2; 2;2; 2; 2; 2;2; 2; 2; 2;
   2; 2; 2; 2; 2; 2; 2; 2; 2; 2; 2; 2; 2; 2; 2; 2;
   3; 4; 4; 4; 4; 4; 4; 4; 4; 4; 4; 4; 4; 5; 4; 4;
  6; 7; 7 ;7; 8; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0; 0;
|]

(* Next unpacks the first UT8-encoding in bytes and returns the codepoint with
 * its corresponding width in bytes *)
let nextInString s =
  let len = String.length s in
  if len < 1 then (errChar, 0) else
  let c0 = int_of_char s.[0] in
  if (c0 < 128) then (Uchar.fromInt c0, 1) else
  let ci = categories.(c0) in
  if ci = 0 then (errChar, 1) else
    let cat = categoryTable.(ci) in
    if len < cat.size then (errChar, 1)
    else if cat.size == 2 then
        let c1 = int_of_char s.[1] in
        if c1 < cat.low || cat.high < c1 then (errChar, 1)
        else
          let i1 = c1 land 0b00111111 in
          let i0 = (c0 land 0b00011111) lsl 6 in
          let uc = i0 lor i1 in
          (Uchar.fromInt uc, 2)
    else if cat.size == 3 then
          let c1 = int_of_char s.[1] in
          let c2 = int_of_char s.[2] in
          if c1 < cat.low || cat.high < c1
            || c2 < locb || hicb < c2
          then (errChar, 1)
          else
            let i0 = (c0 land 0b00001111) lsl 12 in
            let i1 = (c1 land 0b00111111) lsl 6 in
            let i2 = (c2 land 0b00111111) in
            let uc = i0 lor i1 lor i2 in
            (Uchar.fromInt uc, 3)
    else
        let c1 = int_of_char s.[1] in
        let c2 = int_of_char s.[2] in
        let c3 = int_of_char s.[3] in
        if c1 < cat.low || cat.high < c1
          || c2 < locb || c2 < hicb
          || c3 < locb || c3 < hicb
        then (errChar, 1)
        else
          let i0 = (c0 land 0x07) lsl 18 in
          let i1 = (c1 land 0x3f) lsl 12 in
          let i2 = (c2 land 0x3f) lsl 6 in
          let i3 = (c3 land 0x3f) in
          let uc = i0 lor i3 lor i2 lor i1 in
          (Uchar.fromInt uc, 4)

let decodeUchar i b len =
  if len < 1 then (errChar, 1) else
  let first = int_of_char (Bytes.unsafe_get b i) in
  if first < 128 then (Uchar.fromInt first, 1) else
  let index = categories.(first) in
  if index = 0 then
    (errChar, 1)
  else
    let cat = categoryTable.(index) in
    if len < i + cat.size then
      (errChar, 1)
    else if cat.size == 2 then
        let c1 = int_of_char (Bytes.unsafe_get b (i +1)) in
        if c1 < cat.low || cat.high < c1 then
          (errChar, 1)
        else
          let i1 = c1 land 0b00111111 in
          let i0 = (first land 0b00011111) lsl 6 in
          let uc = i0 lor i1 in
          (Uchar.fromInt uc, 2)
    else if cat.size == 3 then
      let c1 = int_of_char (Bytes.unsafe_get b (i +1)) in
      let c2 = int_of_char (Bytes.unsafe_get b (i +2)) in
      if c1 < cat.low || cat.high < c1 || c2 < locb || hicb < c2 then (errChar, 1)
      else
        let i0 = (first land 0b00001111) lsl 12 in
        let i1 = (c1 land 0b00111111) lsl 6 in
        let i2 = (c2 land 0b00111111) in
        let uc = i0 lor i1 lor i2 in
        (Uchar.fromInt uc, 3)
    else
      let c1 = int_of_char (Bytes.unsafe_get b (i +1)) in
      let c2 = int_of_char (Bytes.unsafe_get b (i +2)) in
      let c3 = int_of_char (Bytes.unsafe_get b (i +3)) in
      if c1 < cat.low || cat.high < c1 ||
         c2 < locb || hicb < c2 || c3 < locb || hicb < c3
      then (errChar, 1)
      else
        let i1 = (c1 land 0x3f) lsl 12 in
        let i2 = (c2 land 0x3f) lsl 6 in
        let i3 = (c3 land 0x3f) in
        let i0 = (first land 0x07) lsl 18 in
        let uc = i0 lor i3 lor i2 lor i1 in
        (Uchar.fromInt uc, 4)


let decodeUcharInString i b len =
  if len < 1 then (errChar, 1) else
  let first = int_of_char (String.unsafe_get b i) in
  if first < 128 then (Uchar.fromInt first, 1) else
  let index = categories.(first) in
  if index = 0 then (errChar, 1) else
  let cat = categoryTable.(index) in
  if len < i + cat.size then (errChar, 1)
  else
  if cat.size == 2 then
      let c1 = int_of_char (String.unsafe_get b (i +1)) in
      if c1 < cat.low || cat.high < c1 then
        (errChar, 1)
      else
        let i1 = c1 land 0b00111111 in
        let i0 = (first land 0b00011111) lsl 6 in
        let uc = i0 lor i1 in
        (Uchar.fromInt uc, 2)
  else if cat.size == 3 then
    let c1 = int_of_char (String.unsafe_get b (i +1)) in
    let c2 = int_of_char (String.unsafe_get b (i +2)) in
    if c1 < cat.low || cat.high < c1 || c2 < locb || hicb < c2 then (errChar, 1)
    else
      let i0 = (first land 0b00001111) lsl 12 in
      let i1 = (c1 land 0b00111111) lsl 6 in
      let i2 = (c2 land 0b00111111) in
      let uc = i0 lor i1 lor i2 in
      (Uchar.fromInt uc, 3)
  else
    let c1 = int_of_char (String.unsafe_get b (i +1)) in
    let c2 = int_of_char (String.unsafe_get b (i +2)) in
    let c3 = int_of_char (String.unsafe_get b (i +3)) in
    if c1 < cat.low || cat.high < c1 ||
       c2 < locb || hicb < c2 || c3 < locb || hicb < c3
    then (errChar, 1)
    else
      let i1 = (c1 land 0x3f) lsl 12 in
      let i2 = (c2 land 0x3f) lsl 6 in
      let i3 = (c3 land 0x3f) in
      let i0 = (first land 0x07) lsl 18 in
      let uc = i0 lor i3 lor i2 lor i1 in
      (Uchar.fromInt uc, 4)

let decodeUtf8String s =
  let lst = ref [] in
  let add elem = lst := elem :: !lst in
  let rec decode i s slen =
    if (i == slen) then ()
    else
      let (codePoint, offset) = decodeUcharInString i s slen in
      add codePoint;
      decode (i + offset) s slen
  in
  decode 0 s (String.length s);
  List.rev !lst


let validUchar uc = match (Uchar.toInt uc) with
  | c when (0 <= c && c < Uchar.toInt surrogateMin) -> true
	| c when (Uchar.toInt surrogateMax < c && c <= Uchar.toInt max) -> true
  | _ -> false

let ucharLength uc = match (Uchar.toInt uc) with
  | u when u < 0 -> -1
  | u when u <= 127 -> 1
  | u when u <= 2047 -> 2
  | u when Uchar.toInt surrogateMin <= u && u <=  Uchar.toInt surrogateMax -> -1
  | u when u <= 65535 -> 3
  | u when u <= Uchar.toInt max -> 4
  | _ -> -1

let count b =
	let len = Bytes.length b in
  let i = ref 0 in
  let offset = ref 0 in
  while (!offset < len) do (
    let c0 = int_of_char (Bytes.unsafe_get b !offset) in
    if c0 < 128 then begin
      i := !i + 1;
      offset := !offset + 1;
    end else
  let index = categories.(c0) in
  if index == 0 then begin
    i := !i + 1;
    offset := !offset + 1;
  end else
    let cat = categoryTable.(index) in
    if len < !i + cat.size then begin
      i := !i + 1;
      offset := !offset + 1;
    end else
    if cat.size == 2 then
        let c1 = int_of_char (Bytes.unsafe_get b (!offset + 1)) in
        if c1 < cat.low || cat.high < c1 then begin
          i := !i + 1;
          offset := !offset + 1;
        end else begin
          i := !i + 1;
          offset := !offset + 2;
        end
    else if cat.size == 3 then
      let c1 = int_of_char (Bytes.unsafe_get b (!offset + 1)) in
      let c2 = int_of_char (Bytes.unsafe_get b (!offset + 2)) in
        if c1 > cat.low || cat.high < c1 || c2 < locb || hicb < c2
        then begin
          i := !i + 1;
          offset := !offset + 1;
        end else begin
          i := !i + 1;
          offset := !offset + 3;
        end
    else
      let c1 = int_of_char (Bytes.unsafe_get b (!offset + 1)) in
      let c2 = int_of_char (Bytes.unsafe_get b (!offset + 2)) in
      let c3 = int_of_char (Bytes.unsafe_get b (!offset + 3)) in
      if c1 < cat.low || cat.high < c1 ||
         c2 < locb || hicb < c2 || c3 < locb || hicb < c3
        then  begin
          i := !i + 1;
          offset := !offset + 1;
      end else begin
          i := !i + 1;
          offset := !offset + 4;
        end
  ) done;
  !i


let countInString s =
	let len = String.length s in
  let i = ref 0 in
  let offset = ref 0 in
  while (!offset < len) do (
    let c0 = int_of_char (String.unsafe_get s !offset) in
    if c0 < 128 then begin
      i := !i + 1;
      offset := !offset + 1;
    end else
  let index = categories.(c0) in
  if index == 0 then begin
    i := !i + 1;
    offset := !offset + 1;
  end else
    let cat = categoryTable.(index) in
    if len < !i + cat.size then begin
      i := !i + 1;
      offset := !offset + 1;
    end else
    if cat.size == 2 then
        let c1 = int_of_char (String.unsafe_get s (!offset + 1)) in
        if c1 < cat.low || cat.high < c1 then begin
          i := !i + 1;
          offset := !offset + 1;
        end else begin
          i := !i + 1;
          offset := !offset + 2;
        end
    else if cat.size == 3 then
      let c1 = int_of_char (String.unsafe_get s (!offset + 1)) in
      let c2 = int_of_char (String.unsafe_get s (!offset + 2)) in
        if c1 > cat.low || cat.high < c1 || c2 < locb || hicb < c2
        then begin
          i := !i + 1;
          offset := !offset + 1;
        end else begin
          i := !i + 1;
          offset := !offset + 3;
        end
    else
      let c1 = int_of_char (String.unsafe_get s (!offset + 1)) in
      let c2 = int_of_char (String.unsafe_get s (!offset + 2)) in
      let c3 = int_of_char (String.unsafe_get s (!offset + 3)) in
      if c1 < cat.low || cat.high < c1 ||
         c2 < locb || hicb < c2 || c3 < locb || hicb < c3
        then  begin
          i := !i + 1;
          offset := !offset + 1;
      end else begin
          i := !i + 1;
          offset := !offset + 4;
        end
  ) done;
  !i

let valid b =
  let len = Bytes.length b in
  let i = ref 0 in
  let res = ref true in
  while !i < len && !res = true do begin
    let c0 = int_of_char (Bytes.unsafe_get b !i) in
    if c0 < 128 then begin i := !i + 1; end
    else
    let ci = categories.(c0) in
    if ci = 0 then begin res := false; i := !i + 1; end
    else
      let cat = categoryTable.(ci) in
      if !i + cat.size > len then begin res := false; i := !i + 1; end
      else
        if cat.size == 2 then begin
        let c1 = int_of_char (Bytes.unsafe_get b (!i +1)) in
        if c1 < cat.low || cat.high < c1 then begin
          res := false;
          i := !i + 1;
        end
        else begin
          i := !i + cat.size;
        end
        end else if cat.size == 3 then begin
      let c1 = int_of_char (Bytes.unsafe_get b (!i +1)) in
      let c2 = int_of_char (Bytes.unsafe_get b (!i +2)) in
      if c1 < cat.low || cat.high < c1 || c2 < locb || hicb < c2 then begin
        res := false;
        i := !i + 1;
      end
      else
        i := !i + cat.size
      end else begin
      let c1 = int_of_char (Bytes.unsafe_get b (!i +1)) in
      let c2 = int_of_char (Bytes.unsafe_get b (!i +2)) in
      let c3 = int_of_char (Bytes.unsafe_get b (!i +3)) in

      if c1 < cat.low || cat.high < c1 ||
         c2 < locb || hicb < c2 || c3 < locb || hicb < c3
      then begin res := false; i := !i + 1; end
        else
          i := !i + cat.size
        end
  end done;
  !res

let validString s =
  let len = String.length s in
  let i = ref 0 in
  let res = ref true in
  while !i < len && !res = true do begin
    let c0 = int_of_char (String.unsafe_get s !i) in
    if c0 < 128 then begin i := !i + 1; end
    else
    let ci = categories.(c0) in
    if ci = 0 then begin res := false; i := !i + 1; end
    else
      let cat = categoryTable.(ci) in
      if !i + cat.size > len then begin res := false; i := !i + 1; end
      else if cat.size == 2 then begin
        let c1 = int_of_char (String.unsafe_get s (!i +1)) in
        if c1 < cat.low || cat.high < c1 then begin
          res := false;
          i := !i + 1;
        end
        else begin
          i := !i + cat.size;
        end
      end
        else if cat.size == 3 then begin
      let c1 = int_of_char (String.unsafe_get s (!i +1)) in
      let c2 = int_of_char (String.unsafe_get s (!i +2)) in
      if c1 < cat.low || cat.high < c1 || c2 < locb || hicb < c2 then begin
        res := false;
        i := !i + 1;
      end
      else
        i := !i + cat.size
  end else begin
      let c1 = int_of_char (String.unsafe_get s (!i +1)) in
      let c2 = int_of_char (String.unsafe_get s (!i +2)) in
      let c3 = int_of_char (String.unsafe_get s (!i +3)) in

      if c1 < cat.low || cat.high < c1 ||
         c2 < locb || hicb < c2 || c3 < locb || hicb < c3
      then begin res := false; i := !i + 1; end
        else
          i := !i + cat.size
        end
  end done;
  !res
