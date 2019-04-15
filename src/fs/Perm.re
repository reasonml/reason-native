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

let toInt = ({owner, group, other}) => {
  actionToInt(other) + 8 * actionToInt(group) + 64 * actionToInt(owner);
};

let defaultPerm = {
  owner: ReadWriteExecute,
  group: ReadExecute,
  other: ReadExecute,
};
