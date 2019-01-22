open Printf;

let rimraf = s =>
  switch (Bos.OS.Dir.delete(~recurse=true, Fpath.v(s))) {
  | Ok () => ()
  | _ => Printf.fprintf(stdout, "Warning: Could not delete %s\n", s)
  };

let buffer_size = 8192;
let buffer = Bytes.create(buffer_size);

let runCommandWithEnv = (command, args) => {
  let attach =
    Unix.create_process_env(
      command,
      Array.append([|command|], args),
      Unix.environment(),
    );
  let pid = attach(Unix.stdin, Unix.stdout, Unix.stderr);
  switch (Unix.waitpid([], pid)) {
  | (_, WEXITED(c)) => c
  | (_, WSIGNALED(c)) => c
  | (_, WSTOPPED(c)) => c
  };
};

let mkdir = (~perms=?, p) =>
  switch (perms) {
  | Some(x) => Unix.mkdir(p, x)
  | None => Unix.mkdir(p, 0o755)
  };

let file_copy = (input_name, output_name) => {
  open Unix;
  let fd_in = openfile(input_name, [O_RDONLY], 0);
  let fd_out = openfile(output_name, [O_WRONLY, O_CREAT, O_TRUNC], 438);
  let rec copy_loop = () =>
    switch (read(fd_in, buffer, 0, buffer_size)) {
    | 0 => ()
    | r =>
      ignore(write(fd_out, buffer, 0, r));
      copy_loop();
    };

  copy_loop();
  close(fd_in);
  close(fd_out);
};

module Path = {
  let (/) = Filename.concat;
};

let parent = Filename.dirname;

let tmpDir = Filename.get_temp_dir_name();
let testProject = "test-project";
let testProjectDir = Filename.concat(tmpDir, testProject);
let pesyBinPath = "pesy";

let esyCommand =
  Sys.unix ?
    "esy" :
    {
      let pathVars =
        Array.to_list(Unix.environment())
        |> List.map(e =>
             switch (Str.split(Str.regexp("="), e)) {
             | [k, v, ...rest] => Some((k, v))
             | _ => None
             }
           )
        |> List.filter(
             fun
             | None => false
             | _ => true,
           )
        |> List.filter(e =>
             switch (e) {
             | Some((k, _)) => String.lowercase_ascii(k) == "path"
             | _ => false
             }
           )
        |> List.map(
             fun
             | Some(x) => x
             | None => ("", "") /* Why not filter_map? */
           );

      let v =
        List.fold_right(
          (e, acc) => {
            let (_, v) = e;
            acc ++ (Sys.unix ? ":" : ";") ++ v;
          },
          pathVars,
          "",
        );

      Unix.putenv("PATH", v);

      let paths = Str.split(Str.regexp(Sys.unix ? ":" : ";"), v);
      List.iter(print_endline, paths);
      let npmPaths =
        List.filter(
          path => Sys.file_exists(Filename.concat(path, "esy.cmd")),
          paths,
        );
      switch (npmPaths) {
      | [] =>
        fprintf(stderr, "No npm bin path found");
        exit(-1);
      | [h, ..._] => Filename.concat(h, "esy.cmd")
      };
    };

rimraf(testProjectDir); /* So that we can run it stateless locally */
mkdir(testProjectDir);
Sys.chdir(testProjectDir);

let exitStatus = runCommandWithEnv(pesyBinPath, [||]);
if (exitStatus != 0) {
  Printf.fprintf(
    stderr,
    "Test failed: Non zero exit when running bootstrapper",
  );
  exit(-1);
};

let exitStatus =
  runCommandWithEnv(esyCommand, [|"x", "TestProjectApp.exe"|]);

if (exitStatus != 0) {
  Printf.fprintf(
    stderr,
    "Test failed: Non zero exit when running TestProjectApp.exe\n Code: %d",
    exitStatus,
  );
  exit(-1);
};

let exitStatus =
  runCommandWithEnv(esyCommand, [|"x", "TestTestProject.exe"|]);

if (exitStatus != 0) {
  Printf.fprintf(
    stderr,
    "Test failed: Non zero exit when running TestTestProject.exe\n Code: %d\n",
    exitStatus,
  );
  exit(-1);
};
