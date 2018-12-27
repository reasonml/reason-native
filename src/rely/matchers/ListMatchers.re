/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
include CollectionMatchers.Make({
  type t('a) = list('a);
  let isEmpty = l => List.length(l) == 0;
  let rec collectionEquals =
          (
            ~memberEquals: ('a, 'a) => bool,
            listA: list('a),
            listB: list('a),
          )
          : bool =>
    switch (listA, listB) {
    | ([], []) => true
    | ([], _) => false
    | (_, []) => false
    | ([a, ...listAExtra], [b, ...listBExtra]) when memberEquals(a, b) =>
      collectionEquals(listAExtra, listBExtra, ~memberEquals)
    | _ => false
    };
  let emptyDisplay = "[]";
});
