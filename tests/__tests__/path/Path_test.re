/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open TestFramework;

module FpMatchers = {
  open Rely.MatcherTypes;
  type absolutePathMatchers = {toEqualPath: 'kind. Fp.t('kind) => unit};

  type absolutePathMatchersWithNot = {
    toEqualPath: 'kind. Fp.t('kind) => unit,
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
          Fp.eq(actualAbsolutePath, expectedAbsolutePath) == !isNot;
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
                Fp.toDebugString(actualAbsolutePath),
                " to ",
                isNot ? "not " : "",
                "equal path ",
                Fp.toDebugString(expectedAbsolutePath),
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
open FpMatchers;

type customMatchers = {
  path: 'kind. Fp.t('kind) => FpMatchers.absolutePathMatchersWithNot,
};

let customMatchers = extendUtils => {
  path: path =>
    FpMatchers.makeAbsolutePathMatchers(".ext.path", path, extendUtils),
};

let describe =
  (describeConfig |> withCustomMatchers(customMatchers) |> build).
    describe;

describe("Path", ({test}) => {
  test("Basic creation", ({expect}) => {
    let path = Fp.absoluteCurrentPlatformExn("/foo/bar/baz");
    expect.string(path |> Fp.toString).toEqual("/foo/bar/baz");

    let path = Fp.absoluteCurrentPlatformExn("C:/foo/bar/baz");
    expect.string(path |> Fp.toString).toEqual("C:/foo/bar/baz");

    let path = Fp.absoluteCurrentPlatformExn("/foo");
    expect.string(path |> Fp.toString).toEqual("/foo");

    let path = Fp.absoluteCurrentPlatformExn("C:/foo");
    expect.string(path |> Fp.toString).toEqual("C:/foo");

    let path = Fp.absoluteCurrentPlatformExn("/");
    expect.string(path |> Fp.toString).toEqual("/");

    let path = Fp.absoluteCurrentPlatformExn("C:/");
    expect.string(path |> Fp.toString).toEqual("C:/");
  });

  test("Parent directory", ({expect}) => {
    let cDrive = Fp.absoluteCurrentPlatformExn("C:/");
    let cFoo = Fp.absoluteCurrentPlatformExn("C:/foo");
    let cFooBar = Fp.absoluteCurrentPlatformExn("C:/foo/bar/");
    let root = Fp.absoluteCurrentPlatformExn("/");
    let foo = Fp.absoluteCurrentPlatformExn("/foo");
    let fooBar = Fp.absoluteCurrentPlatformExn("/foo/bar/");
    expect.bool(cDrive |> Fp.hasParentDir).toBeFalse();
    expect.bool(cFoo |> Fp.hasParentDir).toBeTrue();
    expect.bool(cFooBar |> Fp.hasParentDir).toBeTrue();
    expect.ext.path(Fp.dirName(cFooBar)).toEqualPath(cFoo);
    expect.ext.path(Fp.dirName(cFoo)).toEqualPath(cDrive);
    expect.equal(Fp.baseName(cFooBar), Some("bar"));
    expect.equal(Fp.baseName(cFoo), Some("foo"));
    expect.equal(Fp.baseName(cDrive), None);

    expect.bool(root |> Fp.hasParentDir).toBeFalse();
    expect.bool(foo |> Fp.hasParentDir).toBeTrue();
    expect.bool(fooBar |> Fp.hasParentDir).toBeTrue();
    expect.ext.path(Fp.dirName(fooBar)).toEqualPath(foo);
    expect.ext.path(Fp.dirName(foo)).toEqualPath(root);
    expect.equal(Fp.baseName(cFooBar), Some("bar"));
    expect.equal(Fp.baseName(cFoo), Some("foo"));
    expect.equal(Fp.baseName(cDrive), None);
  });

  test("Weird characters", ({expect}) => {
    /**
     * We might actually want to rethink this behavior about spaces.
     * Currently, spaces don't need to be escaped in the convention and some
     * files might start with a space! Who would do that to their computer?
     */
    let path = Fp.absoluteCurrentPlatformExn(" / ");
    expect.string(path |> Fp.toString).toEqual("/");

    let path = Fp.absoluteCurrentPlatformExn(" C:/ ");
    expect.string(path |> Fp.toString).toEqual("C:/");

    let path = Fp.absoluteCurrentPlatformExn("/C:/");
    expect.string(path |> Fp.toString).toEqual("/C:");

    let path = Fp.relativeExn(".../a");
    expect.string(path |> Fp.toDebugString).toEqual("./.../a");

    let path = Fp.relativeExn("../C:/");
    expect.string(path |> Fp.toDebugString).toEqual("./../C:");

    let path = Fp.absoluteCurrentPlatformExn("/a/C:/");
    expect.string(path |> Fp.toString).toEqual("/a/C:");

    let path = Fp.relativeExn("./a/C:/");
    expect.string(path |> Fp.toDebugString).toEqual("./a/C:");

    let path = Fp.relativeExn("./a/C:/../");
    expect.string(path |> Fp.toDebugString).toEqual("./a");

    let path = Fp.absoluteCurrentPlatformExn("/foo\\/");
    expect.string(path |> Fp.toString).toEqual("/foo\\/");

    let path = Fp.absoluteCurrentPlatformExn(" C:/\\/");
    expect.string(path |> Fp.toString).toEqual("C:/\\/");
  });

  test("Relative/absolute inference", ({expect}) => {
    expect.fn(() => Fp.absoluteCurrentPlatformExn("/../../")).not.toThrow();

    expect.fn(() => Fp.relativeExn("./../../")).not.toThrow();

    expect.fn(() => Fp.relativeExn("../../")).not.toThrow();

    expect.fn(() => Fp.relativeExn("a/../../")).not.toThrow();

    expect.fn(() => Fp.relativeExn("~/../../")).not.toThrow();

    expect.fn(() => Fp.relativeExn("~/../../")).not.toThrow();

    expect.fn(() => Fp.relativeExn("~/a/../../")).not.toThrow();

    expect.fn(() => Fp.absoluteCurrentPlatformExn("C:/../")).not.toThrow();

    expect.fn(() => Fp.absoluteCurrentPlatformExn("")).toThrow();

    expect.fn(() => Fp.relativeExn("/foo")).toThrow();

    expect.fn(() => Fp.relativeExn("C:/foo")).toThrow();

    expect.fn(() => Fp.absoluteCurrentPlatformExn("a/b")).toThrow();
  });

  test("Path compression", ({expect}) => {
    let path = Fp.relativeExn("a/../..//");
    expect.string(path |> Fp.toDebugString).toEqual("./..");

    let path = Fp.absoluteCurrentPlatformExn("C:/../..//");
    expect.string(path |> Fp.toDebugString).toEqual("C:/");

    let path = Fp.absoluteCurrentPlatformExn("C://");
    expect.string(path |> Fp.toDebugString).toEqual("C:/");

    let path = Fp.absoluteCurrentPlatformExn("//");
    expect.string(path |> Fp.toDebugString).toEqual("/");

    let path = Fp.absoluteCurrentPlatformExn("/../..//");
    expect.string(path |> Fp.toDebugString).toEqual("/");

    let path = Fp.relativeExn("a/../../");
    expect.string(path |> Fp.toDebugString).toEqual("./..");

    let path = Fp.relativeExn("~/a/../../");
    expect.string(path |> Fp.toDebugString).toEqual("~/..");

    let path = Fp.relativeExn("~/a/../");
    expect.string(path |> Fp.toDebugString).toEqual("~/");

    let path = Fp.relativeExn("~/../../");
    expect.string(path |> Fp.toDebugString).toEqual("~/../..");

    let path = Fp.absoluteCurrentPlatformExn("/../../");
    expect.string(path |> Fp.toString).toEqual("/");

    /* Should not compress ../ for relatives beyond what is possible */
    let path = Fp.relativeExn("./../../");
    expect.string(path |> Fp.toDebugString).toEqual("./../..");

    let path = Fp.relativeExn("../../");
    expect.string(path |> Fp.toDebugString).toEqual("./../..");

    let path = Fp.relativeExn("../../a/../");
    expect.string(path |> Fp.toDebugString).toEqual("./../..");

    let path = Fp.relativeExn("./~");
    expect.string(path |> Fp.toDebugString).toEqual("./~");

    let path = Fp.absoluteCurrentPlatformExn("/~");
    expect.string(path |> Fp.toDebugString).toEqual("/~");

    let relativePath = Fp.relativeExn("");
    expect.string(relativePath |> Fp.toDebugString).toEqual("./");

    let relativePath = Fp.relativeExn(".");
    expect.string(relativePath |> Fp.toDebugString).toEqual("./");

    let relativePath = Fp.relativeExn("./");
    expect.string(relativePath |> Fp.toDebugString).toEqual("./");

    let relativePath = Fp.relativeExn(".//");
    expect.string(relativePath |> Fp.toDebugString).toEqual("./");
  });

  test("Path escaping", ({expect}) => {
    /**
     * We might actually want to rethink this behavior about spaces.
     * Currently, spaces don't need to be escaped in the convention and some
     * files might start with a space! Who would do that to their computer?
     */
    let path = Fp.absoluteCurrentPlatformExn(" C:/ ");
    expect.string(path |> Fp.toString).toEqual("C:/");

    /**
     * To make a truly universal usable path format, we should accept both
     * forward slashes, and backslashes in paths - *except* when a path has a
     * backslash followed immediately by a forward slash because that can only
     * mean an escaped forward slash. Similarly, a backslash followed
     * immediately by a backslash should also be considered a path separator.
     */
    let path = Fp.absoluteCurrentPlatformExn("/foo\\/");
    expect.string(path |> Fp.toString).toEqual("/foo\\/");

    let path = Fp.absoluteCurrentPlatformExn(" C:/\\/");
    expect.string(path |> Fp.toString).toEqual("C:/\\/");

    let path = Fp.relativeExn("./\\./foo");
    expect.string(path |> Fp.toDebugString).toEqual("./\\./foo");

    let path = Fp.relativeExn(".\\./foo");
    expect.string(path |> Fp.toDebugString).toEqual("./.\\./foo");

    let path = Fp.relativeExn("./.\\./foo");
    expect.string(path |> Fp.toDebugString).toEqual("./.\\./foo");

    let path = Fp.absoluteCurrentPlatformExn("C:/./\\./foo");
    expect.string(path |> Fp.toString).toEqual("C:/\\./foo");

    let path = Fp.absoluteCurrentPlatformExn("C:/\\/foo");
    expect.string(path |> Fp.toString).toEqual("C:/\\/foo");

    let path = Fp.absoluteCurrentPlatformExn("/\\/foo");
    expect.string(path |> Fp.toString).toEqual("/\\/foo");
  });

  test("Syntax", ({expect}) => {
    let root = Fp.root;
    let drive = Fp.drive("C:");
    let relRoot = Fp.relativeExn("a");
    let path = Fp.At.(relRoot / "b" /../ "");
    expect.string(path |> Fp.toDebugString).toEqual("./a");

    let path = Fp.At.(relRoot / "b" /../../ "");
    expect.string(path |> Fp.toDebugString).toEqual("./");

    let path = Fp.At.(drive / "a" / "b" /../ "");
    expect.string(path |> Fp.toDebugString).toEqual("C:/a");

    let path = Fp.At.(drive / "a" / "b" /../../ "");
    expect.string(path |> Fp.toDebugString).toEqual("C:/");

    let path = Fp.At.(root / "a" / "b" /../ "");
    expect.string(path |> Fp.toDebugString).toEqual("/a");

    let path = Fp.At.(root / "a" / "b" /../../ "");
    expect.string(path |> Fp.toDebugString).toEqual("/");
  });

  test("Relativization throws for mismatched drives", ({expect}) => {
    open Fp;
    let driveC = Fp.drive("C:");
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
    open Fp;
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
    open Fp;
    let root = Fp.root;

    /* let rel = */
    /*   Fp.(relativizeExn(~source=At.(root / "a"), ~dest=At.(root / "a"))); */
    /* expect.bool(Fp.eq(Fp.dot, rel)).toBeTrue(); */

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
    open Fp;
    let driveC = Fp.drive("C:");

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
    open Fp;

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
    open Fp;

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
    open Fp;
    let root = Fp.root;

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

    let res =
      isDescendent(~ofPath=At.(Fp.drive("C")), At.(root / "x" / "y"));
    expect.bool(res).toBeFalse();
  });

  test("Containment for absolute drives", ({expect}) => {
    open Fp;
    let drive = Fp.drive("C:");

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

    let res = isDescendent(~ofPath=At.(Fp.root), At.(drive / "x" / "y"));
    expect.bool(res).toBeFalse();
  });

  test("Containment for relative paths", ({expect}) => {
    open Fp;
    let dot = Fp.dot;

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

    let res = isDescendent(~ofPath=At.(Fp.home), At.(dot / "x" / "y"));
    expect.bool(res).toBeFalse();
  });

  test("Containment for home relative paths", ({expect}) => {
    open Fp;
    let home = Fp.home;

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

    let res = isDescendent(~ofPath=At.(Fp.dot), At.(home / "x" / "y"));
    expect.bool(res).toBeFalse();
  });
});
