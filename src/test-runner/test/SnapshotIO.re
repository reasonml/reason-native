/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 */
module type SnapshotIO {
    let existsFile: string => bool;
    let writeFile: (string, string) => unit;
    let removeFile: string => unit;
    let readFile: string => string;
    let readSnapshotNames: string => list(string);
  };

  module FileSystemSnapshotIO: SnapshotIO = {
    exception SnapshotDirectoryNotFound(string);
    let existsFile = Sys.file_exists;
    let writeFile = (name, contents) => {
      IO.mkdirp(Filename.dirname(name));
      IO.writeFile(name, contents);
    };
    let removeFile = Sys.remove;
    let readFile = IO.readFile;
    let readSnapshotNames = snapshotDir =>
      if (Sys.file_exists(snapshotDir)) {
        snapshotDir
        |> Sys.readdir
        |> Array.to_list
        |> List.map(fileName => Filename.concat(snapshotDir, fileName))
        |> List.filter(filePath => !Sys.is_directory(filePath));
      } else {
        raise(SnapshotDirectoryNotFound(snapshotDir));
      };
  };
