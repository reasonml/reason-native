/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 */;

open Types;

let mode = path => {
  switch (Query.query(path)) {
  | None => Error(Invalid_argument("No file for path"))
  | Some(File(_, st) | Dir(_, st) | Link(_, _, st) | Other(_, st, _)) =>
    Ok(Perm.fromInt(st.st_perm))
  };
};

let modeExn = path => Util.throwErrorResult(mode(path));

let rec changeMode = (perm, path) =>
  try(Ok(Unix.chmod(Fp.toString(path), Perm.toInt(perm)))) {
  | Unix.Unix_error(Unix.EINTR, _, _) => changeMode(perm, path)
  | Unix.Unix_error(e, _, _) =>
    Error(Invalid_argument("Cannot changeMode for " ++ Fp.toString(path)))
  };

let rec changeModeExn =
  (perm, path) => Util.throwErrorResult(changeMode(perm, path));

let rec readLink = path => {
  let stringPath = Fp.toString(path);
  try (Ok(Fp.testForPathExn(Unix.readlink(stringPath)))) {
  /* Invalid argument - not a symlink */
  | Unix.Unix_error(Unix.EINVAL, _, _) =>
    Error(Invalid_argument("Path is not a symlink " ++ stringPath))
  | Unix.Unix_error(Unix.EINTR, _, _) => readLink(path)
  | Unix.Unix_error(e, _, _) as err => Error(err)
  };
};

let rec readLinkExn = path => Util.throwErrorResult(readLink(path));

let rec foldKnownLink = (path, acc, f) => {
  let next = f(path, acc);
  switch (Query.query(path)) {
  | Some(Other(_))
  | Some(File(_))
  | Some(Dir(_))
  /* If we've reached a null pointer, we return the address */
  | None => next
  | Some(Link(from, toTarget, _)) =>
    foldKnownLink(Util.resolvePath(from, toTarget), next, f)
  };
};

/*
 * Utility, follows a *known* symlink.
 */
let followKnownLink = p => foldKnownLink(p, p, (nextPath, _) => nextPath);

let notALink = p => "followLink: Not a symlink " ++ Fp.toString(p);
let pathNotExist = p => "followLink: path doesn't exist" ++ Fp.toString(p);
let rec followLink = path =>
  switch (Query.query(path)) {
  | Some(Other(_))
  | Some(File(_))
  | Some(Dir(_)) => Error(Invalid_argument(notALink(path)))
  | None => Error(Invalid_argument(pathNotExist(path)))
  | Some(Link(from, toTarget, _)) =>
    Ok(followKnownLink(Util.resolvePath(from, toTarget)))
  };

let rec followLinkExn = path => Util.throwErrorResult(followLink(path));

let rec resolveLink = path =>
  switch (Query.query(path)) {
  | Some(Other(_))
  | Some(File(_))
  | Some(Dir(_)) => Ok(path)
  | None => Error(Invalid_argument(pathNotExist(path)))
  | Some(Link(from, toTarget, _)) =>
    Ok(followKnownLink(Util.resolvePath(from, toTarget)))
  };

let rec resolveLinkExn = path => Util.throwErrorResult(resolveLink(path));

let rec links = path =>
  switch (Query.query(path)) {
  | Some(Other(_))
  | Some(File(_))
  | Some(Dir(_)) => Ok([])
  | None => Error(Invalid_argument(pathNotExist(path)))
  | Some(Link(from, toTarget, _)) =>
    let startPath = Util.resolvePath(from, toTarget);
    Ok(List.rev(foldKnownLink(startPath, [], List.cons)));
  };

let rec linksExn = path => Util.throwErrorResult(links(path));

let rec rm = path =>
  try (Ok(Unix.unlink(Fp.toString(path)))) {
  | Unix.Unix_error(Unix.EINTR, _, _) => rm(path)
  /* Will raise Unix.Unix_error(Unix.ENOENT, _, _) if not exists */
  | Unix.Unix_error(_) as e => Error(e)
  };

let rec rmExn = path =>
  try (Unix.unlink(Fp.toString(path))) {
  | Unix.Unix_error(Unix.EINTR, _, _) => rmExn(path)
  /* Will raise Unix.Unix_error(Unix.ENOENT, _, _) if not exists */
  | Unix.Unix_error(_) as e => Util.reraise(e)
  };

let rec rmEmptyDir = path =>
  try (Ok(Unix.rmdir(Fp.toString(path)))) {
  | Unix.Unix_error(Unix.EINTR, _, _) => rmEmptyDir(path)
  /* Will raise Unix.Unix_error(Unix.ENOENT, _, _) if not exists */
  | Unix.Unix_error(_) as e => Error(e)
  };

let rec rmEmptyDirExn = path =>
  try (Unix.rmdir(Fp.toString(path))) {
  | Unix.Unix_error(Unix.EINTR, _, _) => rmEmptyDirExn(path)
  /* Will raise Unix.Unix_error(Unix.ENOENT, _, _) if not exists */
  | Unix.Unix_error(_) as e => Util.reraise(e)
  };

let rec rmIfExists = path =>
  try (Ok(Unix.unlink(Fp.toString(path)))) {
  | Unix.Unix_error(Unix.ENOENT, _, _) => Ok()
  | Unix.Unix_error(Unix.EINTR, _, _) => rmIfExists(path)
  | Unix.Unix_error(_) as e => Error(e)
  };

let rec rmIfExistsExn = path =>
  try (Unix.unlink(Fp.toString(path))) {
  | Unix.Unix_error(Unix.ENOENT, _, _) => ()
  | Unix.Unix_error(Unix.EINTR, _, _) => rmIfExistsExn(path)
  | Unix.Unix_error(_) as e => Util.reraise(e)
  };

let rec mkDir = (~perm=Perm.defaultPerm, path) =>
  try (Ok(Unix.mkdir(Fp.toString(path), Perm.toInt(perm)))) {
  | Unix.Unix_error(Unix.EEXIST, _, _) as e => Error(e)
  | Unix.Unix_error(Unix.EINTR, _, _) => mkDir(~perm, path)
  | Unix.Unix_error(_) as e => Error(e)
  };

let rec mkDirExn = (~perm=Perm.defaultPerm, path) =>
  try (Unix.mkdir(Fp.toString(path), Perm.toInt(perm))) {
  | Unix.Unix_error(Unix.EEXIST, _, _) as e => Util.reraise(e)
  | Unix.Unix_error(Unix.EINTR, _, _) => mkDirExn(~perm, path)
  | Unix.Unix_error(_) as e => Util.reraise(e)
  };

let onForceRemoveDir = (queryResult, cont) =>
  switch (queryResult) {
  | Other(path, _, _) =>
    rmExn(path);
    cont();
  | File(path, _) => rmExn(path)
  | Dir(path, _) =>
    cont();
    /* Post order */
    rmEmptyDirExn(path);
  /* For symbolic links, we don't want to continue because either:
   * 1. The link points to something inside the original removal root
   * in which case, we'll find it through some other path.
   * 2. The link points outside the original removal root and in that
   * case we definitely don't want to remove what it points to.
   */
  | Link(path, _, _) => rmExn(path)
  };

let cannotRemovePathNoExist = p =>
  "Cannot remove path that does not exist " ++ Fp.toDebugString(p);
let cannotRemovePathNotDir = p =>
  "Path is not a directory not exist " ++ Fp.toDebugString(p);
let rmDir = path => {
  assert(Fp.hasParentDir(path));
  switch (Query.query(path)) {
  | None => Error(Invalid_argument(cannotRemovePathNoExist(path)))
  | Some(Dir(p, info)) =>
    Ok(
      Traverse.traverseFileSystemFromQueryResult(
        ~onNode=onForceRemoveDir,
        Dir(p, info),
      ),
    )
  | Some(_) => Error(Invalid_argument(cannotRemovePathNotDir(path)))
  };
};

let rmDirExn = path => Util.throwErrorResult(rmDir(path));

/*
 * Make one directory for the sake of mkdir -p style behavior.
 */
let rec mkDirPOneExn = (~perm=Perm.defaultPerm, path) =>
  try (Unix.mkdir(Fp.toString(path), Perm.toInt(perm))) {
  | Unix.Unix_error(Unix.EEXIST, _, _) as e =>
    switch (Query.queryExn(path)) {
    | Link(_)
    | Other(_)
    | File(_) => Util.reraise(e)
    /* Maybe something else made it before we got the chance to? */
    | Dir(_) => ()
    }
  | Unix.Unix_error(Unix.EINTR, _, _) => mkDirPOneExn(~perm, path)
  | Unix.Unix_error(_) as e => Util.reraise(e)
  };

/*
 * TODO: Detect when the mkdirp will fail (something along the path is a
 * symlink etc).
 */
let noRoot = p =>
  "mkDirP: Why does it appear you have no root directory? "
  ++ Fp.toString(p);
let nonDir = p =>
  "mkDirP: The path "
  ++ Fp.toString(p)
  ++ " is a symlink that points to a non-directory";

let rec pathsToMake = (acc, p) =>
  switch (Query.query(p)) {
  | Some(Dir(_)) => Ok(acc)
  | Some(Link(from, toTarget, _)) =>
    switch (Query.query(followKnownLink(Util.resolvePath(from, toTarget)))) {
    /* Relaxes the error in this case - like mkdir -p does */
    | Some(Dir(_, _)) => Ok(acc)
    | None
    | Some(File(_))
    | Some(Other(_))
    /* This case should never happen - we already followed */
    | Some(Link(_)) => Error(Invalid_argument(nonDir(p)))
    }
  | None =>
    if (Fp.hasParentDir(p)) {
      pathsToMake([p, ...acc], Fp.dirName(p));
    } else {
      Error(Sys_error(noRoot(p)));
    }
  | Some(File(_))
  | Some(Other(_)) =>
    Error(Invalid_argument("Cannot mkdir at file " ++ Fp.toString(p)))
  };

let mkDirP = (~perm=Perm.defaultPerm, path) => {
  switch (pathsToMake([], path)) {
  | Ok(paths) => Ok(List.iter(mkDirPOneExn(~perm), paths))
  | Error(exn) as e => e
  };
};

let mkDirPExn = (~perm=Perm.defaultPerm, path) =>
  Util.throwErrorResult(mkDirP(~perm, path));

let rec linkImpl = (~isDir, ~from, ~toTarget) =>
  try (
    Ok(
      Unix.symlink(
        ~to_dir=isDir,
        Fp.toDebugString(toTarget),
        Fp.toString(from),
      ),
    )
  ) {
  | Unix.Unix_error(Unix.EEXIST, _, _) as e =>
    switch (Query.queryExn(from)) {
    | Link(_) =>
      rmExn(from);
      linkImpl(~isDir, ~from, ~toTarget);
    | Dir(_)
    | Other(_)
    | File(_) => Error(e)
    }
  | Unix.Unix_error(Unix.EINTR, _, _) => linkImpl(~isDir, ~from, ~toTarget)
  | Unix.Unix_error(_) as e => Error(e)
  };

let rec link = (~from, ~toTarget) =>
  linkImpl(~isDir=false, ~from, ~toTarget);

let rec linkExn = (~from, ~toTarget) =>
  Util.throwErrorResult(link(~from, ~toTarget));

let rec linkDir = (~from, ~toTarget) =>
  linkImpl(~isDir=true, ~from, ~toTarget);

let rec linkDirExn = (~from, ~toTarget) =>
  Util.throwErrorResult(linkDir(~from, ~toTarget));
