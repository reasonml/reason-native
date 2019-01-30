/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type t =
  /* An offset of the current time and 00:00:00 GMT, Jan. 1, 1970 in seconds*/
  | Seconds(float);

let subtract = (t1, t2) =>
  switch (t1, t2) {
  | (Seconds(s1), Seconds(s2)) => Seconds((-.)(s1, s2))
  };
