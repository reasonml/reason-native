---
id: introduction
title: Reason Native
sidebar_label: Introduction
---

Testing, printing, coloring, and other tools to effectively write native Reason code.


## Packages:

- `@reason-native/pastel`: Terminal highlighting with support for nesting and style propagation.
- `@reason-native/console`: A web-influenced polymorphic console API for native `Console.log(anything)` with runtime printing.
- `@reason-native/file-context-printer`: Utility for displaying snippets of files on the command line.
- `@reason-native/refmterr`: Utility for extracting structure from unstructured ocaml compiler errors, and displaying them.
- `@reason-native/rely`: Native Jest style API.
- `@reason-native/pastel-console`: To be used in `devDependencies`. Enhances `console` with colors.

## Status:

The packages included in this repo are not yet released to npm/opam. They are being made public
as a way to gather feedback and share progress on native command line tooling.

Inside of this repo, there are several packages inside the `src` directory that are to be individually
released when their APIs are sufficiently stable.


## Examples

* [Test Runner](src/rely)

## Contributing

* install esy (https://esy.sh/)
* run `esy install` and `esy build`
* test executables are currently defined in the respective .json files for each repository, to run them run `esy x ExecutableName.exe`

## License

Reason Native is MIT licensed, as found in the LICENSE file.
