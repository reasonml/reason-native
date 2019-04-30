/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open TestFramework;

module PathMatchers = {
  open Rely.MatcherTypes;
  type absolutePathMatchers = {toEqualPath: 'kind. Path.t('kind) => unit};

  type absolutePathMatchersWithNot = {
    toEqualPath: 'kind. Path.t('kind) => unit,
    not: absolutePathMatchers,
  };

  let makeAbsolutePathMatchers =
      (accessorPath, absolutePath, {createMatcher}) => {
    let passMessageThunk = () => "";
    let createAbsolutePathMatcher = isNot =>
      createMatcher(
        (
          {matcherHint, formatReceived, formatExpected},
          actualThunk,
          expectedThunk,
        ) => {
        let actualAbsolutePath = actualThunk();
        let expectedAbsolutePath = expectedThunk();
        let pass =
          Path.eq(actualAbsolutePath, expectedAbsolutePath) == !isNot;
        if (!pass) {
          let failureMessage =
            String.concat(
              "",
              [
                matcherHint(
                  ~expectType=accessorPath,
                  ~matcherName=".toEqualPath",
                  ~isNot,
                  ~received="path",
                  ~expected="path",
                  (),
                ),
                "\n\n",
                "Expected path ",
                Path.toDebugString(actualAbsolutePath),
                " to ",
                isNot ? "not " : "",
                "equal path ",
                Path.toDebugString(expectedAbsolutePath),
              ],
            );
          (() => failureMessage, false);
        } else {
          (passMessageThunk, true);
        };
      });
    {
      not: {
        toEqualPath: otherPath =>
          createAbsolutePathMatcher(
            true,
            () => absolutePath,
            () => otherPath,
          ),
      },
      toEqualPath: otherPath =>
        createAbsolutePathMatcher(false, () => absolutePath, () => otherPath),
    };
  };
};
open PathMatchers;

type customMatchers = {
  path: 'kind. Path.t('kind) => PathMatchers.absolutePathMatchersWithNot,
};

let customMatchers = extendUtils => {
  path: path =>
    PathMatchers.makeAbsolutePathMatchers(".ext.path", path, extendUtils),
};

let describe = extendDescribe(customMatchers).describe;

describe("Path", ({test}) => {
  test("Basic creation", ({expect}) => {
    let path = Path.absoluteExn("/foo/bar/baz");
    expect.string(path |> Path.toString).toEqual("/foo/bar/baz");

    let path = Path.absoluteExn("C:/foo/bar/baz");
    expect.string(path |> Path.toString).toEqual("C:/foo/bar/baz");

    let path = Path.absoluteExn("/foo");
    expect.string(path |> Path.toString).toEqual("/foo");

    let path = Path.absoluteExn("C:/foo");
    expect.string(path |> Path.toString).toEqual("C:/foo");

    let path = Path.absoluteExn("/");
    expect.string(path |> Path.toString).toEqual("/");

    let path = Path.absoluteExn("C:/");
    expect.string(path |> Path.toString).toEqual("C:/");
  });

  test("Parent directory", ({expect}) => {
    let cDrive = Path.absoluteExn("C:/");
    let cFoo = Path.absoluteExn("C:/foo");
    let cFooBar = Path.absoluteExn("C:/foo/bar/");
    let root = Path.absoluteExn("/");
    let foo = Path.absoluteExn("/foo");
    let fooBar = Path.absoluteExn("/foo/bar/");
    expect.bool(cDrive |> Path.hasParentDir).toBeFalse();
    expect.bool(cFoo |> Path.hasParentDir).toBeTrue();
    expect.bool(cFooBar |> Path.hasParentDir).toBeTrue();
    expect.ext.path(Path.dirName(cFooBar)).toEqualPath(cFoo);
    expect.ext.path(Path.dirName(cFoo)).toEqualPath(cDrive);
    expect.equal(Path.baseName(cFooBar), Some("bar"));
    expect.equal(Path.baseName(cFoo), Some("foo"));
    expect.equal(Path.baseName(cDrive), None);

    expect.bool(root |> Path.hasParentDir).toBeFalse();
    expect.bool(foo |> Path.hasParentDir).toBeTrue();
    expect.bool(fooBar |> Path.hasParentDir).toBeTrue();
    expect.ext.path(Path.dirName(fooBar)).toEqualPath(foo);
    expect.ext.path(Path.dirName(foo)).toEqualPath(root);
    expect.equal(Path.baseName(cFooBar), Some("bar"));
    expect.equal(Path.baseName(cFoo), Some("foo"));
    expect.equal(Path.baseName(cDrive), None);
  });

  test("Weird characters", ({expect}) => {
    /**
     * We might actually want to rethink this behavior about spaces.
     * Currently, spaces don't need to be escaped in the convention and some
     * files might start with a space! Who would do that to their computer?
     */
    let path = Path.absoluteExn(" / ");
    expect.string(path |> Path.toString).toEqual("/");

    let path = Path.absoluteExn(" C:/ ");
    expect.string(path |> Path.toString).toEqual("C:/");

    let path = Path.absoluteExn("/C:/");
    expect.string(path |> Path.toString).toEqual("/C:");

    let path = Path.relativeExn(".../a");
    expect.string(path |> Path.toDebugString).toEqual("./.../a");

    let path = Path.relativeExn("../C:/");
    expect.string(path |> Path.toDebugString).toEqual("./../C:");

    let path = Path.absoluteExn("/a/C:/");
    expect.string(path |> Path.toString).toEqual("/a/C:");

    let path = Path.relativeExn("./a/C:/");
    expect.string(path |> Path.toDebugString).toEqual("./a/C:");

    let path = Path.relativeExn("./a/C:/../");
    expect.string(path |> Path.toDebugString).toEqual("./a");

    let path = Path.absoluteExn("/foo\\/");
    expect.string(path |> Path.toString).toEqual("/foo\\/");

    let path = Path.absoluteExn(" C:/\\/");
    expect.string(path |> Path.toString).toEqual("C:/\\/");
  });

  test("Relative/absolute inference", ({expect}) => {
    expect.fn(() => Path.absoluteExn("/../../")).not.toThrow();

    expect.fn(() => Path.relativeExn("./../../")).not.toThrow();

    expect.fn(() => Path.relativeExn("../../")).not.toThrow();

    expect.fn(() => Path.relativeExn("a/../../")).not.toThrow();

    expect.fn(() => Path.relativeExn("~/../../")).not.toThrow();

    expect.fn(() => Path.relativeExn("~/../../")).not.toThrow();

    expect.fn(() => Path.relativeExn("~/a/../../")).not.toThrow();

    expect.fn(() => Path.absoluteExn("C:/../")).not.toThrow();

    expect.fn(() => Path.absoluteExn("")).toThrow();

    expect.fn(() => Path.relativeExn("/foo")).toThrow();

    expect.fn(() => Path.relativeExn("C:/foo")).toThrow();

    expect.fn(() => Path.absoluteExn("a/b")).toThrow();
  });

  test("Path compression", ({expect}) => {
    let path = Path.relativeExn("a/../..//");
    expect.string(path |> Path.toDebugString).toEqual("./..");

    let path = Path.absoluteExn("C:/../..//");
    expect.string(path |> Path.toDebugString).toEqual("C:/");

    let path = Path.absoluteExn("C://");
    expect.string(path |> Path.toDebugString).toEqual("C:/");

    let path = Path.absoluteExn("//");
    expect.string(path |> Path.toDebugString).toEqual("/");

    let path = Path.absoluteExn("/../..//");
    expect.string(path |> Path.toDebugString).toEqual("/");

    let path = Path.relativeExn("a/../../");
    expect.string(path |> Path.toDebugString).toEqual("./..");

    let path = Path.relativeExn("~/a/../../");
    expect.string(path |> Path.toDebugString).toEqual("~/..");

    let path = Path.relativeExn("~/a/../");
    expect.string(path |> Path.toDebugString).toEqual("~/");

    let path = Path.relativeExn("~/../../");
    expect.string(path |> Path.toDebugString).toEqual("~/../..");

    let path = Path.absoluteExn("/../../");
    expect.string(path |> Path.toString).toEqual("/");

    /* Should not compress ../ for relatives beyond what is possible */
    let path = Path.relativeExn("./../../");
    expect.string(path |> Path.toDebugString).toEqual("./../..");

    let path = Path.relativeExn("../../");
    expect.string(path |> Path.toDebugString).toEqual("./../..");

    let path = Path.relativeExn("../../a/../");
    expect.string(path |> Path.toDebugString).toEqual("./../..");

    let path = Path.relativeExn("./~");
    expect.string(path |> Path.toDebugString).toEqual("./~");

    let path = Path.absoluteExn("/~");
    expect.string(path |> Path.toDebugString).toEqual("/~");

    let relativePath = Path.relativeExn("");
    expect.string(relativePath |> Path.toDebugString).toEqual("./");

    let relativePath = Path.relativeExn(".");
    expect.string(relativePath |> Path.toDebugString).toEqual("./");

    let relativePath = Path.relativeExn("./");
    expect.string(relativePath |> Path.toDebugString).toEqual("./");

    let relativePath = Path.relativeExn(".//");
    expect.string(relativePath |> Path.toDebugString).toEqual("./");
  });

  test("Path escaping", ({expect}) => {
    /**
     * We might actually want to rethink this behavior about spaces.
     * Currently, spaces don't need to be escaped in the convention and some
     * files might start with a space! Who would do that to their computer?
     */
    let path = Path.absoluteExn(" C:/ ");
    expect.string(path |> Path.toString).toEqual("C:/");

    /**
     * To make a truly universal usable path format, we should accept both
     * forward slashes, and backslashes in paths - *except* when a path has a
     * backslash followed immediately by a forward slash because that can only
     * mean an escaped forward slash. Similarly, a backslash followed
     * immediately by a backslash should also be considered a path separator.
     */
    let path = Path.absoluteExn("/foo\\/");
    expect.string(path |> Path.toString).toEqual("/foo\\/");

    let path = Path.absoluteExn(" C:/\\/");
    expect.string(path |> Path.toString).toEqual("C:/\\/");

    let path = Path.relativeExn("./\\./foo");
    expect.string(path |> Path.toDebugString).toEqual("./\\./foo");

    let path = Path.relativeExn(".\\./foo");
    expect.string(path |> Path.toDebugString).toEqual("./.\\./foo");

    let path = Path.relativeExn("./.\\./foo");
    expect.string(path |> Path.toDebugString).toEqual("./.\\./foo");

    let path = Path.absoluteExn("C:/./\\./foo");
    expect.string(path |> Path.toString).toEqual("C:/\\./foo");

    let path = Path.absoluteExn("C:/\\/foo");
    expect.string(path |> Path.toString).toEqual("C:/\\/foo");

    let path = Path.absoluteExn("/\\/foo");
    expect.string(path |> Path.toString).toEqual("/\\/foo");
  });

  test("Syntax", ({expect}) => {
    let root = Path.root;
    let drive = Path.drive("C:");
    let relRoot = Path.relativeExn("a");
    let path = Path.At.(relRoot / "b" /../ "");
    expect.string(path |> Path.toDebugString).toEqual("./a");

    let path = Path.At.(relRoot / "b" /../../ "");
    expect.string(path |> Path.toDebugString).toEqual("./");

    let path = Path.At.(drive / "a" / "b" /../ "");
    expect.string(path |> Path.toDebugString).toEqual("C:/a");

    let path = Path.At.(drive / "a" / "b" /../../ "");
    expect.string(path |> Path.toDebugString).toEqual("C:/");

    let path = Path.At.(root / "a" / "b" /../ "");
    expect.string(path |> Path.toDebugString).toEqual("/a");

    let path = Path.At.(root / "a" / "b" /../../ "");
    expect.string(path |> Path.toDebugString).toEqual("/");
  });

  test("Relativization throws for mismatched drives", ({expect}) => {
    open Path;
    let driveC = Path.drive("C:");
    expect.fn(() =>
      relativizeExn(
        ~source=At.(driveC / "a" / "b" / "qqq"),
        ~dest=At.(root / "f" / "f" / "zzz"),
      )
    ).
      toThrow();
    expect.fn(() =>
      relativizeExn(~source=At.(driveC / "a"), ~dest=At.(root / "a"))
    ).
      toThrow();
    expect.fn(() =>
      relativizeExn(
        ~source=At.(root / "a" / "b" / "qqq"),
        ~dest=At.(driveC / "f" / "f" / "zzz"),
      )
    ).
      toThrow();
    expect.fn(() =>
      relativizeExn(
        ~source=At.(dot / "a" / "b" / "qqq"),
        ~dest=At.(home / "f" / "f" / "zzz"),
      )
    ).
      toThrow();
  });

  test("Relativization throws for impossible relativization", ({expect}) => {
    open Path;
    expect.fn(() =>
      relativizeExn(~source=At.(dot / ".." / ""), ~dest=At.(dot / "a"))
    ).
      toThrow();
    expect.fn(() =>
      relativizeExn(
        ~source=At.(dot / "a" / ".." / ".." / "b"),
        ~dest=At.(dot / "a" / ".."),
      )
    ).
      toThrow();
    expect.fn(() => relativizeExn(~source=At.(dot / ".." / ""), ~dest=dot)).
      toThrow();
  });

  test("Relativization correctly for absolute paths", ({expect}) => {
    open Path;
    let root = Path.root;

    /* let rel = */
    /*   Path.(relativizeExn(~source=At.(root / "a"), ~dest=At.(root / "a"))); */
    /* expect.bool(Path.eq(Path.dot, rel)).toBeTrue(); */

    let rel =
      relativizeExn(
        ~source=At.(root / "a" / "b" / "qqq"),
        ~dest=At.(root / "f" / "f" / "zzz"),
      );
    let expected = relativeExn("../../../f/f/zzz");

    expect.ext.path(rel).toEqualPath(expected);
    /* Equality test in the other direction. */
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(
        ~source=At.(root / "a" / "b" / "qqq"),
        ~dest=At.(root / "a" / "b" / "zzz"),
      );
    let expected = relativeExn("../zzz");

    expect.ext.path(rel).toEqualPath(expected);
    /* Equality test in the other direction. */
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(
        ~source=At.(root / "a" / "b" / "c" / "d"),
        ~dest=At.(root / "a" / "b" / "qqq"),
      );
    let expected = relativeExn("../../qqq");

    expect.ext.path(rel).toEqualPath(expected);
    /* Equality test in the other direction. */
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(
        ~source=At.(root / "a" / "b" / "c" / "d"),
        ~dest=At.(root / "a" / "b" / "c" / "d" / "q"),
      );
    let expected = relativeExn("./q");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(
        ~source=At.(root / "x" / "y" / "z"),
        ~dest=At.(root / "a" / "b" / "c"),
      );
    let expected = relativeExn("../../../a/b/c");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);
  });

  test("Relativization correctly for absolute drives", ({expect}) => {
    open Path;
    let driveC = Path.drive("C:");

    let rel =
      relativizeExn(
        ~source=At.(driveC / "a" / "b" / "qqq"),
        ~dest=At.(driveC / "f" / "f" / "zzz"),
      );
    let expected = relativeExn("../../../f/f/zzz");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(
        ~source=At.(driveC / "a" / "b" / "qqq"),
        ~dest=At.(driveC / "a" / "b" / "zzz"),
      );
    let expected = relativeExn("../zzz");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(
        ~source=At.(driveC / "a" / "b" / "c" / "d"),
        ~dest=At.(driveC / "a" / "b" / "qqq"),
      );
    let expected = relativeExn("../../qqq");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(
        ~source=At.(driveC / "a" / "b" / "c" / "d"),
        ~dest=At.(driveC / "a" / "b" / "c" / "d" / "q"),
      );
    let expected = relativeExn("./q");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(
        ~source=At.(driveC / "x" / "y" / "z"),
        ~dest=At.(driveC / "a" / "b" / "c"),
      );
    let expected = relativeExn("../../../a/b/c");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);
  });

  test("Relativization correctly for relative paths", ({expect}) => {
    open Path;

    let rel =
      relativizeExn(~source=At.(dot / "a" / "b"), ~dest=At.(dot / "a"));
    expect.ext.path(At.(dot /../ "")).toEqualPath(rel);
    expect.ext.path(At.(rel)).toEqualPath(At.(dot /../ ""));

    let rel =
      relativizeExn(~source=At.(dot / "a" / "b"), ~dest=At.(dot / "a" / "b"));
    expect.ext.path(At.(dot)).toEqualPath(rel);
    expect.ext.path(At.(rel)).toEqualPath(dot);

    let rel =
      relativizeExn(
        ~source=At.(dot / "a" / "b" / "qqq"),
        ~dest=At.(dot / "f" / "f" / "zzz"),
      );
    let expected = relativeExn("../../../f/f/zzz");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(
        ~source=At.(dot / "a" / "b" / "qqq"),
        ~dest=At.(dot / "a" / "b" / "zzz"),
      );
    let expected = relativeExn("../zzz");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(
        ~source=At.(dot / "a" / "b" / "c" / "d"),
        ~dest=At.(dot / "a" / "b" / "qqq"),
      );
    let expected = relativeExn("../../qqq");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(
        ~source=At.(dot / "a" / "b" / "c" / "d"),
        ~dest=At.(dot / "a" / "b" / "c" / "d" / "q"),
      );
    let expected = relativeExn("./q");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(
        ~source=At.(dot / "x" / "y" / "z"),
        ~dest=At.(dot / "a" / "b" / "c"),
      );
    let expected = relativeExn("../../../a/b/c");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(
        ~source=At.(dot /../ "a" / "b" / "qqq"),
        ~dest=At.(dot /../ "a" / "b" / "qqq"),
      );
    let expected = relativeExn("./");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);

    let rel = relativizeExn(~source=At.(dot /../ ""), ~dest=At.(dot /../ ""));
    let expected = relativeExn("./");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(~source=At.(dot /../ ""), ~dest=At.(dot /../../ ""));
    let expected = relativeExn("../");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(~source=At.(dot /../ "a"), ~dest=At.(dot /../../ "a"));
    let expected = relativeExn("../../a");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);
  });

  test("Relativization correctly for home relative paths", ({expect}) => {
    open Path;

    let rel =
      relativizeExn(~source=At.(home / "a" / "b"), ~dest=At.(home / "a"));
    expect.ext.path(At.(dot /../ "")).toEqualPath(rel);
    expect.ext.path(At.(rel)).toEqualPath(At.(dot /../ ""));

    let rel =
      relativizeExn(
        ~source=At.(home / "a" / "b"),
        ~dest=At.(home / "a" / "b"),
      );
    expect.ext.path(At.(dot)).toEqualPath(rel);
    expect.ext.path(At.(rel)).toEqualPath(dot);

    let rel =
      relativizeExn(
        ~source=At.(home / "a" / "b" / "qqq"),
        ~dest=At.(home / "f" / "f" / "zzz"),
      );
    let expected = relativeExn("../../../f/f/zzz");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(
        ~source=At.(home / "a" / "b" / "qqq"),
        ~dest=At.(home / "a" / "b" / "zzz"),
      );
    let expected = relativeExn("../zzz");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(
        ~source=At.(home / "a" / "b" / "c" / "d"),
        ~dest=At.(home / "a" / "b" / "qqq"),
      );
    let expected = relativeExn("../../qqq");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(
        ~source=At.(home / "a" / "b" / "c" / "d"),
        ~dest=At.(home / "a" / "b" / "c" / "d" / "q"),
      );
    let expected = relativeExn("./q");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);

    let rel =
      relativizeExn(
        ~source=At.(home / "x" / "y" / "z"),
        ~dest=At.(home / "a" / "b" / "c"),
      );
    let expected = relativeExn("../../../a/b/c");

    expect.ext.path(rel).toEqualPath(expected);
    expect.ext.path(expected).toEqualPath(rel);
  });

  test("Containment for absolute paths", ({expect}) => {
    open Path;
    let root = Path.root;

    let res =
      isDescendent(
        ~ofPath=At.(root / "a" / "b" / "qqq"),
        At.(root / "f" / "f" / "zzz"),
      );
    expect.bool(res).toBeFalse();

    let res =
      isDescendent(
        ~ofPath=At.(root / "a" / "b" / "qqq"),
        At.(root / "a" / "b" / "zzz"),
      );
    expect.bool(res).toBeFalse();

    let res =
      isDescendent(
        ~ofPath=At.(root / "a" / "b" / "c" / "d"),
        At.(root / "a" / "b" / "qqq"),
      );
    expect.bool(res).toBeFalse();

    let res =
      isDescendent(
        ~ofPath=At.(root / "a" / "b" / "c" / "d"),
        At.(root / "a" / "b" / "c" / "d" / "q"),
      );
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(root), At.(root));
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(root / "x"), At.(root / "x"));
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(root / "x"), At.(root / "x" / "y"));
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(root / "x" / "y"), At.(root / "x"));
    expect.bool(res).toBeFalse();

    let res = isDescendent(~ofPath=At.(root / "x" / "y"), At.(root));
    expect.bool(res).toBeFalse();

    let res = isDescendent(~ofPath=At.(root), At.(root / "x" / "y"));
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(Path.drive("C")), At.(root / "x" / "y"));
    expect.bool(res).toBeFalse();
  });

  test("Containment for absolute drives", ({expect}) => {
    open Path;
    let drive = Path.drive("C:");

    let res =
      isDescendent(
        ~ofPath=At.(drive / "a" / "b" / "qqq"),
        At.(drive / "f" / "f" / "zzz"),
      );
    expect.bool(res).toBeFalse();

    let res =
      isDescendent(
        ~ofPath=At.(drive / "a" / "b" / "qqq"),
        At.(drive / "a" / "b" / "zzz"),
      );
    expect.bool(res).toBeFalse();

    let res =
      isDescendent(
        ~ofPath=At.(drive / "a" / "b" / "c" / "d"),
        At.(drive / "a" / "b" / "qqq"),
      );
    expect.bool(res).toBeFalse();

    let res =
      isDescendent(
        ~ofPath=At.(drive / "a" / "b" / "c" / "d"),
        At.(drive / "a" / "b" / "c" / "d" / "q"),
      );
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(drive), At.(drive));
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(drive / "x"), At.(drive / "x"));
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(drive / "x"), At.(drive / "x" / "y"));
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(drive / "x" / "y"), At.(drive / "x"));
    expect.bool(res).toBeFalse();

    let res = isDescendent(~ofPath=At.(drive / "x" / "y"), At.(drive));
    expect.bool(res).toBeFalse();

    let res = isDescendent(~ofPath=At.(drive), At.(drive / "x" / "y"));
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(Path.root), At.(drive / "x" / "y"));
    expect.bool(res).toBeFalse();
  });

  test("Containment for relative paths", ({expect}) => {
    open Path;
    let dot = Path.dot;

    let res =
      isDescendent(
        ~ofPath=At.(dot / "a" / "b" / "qqq"),
        At.(dot / "f" / "f" / "zzz"),
      );
    expect.bool(res).toBeFalse();

    let res =
      isDescendent(
        ~ofPath=At.(dot / "a" / "b" / "qqq"),
        At.(dot / "a" / "b" / "zzz"),
      );
    expect.bool(res).toBeFalse();

    let res =
      isDescendent(
        ~ofPath=At.(dot / "a" / "b" / "c" / "d"),
        At.(dot / "a" / "b" / "qqq"),
      );
    expect.bool(res).toBeFalse();

    let res =
      isDescendent(
        ~ofPath=At.(dot / "a" / "b" / "c" / "d"),
        At.(dot / "a" / "b" / "c" / "d" / "q"),
      );
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(dot), At.(dot));
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(dot / "x"), At.(dot / "x"));
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(dot / "x"), At.(dot / "x" / "y"));
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(dot / "x" / "y"), At.(dot / "x"));
    expect.bool(res).toBeFalse();

    let res = isDescendent(~ofPath=At.(dot / "x" / "y"), At.(dot));
    expect.bool(res).toBeFalse();

    let res = isDescendent(~ofPath=At.(dot), At.(dot / "x" / "y"));
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(Path.home), At.(dot / "x" / "y"));
    expect.bool(res).toBeFalse();
  });

  test("Containment for home relative paths", ({expect}) => {
    open Path;
    let home = Path.home;

    let res =
      isDescendent(
        ~ofPath=At.(home / "a" / "b" / "qqq"),
        At.(home / "f" / "f" / "zzz"),
      );
    expect.bool(res).toBeFalse();

    let res =
      isDescendent(
        ~ofPath=At.(home / "a" / "b" / "qqq"),
        At.(home / "a" / "b" / "zzz"),
      );
    expect.bool(res).toBeFalse();

    let res =
      isDescendent(
        ~ofPath=At.(home / "a" / "b" / "c" / "d"),
        At.(home / "a" / "b" / "qqq"),
      );
    expect.bool(res).toBeFalse();

    let res =
      isDescendent(
        ~ofPath=At.(home / "a" / "b" / "c" / "d"),
        At.(home / "a" / "b" / "c" / "d" / "q"),
      );
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(home), At.(home));
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(home / "x"), At.(home / "x"));
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(home / "x"), At.(home / "x" / "y"));
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(home / "x" / "y"), At.(home / "x"));
    expect.bool(res).toBeFalse();

    let res = isDescendent(~ofPath=At.(home / "x" / "y"), At.(home));
    expect.bool(res).toBeFalse();

    let res = isDescendent(~ofPath=At.(home), At.(home / "x" / "y"));
    expect.bool(res).toBeTrue();

    let res = isDescendent(~ofPath=At.(Path.dot), At.(home / "x" / "y"));
    expect.bool(res).toBeFalse();
  });
});
