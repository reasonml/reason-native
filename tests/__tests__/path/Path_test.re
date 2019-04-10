/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open TestFramework;

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
});
