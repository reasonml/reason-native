/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 */;

/**
Number	Octal Permission Representation	Ref
4	Read permission	          r-- :
5	Read + execute permission r-x : 4 (read) + 1 (execute)
6	Read + write permission   rw- : 4 (read) + 2 (write)
7	All permissions           rwx : 4 (read) + 2 (write) + 1 (execute)
*/;

open Types;

let actionToInt = a =>
  switch (a) {
  | No => 0
  | Read => 4
  | ReadExecute => 5
  | ReadWrite => 6
  | ReadWriteExecute => 7
  };

let actionFromInt = i =>
  switch (i) {
  | 0 => No
  | 4 => Read
  | 5 => ReadExecute
  | 6 => ReadWrite
  | 7 => ReadWriteExecute
  | _ =>
    raise(
      Invalid_argument(
        "Invalid integer supplied for permissions. This is likely a bug in Fs.",
      ),
    )
  };

let toInt = ({owner, group, other}) => {
  actionToInt(other) + 8 * actionToInt(group) + 64 * actionToInt(owner);
};

let fromInt = i => {
  let owner = i / 64;
  let rest = i mod 64;
  let group = rest / 8;
  let rest = rest mod 8;
  let other = rest;
  {
    owner: actionFromInt(owner),
    group: actionFromInt(group),
    other: actionFromInt(other),
  };
};

let defaultPerm = {
  owner: ReadWriteExecute,
  group: ReadExecute,
  other: ReadExecute,
};
