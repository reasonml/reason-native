/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open Types;

/**
 * Redundantly coppied from Operations (avoids circular dep) (but avoids
 * overhead of path conversion - uses string).
 */
let rec readLink__ = str =>
  try (Fp.testForPathExn(Unix.readlink(str))) {
  /* Invalid argument - not a symlink */
  | Unix.Unix_error(Unix.EINVAL, _, _) =>
    raise(Invalid_argument("Path is not a symlink " ++ str))
  | Unix.Unix_error(Unix.EINTR, _, _) => readLink__(str)
  };

/*
 * Really, an error is the exceptional case here. Some/None indicates if the
 * object exists.
 * Does not follow links because that takes too much control out of our hands
 * to handle that in various circumstances.
 */
let rec stat = path =>
  try (Some(Unix.lstat(Fp.toString(path)))) {
  | Unix.Unix_error(Unix.ENOTDIR, _, _) => None
  | Unix.Unix_error(Unix.ENOENT, _, _) => None
  | Unix.Unix_error(Unix.EINTR, _, _) => stat(path)
  | Unix.Unix_error(_) as e => Util.reraise(e)
  };

let rec statExn = path => {
  let res = stat(path);
  switch (res) {
  | Some(info) => info
  | None =>
    raise(
      Invalid_argument(
        "Path passed to statExn that does not exist " ++ Fp.toString(path),
      ),
    )
  };
};

let makeResult = (path, fileStat: Unix.stats) =>
  switch (fileStat.st_kind) {
  | Unix.S_REG => File(path, fileStat)
  | Unix.S_DIR => Dir(path, fileStat)
  | Unix.S_LNK => Link(path, readLink__(Fp.toString(path)), fileStat)
  | Unix.S_CHR => Other(path, fileStat, CharacterDevice)
  | Unix.S_BLK => Other(path, fileStat, BlockDevice)
  | Unix.S_FIFO => Other(path, fileStat, NamedPipe)
  | Unix.S_SOCK => Other(path, fileStat, Socket)
  };

let query = p =>
  switch (stat(p)) {
  | None => None
  | Some(info) => Some(makeResult(p, info))
  };
let queryExn = p => makeResult(p, statExn(p));

let readDirByHandle = handle =>
  try (Some(Unix.readdir(handle))) {
  | End_of_file => None
  };

let rec readDir = path => {
  let impl = () => {
    let dirHandle = Unix.opendir(Fp.toString(path));
    /* Making this tail recursive so that if an exception is raised, it's
     * readable. */
    let rec scan = (acc, dirHandle) => {
      let fileName = readDirByHandle(dirHandle);
      switch (fileName) {
      | None => acc
      | Some("..")
      | Some(".") => scan(acc, dirHandle)
      | Some(fileName) =>
        let filePath = Fp.append(path, fileName);
        scan([filePath, ...acc], dirHandle);
      };
    };
    Util.withDirHandle(dirHandle, scan([]));
  };
  try (Ok(impl())) {
  | Unix.Unix_error(Unix.ENOENT, _, _) =>
    Error(
      Invalid_argument("Dir " ++ Fp.toString(path) ++ " does not exist"),
    )
  | Unix.Unix_error(Unix.EINTR, _, _) => readDir(path)
  | Unix.Unix_error(_, _, _) as e => Error(e)
  };
};

let readDirExn = path => Util.throwErrorResult(readDir(path));
