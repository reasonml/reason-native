/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

external reraise: exn => _ = "%reraise";

let throwErrorResult = v =>
  switch (v) {
  | Ok(value) => value
  | Error(e) => reraise(e)
  };

let withInChannel = (resource, doThis) => {
  let res =
    try (doThis(resource)) {
    | e =>
      try (
        {
          close_in(resource);
          reraise(e);
        }
      ) {
      | _ => reraise(e)
      }
    };
  close_in(resource);
  res;
};

let withOutChannel = (resource, doThis) => {
  let res =
    try (doThis(resource)) {
    | e =>
      try (
        {
          close_out(resource);
          reraise(e);
        }
      ) {
      | _ => reraise(e)
      }
    };
  close_out(resource);
  res;
};

let withDirHandle = (resource, doThis) => {
  let res =
    try (doThis(resource)) {
    | e =>
      try (
        {
          Unix.closedir(resource);
          reraise(e);
        }
      ) {
      | _ => reraise(e)
      }
    };
  Unix.closedir(resource);
  res;
};

/*
 * Some special care is required. Symlinks are read from `readlink` as if the
 * relative portions are from the perspective of the `from`'s parent directory.
 * So we apply an additional dirName here.
 */
let rec resolvePath = (from, toTarget) =>
  switch (toTarget) {
  | Fp.Absolute(a) => a
  | Relative(r) => Fp.join(Fp.dirName(from), r)
  };
