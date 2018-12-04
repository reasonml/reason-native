/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type t = ref(int);

let create = () => ref(0);

let startingAt = value => ref(value);

let next = counter => {
  let result = counter^;
  incr(counter);
  result;
};
