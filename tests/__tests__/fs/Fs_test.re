/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open TestFramework;

external reraise: exn => _ = "%reraise";

let unresolvedExePath = Path.absoluteExn(Sys.executable_name);
let resolvedExePath = Fs.resolveLinkExn(unresolvedExePath);
let exeDir = Path.dirName(resolvedExePath);
let testDir = Path.At.(exeDir / "testDir");
let testDataDir = Path.At.(exeDir / "testDir" / "data");

let ensureTestDir = () => {
  switch (Fs.query(testDir)) {
  | Some(Dir(_)) => Fs.rmDirExn(testDir)
  | Some(_) => Fs.rmExn(testDir)
  | None => ()
  };
  Fs.mkDirPExn(testDataDir);
};

let removeTestDir = () => Fs.rmDirExn(testDataDir);

let otherPlatform = Sys.win32 ? Fs.Unix : Fs.Windows;

describe("Fs", ({test}) => {
  test("Removing directories", ({expect}) => {
    ensureTestDir();
    let simpleTextFilePath = Path.append(testDataDir, "simple-text-file.txt");
    let simpleLines = ["hello", "these", "are", "some", "lines"];
    Fs.writeTextExn(simpleTextFilePath, simpleLines);
    expect.fn(() => Fs.rmEmptyDirExn(testDataDir)).toThrow();
    Fs.writeTextExn(~lineEnds=otherPlatform, simpleTextFilePath, simpleLines);
    let lines = Fs.readTextExn(simpleTextFilePath);
    expect.lines(lines).toEqualLines(simpleLines);
    removeTestDir();
  });
  test("Writing lines", ({expect}) => {
    ensureTestDir();
    let simpleTextFilePath = Path.append(testDataDir, "simple-text-file.txt");
    expect.fn(() => Fs.readTextExn(simpleTextFilePath)).toThrow();
    let simpleLines = ["hello", "these", "are", "some", "lines"];
    Fs.writeTextExn(simpleTextFilePath, simpleLines);
    let lines = Fs.readTextExn(simpleTextFilePath);
    expect.string(String.concat("", lines)).toEqual(
      String.concat("", simpleLines),
    );
    Fs.writeTextExn(simpleTextFilePath, simpleLines);
    let lines = Fs.readTextExn(simpleTextFilePath);
    expect.lines(lines).toEqualLines(simpleLines);
    Fs.writeTextExn(~lineEnds=otherPlatform, simpleTextFilePath, simpleLines);
    let lines = Fs.readTextExn(simpleTextFilePath);
    expect.lines(lines).toEqualLines(simpleLines);
    removeTestDir();
  });
  test("Links", ({expect}) => {
    ensureTestDir();
    let dir = Path.append(testDataDir, "dir");
    let dirFoo = Path.append(dir, "foo");
    expect.fn(() => Fs.readDirExn(dir)).toThrow();
    Fs.mkDirPExn(dir);
    expect.fn(() => Fs.readDirExn(dir)).not.toThrow();
    /* dir/foo still throws */
    expect.fn(() => Fs.readDirExn(dirFoo)).toThrow();
    let subdirs = Fs.readDirExn(dir);
    expect.int(List.length(subdirs)).toBe(0);
    let linkToDir = Path.append(testDataDir, "toDir");
    let linkToDirFoo = Path.append(testDataDir, "toDirFoo");
    /* Create links to files that don't yet exist */
    Fs.linkDirExn(linkToDir, dir);
    Fs.linkDirExn(linkToDirFoo, dirFoo);
    let subdirsInMainTestData = Fs.readDirExn(testDataDir);
    expect.int(List.length(subdirsInMainTestData)).toBe(3);
    /* Test that we can remove the link */
    Fs.rmExn(linkToDir);
    Fs.rmExn(linkToDirFoo);
    /* Recreate them */
    Fs.linkExn(linkToDir, dir);
    Fs.linkExn(linkToDirFoo, dirFoo);

    /* Test that the links point to the right places */
    expect.string(Fs.resolveLinkExn(linkToDir) |> Path.toString).toEqual(
      dir |> Path.toString,
    );
    expect.string(Fs.resolveLinkExn(linkToDirFoo) |> Path.toString).toEqual(
      dirFoo |> Path.toString,
    );
    expect.lines(List.map(Path.toString, Fs.linksExn(linkToDir))).
      toEqualLines([
      Path.toString(dir),
    ]);
    expect.lines(List.map(Path.toString, Fs.linksExn(linkToDirFoo))).
      toEqualLines([
      Path.toString(dirFoo),
    ]);

    /* Two levels of links */
    let linkToToDirFoo = Path.append(testDataDir, "toToDirFoo");
    /* Create links to files that don't yet exist */
    Fs.linkDirExn(linkToToDirFoo, linkToDirFoo);
    /* Test that the "hops" for multiple links are correct */
    expect.string(Fs.resolveLinkExn(linkToToDirFoo) |> Path.toString).toEqual(
      dirFoo |> Path.toString,
    );
    expect.lines(List.map(Path.toString, Fs.linksExn(linkToToDirFoo))).
      toEqualLines([
      Path.toString(linkToDirFoo),
      Path.toString(dirFoo),
    ]);

    /* Now test relative links */
    let relLinkToDirFoo = Path.append(testDataDir, "relToDirFoo");
    let relLinkToToDirFoo = Path.append(testDataDir, "relToToDirFoo");
    /* Create links to files that don't yet exist */
    Fs.linkExn(relLinkToDirFoo, Path.At.(Path.dot / "dir" / "foo"));
    Fs.linkExn(relLinkToToDirFoo, Path.At.(Path.dot / "toDirFoo"));
    /* Test that the "hops" for multiple links are correct */
    expect.string(Fs.resolveLinkExn(relLinkToDirFoo) |> Path.toString).
      toEqual(
      dirFoo |> Path.toString,
    );
    expect.string(Fs.resolveLinkExn(relLinkToToDirFoo) |> Path.toString).
      toEqual(
      dirFoo |> Path.toString,
    );
    expect.lines(List.map(Path.toString, Fs.linksExn(relLinkToDirFoo))).
      toEqualLines([
      Path.toString(dirFoo),
    ]);
    expect.lines(List.map(Path.toString, Fs.linksExn(relLinkToToDirFoo))).
      toEqualLines([
      Path.toString(linkToDirFoo),
      Path.toString(dirFoo),
    ]);

    /* Now test sideways links */
    let deepDir = Path.append(testDataDir, "deepDir");
    Fs.mkDirExn(deepDir);
    let sidewaysLinkToDirFoo = Path.append(deepDir, "sidewaysToDirFoo");
    /* Create links to files that don't yet exist */
    Fs.linkExn(sidewaysLinkToDirFoo, Path.At.(Path.dot /../ "dir" / "foo"));
    /* Test that the "hops" for multiple links are correct */
    expect.string(Fs.resolveLinkExn(sidewaysLinkToDirFoo) |> Path.toString).
      toEqual(
      dirFoo |> Path.toString,
    );
    expect.lines(List.map(Path.toString, Fs.linksExn(relLinkToDirFoo))).
      toEqualLines([
      Path.toString(dirFoo),
    ]);

    let lines = ["bye", "hello"];
    /* Test writing lines through the links */
    Fs.writeTextExn(linkToDirFoo, lines);

    /* Test reading lines through the links */
    let writtenLines = Fs.readTextExn(linkToDirFoo);
    expect.lines(writtenLines).toEqualLines(lines);

    let linkDest = Fs.resolveLinkExn(linkToDirFoo);
    let lines = ["bye", "hello"];
    /* Test writing lines through the final link destination */
    Fs.writeTextExn(linkDest, lines);
    /* Test reading lines through the final link destination */
    let writtenLines = Fs.readTextExn(linkDest);
    expect.lines(writtenLines).toEqualLines(lines);
    /* Test reading lines through the links */
    let writtenLines = Fs.readTextExn(linkToDirFoo);
    expect.lines(writtenLines).toEqualLines(lines);
  });
});
