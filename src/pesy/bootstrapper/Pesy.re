open Printf;

let userCommand =
  if (Array.length(Sys.argv) > 1) {
    Some(Sys.argv[1]);
  } else {
    None;
  };

/* TODO: prompt user for their choice */
let projectRoot = Sys.getcwd();

/* use readFileOpt to read previously computed directory path */
printf("Bootstrapping files...");
flush_all();
PesyLib.bootstrapIfNecessary(projectRoot);
print_endline("done");
PesyLib.generateBuildFiles(projectRoot);

/*  @esy-ocaml/foo-package -> foo-package */
print_endline("Installing deps and building...");

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

      /* Unix.putenv("PATH", v); /\* This didn't work! *\/ */

      let paths = Str.split(Str.regexp(Sys.unix ? ":" : ";"), v);

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

print_endline("Running esy install...");

let setupStatus = PesyUtils.runCommandWithEnv(esyCommand, [|"install"|]);
if (setupStatus != 0) {
  fprintf(stderr, "esy (%s) install failed!", esyCommand);
  exit(-1);
};

print_endline("Running esy build...");
let setupStatus = PesyUtils.runCommandWithEnv(esyCommand, [|"build"|]);
if (setupStatus != 0) {
  fprintf(stderr, "esy (%s) build failed!", esyCommand);
  exit(-1);
};
print_endline("You can now run `esy test`");
