open PesyUtils;

let userCommand =
  if (Array.length(Sys.argv) > 1) {
    Some(Sys.argv[1]);
  } else {
    None;
  };

let projectRoot =
  switch (Sys.getenv_opt("cur__root")) {
  | Some(curRoot) =>
    /**
     * This means the user ran pesy in an esy environment.
     * Either as
     * 1. esy pesy
     * 2. esy b pesy
     * 3. esy pesy build
     * 4. esy b pesy build
     */
    curRoot
  | None =>
    /**
     * This mean pesy is being run naked on the shell.
     * Either it was:
     */
    print_endline("pesy-configure must be run in esy's env");
    exit(0);
  };

/* use readFileOpt to read previously computed directory path */
let mode =
  PesyLib.Mode.EsyEnv.(
    switch (userCommand) {
    | _ => UPDATE
    }
  );

switch (mode) {
| UPDATE =>
  let packageJSONPath = Path.(projectRoot / "package.json");
  PesyLib.PesyConf.gen(projectRoot, packageJSONPath);
};

print_endline("Build config updated!");
