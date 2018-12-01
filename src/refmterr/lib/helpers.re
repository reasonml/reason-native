/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
let indent = (prefixStr, lines) => List.map(s => prefixStr ++ s, lines);

let splitOnChar = (sep, s) => {
  let j = ref(String.length(s));
  let r = ref([]);
  for (i in String.length(s) - 1 downto 0) {
    if (String.unsafe_get(s, i) == sep) {
      r.contents = [String.sub(s, i + 1, j.contents - i - 1), ...r.contents];
      j.contents = i;
    };
  };
  let tl = r.contents;
  [String.sub(s, 0, j.contents), ...tl];
};

let split = (sep, str) => {
  let rex = Re.Pcre.regexp(sep);
  Re.Pcre.split(~rex, str);
};

let splitLeadingWhiteSpace = s => {
  let index = {contents: 0};
  let firstNonWhite = {contents: (-1)}; /* -1 means does not exist. */
  let len = String.length(s);
  while (index.contents < len) {
    let char = s.[index.contents];
    if (firstNonWhite.contents === (-1)) {
      if (char !== ' ' && char !== '\n' && char !== '\t' && char !== '\r') {
        firstNonWhite.contents = index.contents;
      };
    };
    index.contents = index.contents + 1;
  };
  if (firstNonWhite.contents === (-1)) {
    (s, "");
  } else {
    (
      String.sub(s, 0, firstNonWhite.contents),
      String.sub(s, firstNonWhite.contents, len - firstNonWhite.contents),
    );
  };
};

let doubleUnder = Re.Pcre.regexp({|__|});

let subDot = _ => ".";

let moreThanOneSpace = Re.Pcre.regexp({|\s[\s]*|});

let subOneSpace = _ => " ";

/*
 * Collapses multiple spaces into a single space.
 */
let collapseSpacing = s =>
  Re.Pcre.substitute(~rex=moreThanOneSpace, ~subst=subOneSpace, s);

/*
 * Replaces the common module alias names with their conceptual counterparts
 * (double underscores become dot).
 */
let removeModuleAlias = s =>
  Re.Pcre.substitute(~rex=doubleUnder, ~subst=subDot, s);

let indentStr = (prefixStr, s) =>
  if (prefixStr == "") {
    s;
  } else {
    splitOnChar('\n', s)
    |> List.map(s => prefixStr ++ s)
    |> String.concat("\n");
  };

/* Batteries library substitutes */
let listDrop = (n, lst) => {
  let lst = ref(lst);
  for (_ in 1 to n) {
    lst := List.tl(lst^);
  };
  lst^;
};

let listDropWhile = (f, lst) => {
  let lst = ref(lst);
  while (f(List.hd(lst^))) {
    lst := List.tl(lst^);
  };
  lst^;
};

let listTake = (n, lst) => {
  let result = ref([]);
  let lst = ref(lst);
  for (_ in 1 to n) {
    result := [List.hd(lst^), ...result^];
    lst := List.tl(lst^);
  };
  List.rev(result^);
};

let listTakeWhile = (f, lst) => {
  let result = ref([]);
  let lst = ref(lst);
  while (f(List.hd(lst^))) {
    result := [List.hd(lst^), ...result^];
    lst := List.tl(lst^);
  };
  List.rev(result^);
};

let optionGet = a =>
  switch (a) {
  | Some(n) => n
  | None => raise(Invalid_argument("optionGet"))
  };

let optionMap = (f, a) =>
  switch (a) {
  | Some(a') => Some(f(a'))
  | None => None
  };

let listFilterMap = (f, lst) =>
  List.map(f, lst)
  |> List.filter(
       fun
       | Some(_) => true
       | None => false,
     )
  |> List.map(optionGet);

let listFindMap = (f, lst) =>
  lst
  |> List.find(a =>
       switch (f(a)) {
       | Some(_) => true
       | None => false
       }
     )
  |> f
  |> optionGet;

let stringSlice = (~first=0, ~last=?, str) => {
  let last =
    switch (last) {
    | Some(l) => min(l, String.length(str))
    | None => String.length(str)
    };
  if (last <= first) {
    "";
  } else {
    String.sub(str, first, last - first);
  };
};

let stringFind = (str, part) => {
  let rec find' = (str, part, idx) =>
    if (String.length(str) < String.length(part)) {
      raise(Not_found);
    } else if (stringSlice(str, ~last=String.length(part)) == part) {
      idx;
    } else {
      find'(stringSlice(str, ~first=1), part, idx + 1);
    };
  find'(str, part, 0);
};

let stringNsplit = (str, ~by) =>
  if (String.length(str) == 0) {
    raise(Invalid_argument("stringNSplit: empty str not allowed"));
  } else if (str == "") {
    [];
  } else {
    let rec split' = (str, ~by, accum, curr) => {
      let lengthBy = String.length(by);
      let lengthStr = String.length(str);
      if (lengthStr < lengthBy) {
        [curr ++ str, ...accum];
      } else if (String.sub(str, 0, lengthBy) == by) {
        split'(
          String.sub(str, lengthBy, lengthStr - lengthBy),
          ~by,
          [curr, ...accum],
          "",
        );
      } else {
        split'(
          String.sub(str, 1, lengthStr - 1),
          ~by,
          accum,
          curr ++ String.sub(str, 0, 1),
        );
      };
    };
    split'(str, ~by, [], "") |> List.rev;
  };

let stringSplit = (str, ~by) =>
  if (by == "") {
    ("", str);
  } else if (str == "") {
    raise(Not_found);
  } else {
    switch (stringNsplit(str, ~by)) {
    | []
    | [_] => raise(Not_found)
    | [x, ...xs] => (x, String.concat(by, xs))
    };
  };

let linesOfChannelExn = chan => {
  let lines = ref([]);
  try (
    {
      while (true) {
        lines := [input_line(chan), ...lines^];
      };
      lines^;
    }
  ) {
  | End_of_file =>
    close_in(chan);
    List.rev(lines^);
  };
};

let fileLinesOfExn = filePath => linesOfChannelExn(open_in(filePath));

/* ============ */
let get_match_n = (n, pat, str) => {
  let rex = Re.Pcre.regexp(pat);
  Re.Pcre.get_substring(Re.Pcre.exec(~rex, str), n);
};

/* get the first (presumably only) match in a string */
let get_match = get_match_n(1);

let get_match_maybe = (pat, str) => {
  let rex = Re.Pcre.regexp(pat);
  try (Some(Re.Pcre.get_substring(Re.Pcre.exec(~rex, str), 1))) {
  | Not_found => None
  };
};

let get_match_n_maybe = (n, pat, str) => {
  let rex = Re.Pcre.regexp(pat);
  try (Some(Re.Pcre.get_substring(Re.Pcre.exec(~rex, str), n))) {
  | _ => None
  };
};

let execMaybe = (pat, str) => {
  let rex = Re.Pcre.regexp(pat);
  try (Some(Re.Pcre.exec(~rex, str))) {
  | Not_found => None
  };
};

let getSubstringMaybe = (result, n) =>
  try (Some(Re.Pcre.get_substring(result, n))) {
  | Not_found => None
  };

let sub = (sep, cb, str) => {
  let rex = Re.Pcre.regexp(sep);
  Re.Pcre.substitute(~rex, ~subst=cb, str);
};

let rec splitInto = (~chunckSize, l: list('a)): list(list('a)) =>
  if (List.length(l) <= chunckSize || chunckSize == 0) {
    [l];
  } else {
    [
      listTake(chunckSize, l),
      ...splitInto(~chunckSize, listDrop(chunckSize, l)),
    ];
  };

let mapcat = (sep, f, l) => String.concat(sep, List.map(f, l));

let sp = Printf.sprintf;

/*
 * Returns whether or not the string has newlines. Only looks for \n so just
 * use this for non-critical things.
 */
let hasNewline = str =>
  switch (String.index_from(str, 0, '\n')) {
  | exception _ => false
  | _ => true
  };

let isWhiteChar = c => c === ' ' || c === '\n' || c === '\t' || c === '\r';

let isWordBoundary = c => c === '.' || c === ',' || c === '(' || c === ')';

/*
 * prevIndex is the index before the search start location. (or after if inc is
 * negative)
 */
let nextNonWhiteChar = (s, inc, prevIndex) => {
  let res = {contents: None};
  let i = {contents: prevIndex + inc};
  let len = String.length(s);
  while (res.contents === None && i.contents < len && i.contents > (-1)) {
    if (isWhiteChar(s.[i.contents])) {
      i.contents = i.contents + inc;
    } else {
      res.contents = Some(i.contents);
    };
  };
  res.contents;
};

/*
 * Returns the (aStartLen, aEndLen, bStartLen, bEndLen) of string a that
 * represents common suffix/prefix without regarding white space in
 * commonality.
 * Aligns to word boundaries.
 */
let findCommonEnds = (aStr, bStr) => {
  let aLen = String.length(aStr);
  let bLen = String.length(bStr);
  let aPrefixLen = {contents: 0};
  let aPrefixLenBoundary = {contents: 0};
  let bPrefixLen = {contents: 0};
  let bPrefixLenBoundary = {contents: 0};
  let aSuffixLen = {contents: 0};
  let aSuffixLenBoundary = {contents: 0};
  let bSuffixLen = {contents: 0};
  let bSuffixLenBoundary = {contents: 0};
  let continuePrefix = {contents: true};
  let continueSuffix = {contents: true};
  while (continuePrefix.contents
         && aPrefixLen.contents <= aLen
         && bPrefixLen.contents <= bLen) {
    let nextNonwhiteA = nextNonWhiteChar(aStr, 1, aPrefixLen.contents - 1);
    let nextNonwhiteB = nextNonWhiteChar(bStr, 1, bPrefixLen.contents - 1);
    switch (nextNonwhiteA, nextNonwhiteB) {
    | (None, None) =>
      aPrefixLenBoundary.contents = aPrefixLen.contents;
      bPrefixLenBoundary.contents = bPrefixLen.contents;
      continuePrefix.contents = false;
    | (None, Some(nb)) =>
      print_newline();
      /* If one was already at its word end */
      if (nb > bPrefixLen.contents) {
        aPrefixLenBoundary.contents = aPrefixLen.contents;
        bPrefixLenBoundary.contents = bPrefixLen.contents;
      };
      continuePrefix.contents = false;
    | (Some(na), None) =>
      /* If one was already at its word end */
      if (na > aPrefixLen.contents) {
        aPrefixLenBoundary.contents = aPrefixLen.contents;
        bPrefixLenBoundary.contents = bPrefixLen.contents;
      };
      continuePrefix.contents = false;
    | (Some(na), Some(nb)) =>
      /* Or if we didn't merely increment within a word, mark the end of
       * previous word as prefix  */
      if (na > aPrefixLen.contents + 1 && nb > bPrefixLen.contents + 1) {
        aPrefixLenBoundary.contents = aPrefixLen.contents;
        bPrefixLenBoundary.contents = bPrefixLen.contents;
      };
      if (aStr.[na] === bStr.[nb]) {
        if (isWordBoundary(aStr.[na])) {
          aPrefixLenBoundary.contents = na + 1;
          bPrefixLenBoundary.contents = nb + 1;
        };
        aPrefixLen.contents = na + 1;
        bPrefixLen.contents = nb + 1;
      } else {
        continuePrefix.contents = false;
      };
    };
  };
  while (continueSuffix.contents
         && aSuffixLen.contents < aLen
         - aPrefixLenBoundary.contents
         && bSuffixLen.contents < bLen
         - bPrefixLenBoundary.contents) {
    let nextNonwhiteA =
      nextNonWhiteChar(aStr, -1, aLen - aSuffixLen.contents);
    let nextNonwhiteB =
      nextNonWhiteChar(bStr, -1, bLen - bSuffixLen.contents);
    switch (nextNonwhiteA, nextNonwhiteB) {
    /* I think these first three cases would have been caught by the prefix
     * finding portion */
    | (None, None)
    | (None, Some(_))
    | (Some(_), None) => continueSuffix.contents = false
    | (Some(na), Some(nb)) =>
      /* Or if we didn't merely increment within a word, mark the end of
       * previous word as prefix  */
      if (na < aSuffixLen.contents - 1 && nb < bSuffixLen.contents - 1) {
        aSuffixLenBoundary.contents = aSuffixLen.contents;
        bSuffixLenBoundary.contents = bSuffixLen.contents;
      };
      if (aStr.[na] === bStr.[nb]) {
        if (isWordBoundary(aStr.[na])) {
          aSuffixLenBoundary.contents = aLen - na;
          bSuffixLenBoundary.contents = bLen - nb;
        };
        aSuffixLen.contents = aLen - na;
        bSuffixLen.contents = bLen - nb;
      } else {
        continueSuffix.contents = false;
      };
    };
  };
  (
    aPrefixLenBoundary.contents,
    aSuffixLenBoundary.contents,
    bPrefixLenBoundary.contents,
    bSuffixLenBoundary.contents,
  );
};

/* Safe pipe operator
   https://github.com/facebook/reason/issues/1928#issuecomment-389161245 */
let (|>?) = (x, access) =>
  switch (x) {
  | None => None
  | Some(v) => access(v)
  };

let (|>+) = (x, access) =>
  switch (x) {
  | None => None
  | Some(v) => Some(access(v))
  };
