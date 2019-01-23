/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
module List = {
  type t('a) = list('a);
  let isEmpty = l =>
    switch (l) {
    | [] => true
    | _ => false
    };
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
  let contains = (target, equals) =>
    List.exists(item => equals(item, target));
  let collectionName = "list";
};

include CollectionMatchers.Make(List);
