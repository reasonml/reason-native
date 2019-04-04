/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open CommonCollections;
open TestPath;
module MTestPathMap = MutableMap.Make(TestPath);

module type Config = {
  let snapshotDir: string;
  module IO: SnapshotIO.SnapshotIO;
};

exception SnapshotDirectoryNotFound(string);
exception InvalidSnapshotFileName(string);
exception DuplicateSnapshot(string);

let getX = (key, map) =>
  switch (MStringMap.getOpt(key, map)) {
  | Some(v) => v
  | None => failwith("shouldn't get here")
  };

module Make: (Config) => Snapshot.Sig =
  (Config: Config) => {
    type snapshotState = {
      unusedSnapshotSet: MStringSet.t,
      updatedSnapshotSet: MStringSet.t,
      removedSnapshotCount: ref(int),
      startingSnapshotSet: StringSet.t,
      testHashes: MStringMap.t(TestPath.test),
    };

    type testId = string;

    type snapshotParts = {
      describeFileName: string,
      testHash: string,
      index: int,
    };

    let initialSnapshots = Config.snapshotDir |> Config.IO.readSnapshotNames;

    let state =
      ref({
        unusedSnapshotSet: MStringSet.fromList(initialSnapshots),
        updatedSnapshotSet: MStringSet.empty(),
        removedSnapshotCount: ref(0),
        startingSnapshotSet: StringSet.fromList(initialSnapshots),
        testHashes: MStringMap.empty(),
      });

    let parseFileName = filename => {
      let filename = Filename.basename(filename);
      let dotRegex = Re.compile(Re.Pcre.re("[.]"));
      let parts = filename |> Re.split(dotRegex);
      /* snapshot files are of the form "describeFileName.TestHash.expectCounter.snapshot" */
      switch (parts) {
      | [describeFileName, testHash, index, _] => {
          describeFileName,
          testHash,
          index: int_of_string(index),
        }
      | _ => raise(InvalidSnapshotFileName(filename))
      };
    };

    let escapeSnapshot = (title: string, s: string): string => {
      ();
      let withoutEndline =
        s
        |> Str.split_delim(Str.regexp("\n"))
        |> List.map(String.escaped)
        |> String.concat("\n");
      title ++ "\n" ++ withoutEndline ++ "\n";
    };

    let unescapeSnapshot = (s: string): string => {
      ();
      let result =
        s
        |> Str.split_delim(Str.regexp("\n"))
        /* This removes the title we manually add */
        |> List.tl
        |> List.map(Scanf.unescaped)
        /*
         * Since concat does not add the delimeter after the last element we
         * have correctly unescaped the trailing newline we always add in escape.
         */
        |> String.concat("\n");
      result;
    };

    let getNewId = testPath => {
      let testHash = ref(None);
      let i = ref(0);
      let break = ref(false);
      while (! break^ && i^ < 10000) {
        let testHashAttempt = TestPath.hash(Test(testPath), i^);
        switch (testHash^) {
        | None =>
          if (!MStringMap.has(testHashAttempt, state^.testHashes)) {
            break := true;
            testHash := Some(testHashAttempt);
            let _ =
              MStringMap.set(testHashAttempt, testPath, state^.testHashes);
            ();
          }
        | _ => ()
        };
        incr(i);
      };
      let testHash =
        switch (testHash^) {
        | None => "hash_conflict"
        | Some(testHash) => testHash
        };
      testHash;
    };

    let sanitizeName = (name: string): string => {
      ();
      let name =
        name
        |> Str.split(Str.regexp("\\b"))
        /* TODO consider 0-9 as well, but would be a breaking change */
        |> List.map(Str.global_replace(Str.regexp("[^a-zA-Z]"), ""))
        |> List.filter(part => String.length(part) > 0)
        |> String.concat("_");
      let name =
        if (String.length(name) > 50) {
          String.sub(name, 0, 50) ++ "_";
        } else {
          name;
        };
      name;
    };

    let toFileName = (testHash, index) => {
      let (name, describe) = getX(testHash, state^.testHashes);
      let snapshotPrefix =
        Filename.concat(
          Config.snapshotDir,
          TestPath.describeToString(describe) |> sanitizeName,
        );
      String.concat(
        ".",
        [snapshotPrefix, testHash, string_of_int(index), "snapshot"],
      );
    };

    if (!Config.IO.existsFile(Config.snapshotDir)) {
      Config.IO.mkdirp(Config.snapshotDir);
    };

    let getState = () => state^;

    let getSnapshotStatus: unit => TestResult.AggregatedResult.snapshotSummary =
      () => {
        let {removedSnapshotCount} = state^;
        let updatedSnapshotCount = ref(0);
        let createdSnapshotCount = ref(0);
        let _ =
          MStringSet.forEach(
            snapshot =>
              if (StringSet.has(snapshot, state^.startingSnapshotSet)) {
                incr(updatedSnapshotCount);
              } else {
                incr(createdSnapshotCount);
              },
            state^.updatedSnapshotSet,
          );
        {
          numCreatedSnapshots: createdSnapshotCount^,
          numRemovedSnapshots: removedSnapshotCount^,
          numUpdatedSnapshots: updatedSnapshotCount^,
        };
      };

    let removeUnusedSnapshots: unit => unit =
      () => {
        List.iter(
          snapshot =>
            if (Config.IO.existsFile(snapshot)) {
              Config.IO.removeFile(snapshot);
              state^.removedSnapshotCount := state^.removedSnapshotCount^ + 1;
            },
          MStringSet.values(state^.unusedSnapshotSet),
        );
        ();
      };
    let readSnapshot: (testId, int) => option(string) =
      (testId, index) => {
        let fileName = toFileName(testId, index);
        switch (Config.IO.existsFile(fileName)) {
        | false => None
        | true => Some(fileName |> Config.IO.readFile |> unescapeSnapshot)
        };
      };
    let markSnapshotUsed = (testHash, index) => {
      let fileName = toFileName(testHash, index);
      let _ = MStringSet.delete(fileName, state^.unusedSnapshotSet);
      ();
    };
    let markSnapshotsAsCheckedForTest = testHash => {
      let (_, describe) = getX(testHash, state^.testHashes);
      let describeFileName =
        TestPath.describeToString(describe) |> sanitizeName;

      let _ =
        state^.unusedSnapshotSet
        |> MStringSet.filter(filename =>
             filename
             |> parseFileName
             |> (
               snapshotInfo =>
                 !(
                   snapshotInfo.testHash == testHash
                   && snapshotInfo.describeFileName == describeFileName
                 )
             )
           );
      ();
    };
    let updateSnapshot: (testId, int, string) => unit =
      (testHash, index, snapshotContents) => {
        let testPath = getX(testHash, state^.testHashes);
        let fileName = toFileName(testHash, index);
        Config.IO.writeFile(
          fileName,
          escapeSnapshot(testPath |> TestPath.testToString, snapshotContents),
        );
        let _ = markSnapshotUsed(testHash, index);
        let _ = MStringSet.add(fileName, state^.updatedSnapshotSet);
        ();
      };
  };
