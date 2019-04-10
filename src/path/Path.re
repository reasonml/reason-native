/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 *
 * @emails oncall+ads_front_end_infra
 */;

let uriSep = "/";
let homeChar = "~";

type absolute;
type relative;
type upDirs = int; /* int 0 implies ./ and 1 implies ../ etc */

/**
 * We might eventually want to allow extending this with many
 * reference points.
 */
type relFrom =
  | Home
  | Any;
type base('kind) =
  /* Optional drive name */
  | Abs(option(string)): base(absolute)
  | Rel(relFrom, upDirs): base(relative);
/**
 * Internal representation of paths. The list of strings represents all
 * subdirectoreis after the base (in reverse order - head of the list is the
 * rightmost segment of the path).
 */
type t('kind) = (base('kind), list(string));
type opaqueBase =
  | Base(base('exists)): opaqueBase;
type opaqueT = (opaqueBase, list(string));

let drive = name => (Abs(Some(name)), []);
let root = (Abs(None), []);
let home = (Rel(Home, 0), []);
let dot = (Rel(Any, 0), []);

let toString: type kind. t(kind) => string =
  path =>
    switch (path) {
    | (Abs(l), lst) =>
      let lbl =
        switch (l) {
        | None => ""
        | Some(txt) => txt
        };
      lbl ++ "/" ++ (lst |> List.rev |> String.concat(uriSep));
    | (Rel(w, i), lst) =>
      let init =
        switch (w) {
        | Any => "./"
        | Home => "~/"
        };
      let rest =
        lst
        |> List.rev
        |> List.append(Array.to_list(Array.init(i, i => "..")))
        |> String.concat(uriSep);
      init ++ rest;
    };

/**
 * Expose this under the name `toDebugString` and accept any kind of path.
 * The name is to warn people about using this for relative paths. This may
 * print paths like `"."` and `"~"`, which is not very meaningful.
 */
let toDebugString = toString;

type token =
  | SLASH
  | DOT
  | TILDE
  | DOTDOT
  | DRIVE(string)
  | TXT(string);

let makeToken = s =>
  switch (s) {
  | "~" => TILDE
  | "." => DOT
  | ".." => DOTDOT
  | s when String.length(s) >= 2 && s.[String.length(s) - 1] === ':' =>
    DRIVE(s)
  | s => TXT(s)
  };
/*
 * Splits on slashes, but being intelligent about escaped slashes.
 */
let lex = s => {
  let s = String.trim(s);
  let len = String.length(s);
  let r = ref([]);
  /* j is what you are all caught up to */
  let j = ref(-1);
  let prevEsc = {contents: false};
  for (i in 0 to len - 1) {
    let ch = String.unsafe_get(s, i);
    if (ch === '/' && !prevEsc.contents) {
      if (j.contents !== i - 1) {
        let tok =
          makeToken(String.sub(s, j.contents + 1, i - j.contents - 1));
        r.contents = [tok, ...r.contents];
      };
      r.contents = [SLASH, ...r.contents];
      j.contents = i;
    };
    prevEsc.contents = ch === '\\' && !prevEsc.contents;
  };
  let rev =
    j.contents === len - 1 ?
      r.contents :
      [
        makeToken(String.sub(s, j.contents + 1, len - 1 - j.contents)),
        ...r.contents,
      ];
  List.rev(rev);
};

let parseFirstToken = token =>
  switch (token) {
  | SLASH => (Base(Abs(None)), [])
  | DOT => (Base(Rel(Any, 0)), [])
  | TILDE => (Base(Rel(Home, 0)), [])
  | DOTDOT => (Base(Rel(Any, 1)), [])
  | DRIVE(l) => (Base(Abs(Some(l))), [])
  | TXT(s) => (Base(Rel(Any, 0)), [s])
  };

let rec parseNextToken: type kind. (t(kind), token) => t(kind) =
  (path, nextToken) =>
    switch (path, nextToken) {
    | (path, SLASH) => path
    | (path, DOT) => path
    | ((base, subs), TILDE) => (base, [homeChar, ...subs])
    | ((base, subs), DRIVE(l)) => (base, [l, ...subs])
    | ((base, subs), TXT(s)) => (base, [s, ...subs])
    | ((base, [hd, ...tl]), DOTDOT) => (base, tl)
    | ((Rel(Any, r), []), DOTDOT) => (Rel(Any, r + 1), [])
    | ((Rel(Home, r), []), DOTDOT) => (Rel(Home, r + 1), [])
    | ((Abs(_), []), DOTDOT) => path
    };

let parseFirstTokenAbsolute = token =>
  switch (token) {
  | SLASH => Some((Abs(None), []))
  | DRIVE(l) => Some((Abs(Some(l)), []))
  | TXT(_)
  | DOT
  | TILDE
  | DOTDOT => None
  };

let parseFirstTokenRelative = token =>
  switch (token) {
  | DOT => Some((Rel(Any, 0), []))
  | TILDE => Some((Rel(Home, 0), []))
  | DOTDOT => Some((Rel(Any, 1), []))
  | TXT(s) => Some((Rel(Any, 0), [s]))
  | SLASH => None
  | DRIVE(l) => None
  };

let absolute = s =>
  switch (lex(s)) {
  /* Cannot pass empty string for absolute path */
  | [] => None
  | [hd, ...tl] =>
    switch (parseFirstTokenAbsolute(hd)) {
    | None => None
    | Some(initAbsPath) =>
      Some(List.fold_left(parseNextToken, initAbsPath, tl))
    }
  };

let absoluteExn = s =>
  switch (lex(s)) {
  /* Cannot pass empty string for absolute path */
  | [] => raise(Invalid_argument("Empty path is not a valid absolute path."))
  | [hd, ...tl] =>
    switch (parseFirstTokenAbsolute(hd)) {
    | None =>
      raise(
        Invalid_argument("First token in path " ++ s ++ " is not absolute."),
      )
    | Some(initAbsPath) => List.fold_left(parseNextToken, initAbsPath, tl)
    }
  };

let relative = s => {
  let (tok, tl) =
    switch (lex(s)) {
    | [] => (DOT, [])
    | [hd, ...tl] => (hd, tl)
    };
  switch (parseFirstTokenRelative(tok)) {
  | None => None
  | Some(initRelPath) =>
    Some(List.fold_left(parseNextToken, initRelPath, tl))
  };
};

let relativeExn = s =>
  switch (lex(s)) {
  /* Cannot pass empty string for absolute path */
  | [] => dot
  | [hd, ...tl] =>
    switch (parseFirstTokenRelative(hd)) {
    | None =>
      raise(
        Invalid_argument("First token in path " ++ s ++ " not relative."),
      )
    | Some(initRelPath) => List.fold_left(parseNextToken, initRelPath, tl)
    }
  };

let continue = (s, path) => List.fold_left(parseNextToken, path, lex(s));

let rec join: type k1 k2. (t(k1), t(k2)) => t(k1) =
  (p1, p2) =>
    switch (p1, p2) {
    | ((Rel(w, r1), []), (Rel(Any, r2), s2)) => (Rel(w, r1 + r2), s2)
    | ((Rel(w, r1), [s1hd, ...s1tl] as s1), (Rel(Any, r2), s2)) =>
      r2 > 0 ?
        join((Rel(w, r1), s1tl), (Rel(Any, r2 - 1), s2)) :
        (Rel(w, r1), List.append(s2, s1))
    | ((b1, s1), (Rel(Home, r2), s2)) =>
      join((b1, [homeChar, ...List.append(s2, s1)]), (Rel(Any, r2), s2))
    | ((b1, s1), (Abs(Some(ll)), s2)) => (
        b1,
        [ll, ...List.append(s2, s1)],
      )
    | ((b1, s1), (Abs(None), s2)) => (b1, List.append(s2, s1))
    | ((Abs(_) as d, []), (Rel(Any, r2), s2)) => (d, s2)
    | ((Abs(_) as d, [s1hd, ...s1tl] as s1), (Rel(Any, r2), s2)) =>
      r2 > 0 ?
        join((d, s1tl), (Rel(Any, r2 - 1), s2)) :
        (d, List.append(s2, s1))
    };

let rec dirName: type k1. t(k1) => t(k1) =
  p1 =>
    switch (p1) {
    | (Rel(w, r1), []) => (Rel(w, r1 + 1), [])
    | (Rel(w, r1), [s1hd, ...s1tl]) => (Rel(w, r1), s1tl)
    | (Abs(_) as d, []) => (d, [])
    | (Abs(_) as d, [s1hd, ...s1tl]) => (d, s1tl)
    };

let rec baseName: type k1. t(k1) => option(string) =
  p1 =>
    switch (p1) {
    | (Rel(w, r1), []) => None
    | (Rel(w, r1), [s1hd, ...s1tl]) => Some(s1hd)
    | (Abs(_), []) => None
    | (Abs(_), [s1hd, ...s1tl]) => Some(s1hd)
    };

let sub: type k1. (string, t(k1)) => t(k1) =
  (name, path) => continue(name, path);

/**
 * Append functions always follow their "natural" left/right ordering,
 * regardless of t-first/last.
 *
 * The following pairs are equivalent but not taht `append` is always safe.
 *
 *     Path.append(Path.root, "foo");
 *     Option.getUnsafe(Path.absolute("/foo"));
 *
 *     Path.append(Path.root, "foo/bar");
 *     Option.getUnsafe(Path.absolute("/foo/bar"));
 *
 *     Path.append(Path.drive("C"), "foo/bar");
 *     Option.getUnsafe(Path.absolute("C:/foo/bar"));
 *
 *     Path.append(Path.dot, "foo");
 *     Option.getUnsafe(Path.relative("./foo"));
 */
let append: type k1. (t(k1), string) => t(k1) =
  (path, name) => continue(name, path);

module At = {
  let (/) = append;

  /**
   * Applies `dirName` to the first argument, then passes the result to
   * `append` with the second.
   *
   *     let result = root / "foo" / "bar" /../ "baz";
   *
   * Would result in
   *
   *     "/foo/baz"
   */
  let (/../) = (dir, s) => append(dirName(dir), s);
  let (/../../) = (dir, s) => append(dirName(dirName(dir)), s);
  let (/../../../) = (dir, s) =>
    append(dirName(dirName(dirName(dir))), s);
  let (/../../../../) = (dir, s) =>
    append(dirName(dirName(dirName(dirName(dir)))), s);
  let (/../../../../../) = (dir, s) =>
    append(dirName(dirName(dirName(dirName(dirName(dir))))), s);
};
