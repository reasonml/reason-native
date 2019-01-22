exception InvalidSubDirs(string);
type include_subdirs =
  | No
  | Unqualified;

type t = {
  path: string,
  name: string,
  require: list(string),
  flags: option(list(string)), /* TODO: Use a variant instead since flags only accept a set of values and not any list of strings */
  ocamlcFlags: option(list(string)),
  ocamloptFlags: option(list(string)),
  jsooFlags: option(list(string)),
  preprocess: option(list(string)),
  includeSubdirs: option(include_subdirs),
  rawBuildConfig: option(list(string)),
  rawBuildConfigFooter: option(list(string)),
};
let create =
    (
      name,
      path,
      require,
      flags,
      ocamlcFlags,
      ocamloptFlags,
      jsooFlags,
      preprocess,
      includeSubdirs,
      rawBuildConfig,
      rawBuildConfigFooter,
    ) => {
  let includeSubDirsSafe =
    switch (includeSubdirs) {
    | Some(x) =>
      switch (x) {
      | "no" => Some(No)
      | "unqualified" => Some(Unqualified)
      | _ => raise(InvalidSubDirs(x))
      }
    | None => None
    };
  {
    name,
    path,
    require,
    flags,
    ocamlcFlags,
    ocamloptFlags,
    jsooFlags,
    preprocess,
    includeSubdirs: includeSubDirsSafe,
    rawBuildConfig,
    rawBuildConfigFooter,
  };
};
let toDuneStanzas = c => {
  let {
    name,
    require,
    flags,
    ocamlcFlags,
    ocamloptFlags,
    jsooFlags,
    preprocess,
    includeSubdirs,
    rawBuildConfig,
    rawBuildConfigFooter,
    _,
  } = c;
  (
    /* public_name: */ Stanza.create("public_name", Stanza.createAtom(name)),
    /* libraries: */
    switch (require) {
    | [] => None
    | libs =>
      Some(
        Stanza.createExpression([
          Stanza.createAtom("libraries"),
          ...List.map(r => Stanza.createAtom(r), libs),
        ]),
      )
    },
    /* flags: */
    switch (flags) {
    | None => None
    | Some(l) =>
      Some(
        Stanza.createExpression([
          Stanza.createAtom("flags"),
          ...List.map(f => Stanza.createAtom(f), l),
        ]),
      )
    },
    /* ocamlcFlags */
    switch (ocamlcFlags) {
    | None => None
    | Some(l) =>
      Some(
        Stanza.createExpression([
          Stanza.createAtom("ocamlc_flags"),
          ...List.map(f => Stanza.createAtom(f), l),
        ]),
      )
    },
    /* ocamloptFlags */
    switch (ocamloptFlags) {
    | None => None
    | Some(l) =>
      Some(
        Stanza.createExpression([
          Stanza.createAtom("ocamlopt_flags"),
          ...List.map(f => Stanza.createAtom(f), l),
        ]),
      )
    },
    /* jsooFlags */
    switch (jsooFlags) {
    | None => None
    | Some(l) =>
      Some(
        Stanza.createExpression([
          Stanza.createAtom("js_of_ocaml"),
          ...List.map(f => Stanza.createAtom(f), l),
        ]),
      )
    },
    /* preprocess */
    switch (preprocess) {
    | None => None
    | Some(l) =>
      Some(
        Stanza.createExpression([
          Stanza.createAtom("preprocess"),
          Stanza.createExpression(List.map(f => Stanza.createAtom(f), l)),
        ]),
      )
    },
    /* includeSubdirs */
    switch (includeSubdirs) {
    | None => None
    | Some(v) =>
      Some(
        Stanza.createExpression([
          Stanza.createAtom("include_subdirs"),
          Stanza.createAtom(
            switch (v) {
            | No => "no"
            | Unqualified => "unqualified"
            },
          ),
        ]),
      )
    },
    /* rawBuildConfig */
    switch (rawBuildConfig) {
    | None => None
    | Some(l) => Some(l |> List.map(Stanza.ofString))
    },
    /* rawBuildConfig */
    switch (rawBuildConfigFooter) {
    | None => None
    | Some(l) => Some(l |> List.map(Stanza.ofString))
    },
  );
};

let getPath = c => c.path;
