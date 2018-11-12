/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 */
type t = ref(int);

let create = () => ref(0);

let startingAt = value => ref(value);

let next = counter => {
  let result = counter^;
  incr(counter);
  result;
};
