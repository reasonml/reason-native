/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 */;
open Types;

let compareBaseName = (p1, p2) =>
  switch (Path.baseName(p1), Path.baseName(p2)) {
  | (None, None) => 0
  | (Some(x), None) => 1
  | (None, Some(x)) => (-1)
  | (Some(p1), Some(p2)) => String.compare(p1, p2)
  };

/*
 * Generalizes walking the file system, and allowing the callback `~onNode` to
 * perform an operation at each query result, and continuing if desired.
 * This allows preorder operations or postorder operations by supplying
 * `~onNode` callbacks of the following forms:
 *
 * let onNode = (queryResult, cont) => {
 *   callMyPreorderOperation();
 *   cont();
 * };
 *
 * let onNode = (queryResult, cont) => {
 *   cont();
 *   callMyPostorderOperation();
 * };
 *
 * TODO: Minimize number of file descriptors (reuse some?)
 * TODO: Statically ensure that only directories, and not symlinks are supplied.
 * TODO: Don't traverse symlinks by default.
 */
let rec traverseFileSystemFromQueryResult = (~onNode, queryResult) =>
  try (
    onNode(queryResult, () =>
      switch (queryResult) {
      | Other(_)
      | File(_) => ()
      | Dir(path, _) =>
        let paths = List.fast_sort(compareBaseName, Query.readDirExn(path));
        List.iter(traverseFileSystemFromPath(~onNode), paths);
      | Link(from, toTarget, _) =>
        let nextAbsPath = Util.resolvePath(from, toTarget);
        traverseFileSystemFromPath(~onNode, nextAbsPath);
      }
    )
  ) {
  /* Some symlink probably was bad, or a file was moved since the directory
   * scan was performed. TODO: At least log this. */
  | Unix.Unix_error(ENOENT, _, _) => ()
  }
and traverseFileSystemFromPath = (~onNode, path) =>
  traverseFileSystemFromQueryResult(~onNode, Query.queryExn(path));
