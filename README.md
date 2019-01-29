# Reason Native

[![Build Status](https://dev.azure.com/reason-native/reason-native/_apis/build/status/facebookexperimental.reason-native?branchName=master)](https://dev.azure.com/reason-native/reason-native/_build/latest?definitionId=1?branchName=master)

Testing, printing, coloring, and other tools to effectively write native Reason code.

## Packages:

- `@reason-native/pastel`: Terminal highlighting with support for nesting and style propagation.
- `@reason-native/console`: A web-influenced polymorphic console API for native `Console.log(anything)` with runtime printing.
- `@reason-native/file-context-printer`: Utility for displaying snippets of files on the command line.
- `@reason-native/refmterr`: Utility for extracting structure from unstructured ocaml compiler errors, and displaying them.
- `@reason-native/rely`: Native Jest style API.
- `@reason-native/pastel-console`: To be used in `devDependencies`. Enhances `console` with colors.

## Status:

Inside of this repo, there are several packages inside the `src` directory that are to be individually
released when their APIs are sufficiently stable.

## Examples

* [Rely](src/rely)

## Contributing

* install esy (https://esy.sh/)
* run `esy install` and `esy build`
* test executables are currently defined in the respective .json files for each repository, to run them run `esy x ExecutableName.exe`

## Breaking changes
The packages in reason-native adhere to [semantic versioning](https://semver.org/). Notably we do not consider changes that may induce [warning 9 errors](https://caml.inria.fr/pub/docs/manual-ocaml/comp.html#sec287) (such as adding new fields to records) to be breaking changes. Consequently minor version upgrades may cause warning 9 errors.

## License

reason-native is MIT licensed, as found in the LICENSE file.
