/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 */
/**
 * Reads a file from disk. This returns a list containing each line of the file.
 */
let readLines = (file: string) : list(string) => {
    let lines = ref([]);
    let chan = open_in(file);
    let finished_lines =
      try (
        {
          while (true) {
            lines := [input_line(chan), ... lines^];
          };
          [];
        }
      ) {
      | End_of_file =>
        close_in(chan);
        lines^ |> List.rev;
      };
    finished_lines;
  };

  /**
   * Reads a file from disk. This returns a single, new-line separated string
   * containing the contents of the file.
   */
  let readFile = (file: string) : string =>
    String.concat("\n", readLines(file));

  /**
   * Recursively makes sure a directory exists.
   */
  let rec mkdirp = (dirPath: string) : unit =>
    if (Sys.file_exists(dirPath)) {
      /* The directory exists, stop recursing but check error cases */
      if (Sys.is_directory(dirPath)) {
        ();
          /* All good */
      } else {
        /* This means something like 'foo/bar' is a file, but we are trying
           to create the directory 'foo/bar/baz' since 'bar' is a file not a
           directory this will obviously fail. Here we call out the case
           explicitly and try to provide a helpful error message. */
        failwith(
          Printf.sprintf(
            "'%s' exists but is not a directory, aborting",
            dirPath,
          ),
        );
      };
    } else {
      mkdirp(Filename.dirname(dirPath));
      /* 493 = 0o755 */
      Unix.mkdir(dirPath, 493);
      ();
    };

  /**
   * Very simple wrapper around a `rm -rf <path>` command.
   */
  let rmrf = (path: string) : unit =>
    Core_extended.Std.Shell.run("rm", ["-rf", path]);

  /**
   * Writes a file at the given path containing the given contents.
   */
  let writeFile = (filePath: string, contents: string) => {
    let outFile = open_out(filePath);
    output_string(outFile, contents);
    close_out(outFile);
  };

  /**
   * TODO: Allow passing a serializer and file descriptor the child fn can write
   * its return value to.
   */
  let captureOutput = (fn: unit => unit) : (string, string, Unix.process_status) => {
    /* Capture normal stdout/stderr channels */
    let oldstdout = Unix.dup(Unix.stdout);
    let oldstderr = Unix.dup(Unix.stderr);
    /* Create tmpOut and tmpErr files */
    let (tmpOut, tmpOutChannel) = Filename.open_temp_file("reason-", ".stdout");
    let (tmpErr, tmpErrChannel) = Filename.open_temp_file("reason-", ".stderr");
    /* Redirect stdout/stderr channels to the tmp files */
    Unix.dup2(Unix.descr_of_out_channel(tmpOutChannel), Unix.stdout);
    Unix.dup2(Unix.descr_of_out_channel(tmpErrChannel), Unix.stderr);
    /**
     * Fork and call the function that may produce output. The fork is necessary
     * in case the function fatals. We do not want to crash the parent program.
     */
    let pid = Unix.fork();
    if (pid === 0) {
      fn();
      /*
       * Exit the child process, we don't care about the program anymore. We have
       * already captured the output.
       */
      exit(0);
    } else {
      /* Wait for the child process to finish */
      let (_, processStatus) = Unix.waitpid([], pid);
      /* Flush channels */
      flush(tmpOutChannel);
      flush(tmpErrChannel);
      /* Restore old channels */
      Unix.dup2(oldstdout, Unix.stdout);
      Unix.dup2(oldstderr, Unix.stderr);
      /* Read tmp files */
      let stdOutStr = readFile(tmpOut);
      let stdErrStr = readFile(tmpErr);
      /* Cleanup tmp files */
      Unix.unlink(tmpOut);
      Unix.unlink(tmpErr);
      /* Return outputs and exitStatus */
      (stdOutStr, stdErrStr, processStatus);
    };
  };
