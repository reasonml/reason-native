/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

type fileStat = Unix.stats;

type lineEnds =
  | PlatformDefault
  | Windows
  | Unix;

type queryResultOther =
  | /** Character device */
    CharacterDevice
  | /** Block device */
    BlockDevice
  | /** Named pipe */
    NamedPipe
  | /** Socket */
    Socket;

type queryResult =
  | /** Regular file */
    File(Path.t(Path.absolute), fileStat)
  | /** Directory */
    Dir(Path.t(Path.absolute), fileStat)
  | /** Symbolic link */
    Link(
      Path.t(Path.absolute),
      Path.firstClass,
      fileStat,
    )
  | /** Other Operating System devices */
    Other(
      Path.t(Path.absolute),
      fileStat,
      queryResultOther,
    );

/* This type is made abstract */
type permissionCode = int;
type actions =
  | No
  | Read
  | ReadExecute
  | ReadWrite
  | ReadWriteExecute;

type perm = {
  owner: actions,
  group: actions,
  other: actions,
};
