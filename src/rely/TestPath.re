/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type describe =
  | Terminal(string)
  | Nested(string, describe);
type test = (string, describe);

type t =
  | Describe(describe)
  | Test(test);

let ancestrySeparator = " › ";
let ancestryRegex = Re.compile(Re.Pcre.re(ancestrySeparator));

let rec compare = (path1, path2) =>
  switch (path1, path2) {
  | (Test((name1, describe1)), Test((name2, describe2))) =>
    if (name1 == name2) {
      compare(Describe(describe1), Describe(describe2));
    } else {
      String.compare(name1, name2);
    }
  | (
      Describe(Nested(name1, describe1)),
      Describe(Nested(name2, describe2)),
    ) =>
    if (name1 == name2) {
      compare(Describe(describe1), Describe(describe2));
    } else {
      String.compare(name1, name2);
    }
  | (Describe(Terminal(string1)), Describe(Terminal(string2))) =>
    String.compare(string1, string2)
  | (Describe(Terminal(_)), Describe(Nested(_, _))) => (-1)
  | (Describe(Nested(_, _)), Describe(Terminal(_))) => 1
  | (Test(_), Describe(_)) => (-1)
  | (Describe(_), Test(_)) => 1
  };

let rec toString = path =>
  switch (path) {
  | Describe(Terminal(name)) => name
  | Describe(Nested(name, describe))
  | Test((name, describe)) =>
    String.concat(ancestrySeparator, [toString(Describe(describe)), name])
  };

let describeToString = d => Describe(d) |> toString;
let testToString = t => Test(t) |> toString;

let hash = (path, attempt) => {
  let testTitle = toString(path);
  let testHash =
    String.sub(
      Common.Strs.Crypto.md5(
        Re.replace_string(ancestryRegex, ~by="", testTitle)
        ++ string_of_int(attempt),
      ),
      0,
      8,
    );
  testHash;
};
