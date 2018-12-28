/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
module Array = {
  type t('a) = array('a);
  let isEmpty = l => Array.length(l) == 0;
  let collectionEquals =
      (
        ~memberEquals: ('a, 'a) => bool,
        arrayA: array('a),
        arrayB: array('a),
      )
      : bool => {
    let n1 = Array.length(arrayA);
    let n2 = Array.length(arrayB);
    if (n1 !== n2) {
      false;
    } else {
      let equal = ref(true);
      let i = ref(0);
      while (equal^ && i^ < n1) {
        if (!memberEquals(arrayA[i^], arrayB[i^])) {
          equal := false;
        };
        incr(i);
      };
      equal^;
    };
  };
  let emptyDisplay = "[||]";
  let contains = (target, equals) => Array.exists(item => equals(item, target));
  let collectionName = "array";
}

include CollectionMatchers.Make(Array);
