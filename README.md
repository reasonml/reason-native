# Reason Native

[![Build Status](https://dev.azure.com/reason-native/reason-native/_apis/build/status/facebookexperimental.reason-native?branchName=master)](https://dev.azure.com/reason-native/reason-native/_build/latest?definitionId=1?branchName=master)
![Supported Platforms][platforms-badge]

Testing, printing, coloring, and other tools to effectively write native Reason code.

## Packages:


[![rely esy package on npm][rely-badge]](https://www.npmjs.com/package/@reason-native/rely)
[![pastel esy package on npm][pastel-badge]](https://www.npmjs.com/package/@reason-native/pastel)
[![console esy package on npm][console-badge]](https://www.npmjs.com/package/@reason-native/console)
[![refmterr esy package on npm][refmterr-badge]](https://www.npmjs.com/package/refmterr)


[**`@reason-native/rely`:**](https://reason-native.com/docs/rely/) Native Jest style API.

[**`@reason-native/console`:**](https://reason-native.com/docs/console/quickstart) A web-influenced polymorphic console API for native `Console.log(anything)` with runtime printing.

[**`@reason-native/pastel`:**](https://reason-native.com/docs/pastel/quickstart) Terminal highlighting with support for nesting and style propagation.

[**`refmterr`:**](https://reason-native.com/docs/refmterr/quickstart) Utility for extracting structure from unstructured ocaml compiler errors, and displaying them.

**`@reason-native/file-context-printer`:** Utility for displaying snippets of files on the command line.

**`@reason-native/pastel-console`:** To be used in `devDependencies`. Enhances `console` with colors (not yet released)

## Status:

Inside of this repo, there are several packages inside the `src` directory that
are to be individually released when their APIs are sufficiently stable.
Consult the [docs](https://reason-native.com/) for getting started with each package.

## Contributing

* install esy (https://esy.sh/)
* run `esy install` and `esy build`
* test executables are currently defined in the respective .json files for each repository, to run them run `esy x ExecutableName.exe`
* Look for Github issues tagged "good first issue".

## Breaking changes
The packages in reason-native adhere to [semantic versioning](https://semver.org/). Notably we do not consider changes that may induce [warning 9 errors](https://caml.inria.fr/pub/docs/manual-ocaml/comp.html#sec287) (such as adding new fields to records) to be breaking changes. Consequently minor version upgrades may cause warning 9 errors.

## License

reason-native is MIT licensed, as found in the LICENSE file.

[platforms-badge]: https://img.shields.io/badge/platforms-OSX%20%7C%20Windows%20%7C%20Linux-lightgray.svg

[pastel]: https://www.npmjs.com/package/@reason-native/pastel
[pastel-badge]: https://img.shields.io/npm/v/@reason-native/pastel/latest.svg?color=blue&label=@reason-native/pastel&style=flat&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAzOTcgNDE3IiB3aWR0aD0iMzk3IiBoZWlnaHQ9IjQxNyI+PGcgZmlsbD0iI0ZDRkFGQSI+PHBhdGggZD0iTTI2Ny42NDYgMTQyLjk4MmwzOS42MTYtMjIuOTQ2TDI2Ny41ODMgOTcuMmwtMzkuNjE2IDIyLjk0NiAzOS42NzkgMjIuODM2em0tNjkuMzI4IDQwLjEyOWwzOS42MTYtMjIuOTQ1LTM5LjY3OS0yMi44MzYtMzkuNjE2IDIyLjk0NiAzOS42NzkgMjIuODM1em0tNjkuNDM5LTQwLjEzbDM5LjYxNi0yMi45NDVMMTI4LjgxNiA5Ny4yIDg5LjIgMTIwLjE0NmwzOS42NzkgMjIuODM1em02OS4zMjgtMzkuOThsMzkuNjE2LTIyLjk0NS0zOS42NzktMjIuODM2LTM5LjYxNiAyMi45NDYgMzkuNjc5IDIyLjgzNXoiLz48cGF0aCBkPSJNMTkuODU2IDEzNy41OTFsMTY4LjYzOCA5Ny4wNTEuMjA2IDE0OC43ODlMMjAuMDYzIDI4Ni4zOGwtLjIwNy0xNDguNzg5ek0xOTguMTEyIDIyLjg5bDE2OC42MzcgOTcuMDUyLTE2OC4zNjcgOTcuNTE5TDI5Ljc0NCAxMjAuNDFsMTY4LjM2OC05Ny41MnptMTc4LjU3MyAxMTQuMjA2bC4yMDcgMTQ4Ljc4OS0xNjguMzY4IDk3LjUxOS0uMjA2LTE0OC43ODkgMTY4LjM2Ny05Ny41MTl6TTE5OC4wOCAwTDAgMTE0LjcyOGwuMjU1IDE4My4xMjUgMTk4LjM5NyAxMTQuMTc4IDE5OC4wOC0xMTQuNzI4LS4yNTUtMTgzLjEyNUwxOTguMDggMHoiLz48L2c+PC9zdmc+Cg== "esy package on npm"

[rely]: https://www.npmjs.com/package/@reason-native/rely
[rely-badge]: https://img.shields.io/npm/v/@reason-native/rely/latest.svg?color=blue&label=@reason-native/rely&style=flat&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAzOTcgNDE3IiB3aWR0aD0iMzk3IiBoZWlnaHQ9IjQxNyI+PGcgZmlsbD0iI0ZDRkFGQSI+PHBhdGggZD0iTTI2Ny42NDYgMTQyLjk4MmwzOS42MTYtMjIuOTQ2TDI2Ny41ODMgOTcuMmwtMzkuNjE2IDIyLjk0NiAzOS42NzkgMjIuODM2em0tNjkuMzI4IDQwLjEyOWwzOS42MTYtMjIuOTQ1LTM5LjY3OS0yMi44MzYtMzkuNjE2IDIyLjk0NiAzOS42NzkgMjIuODM1em0tNjkuNDM5LTQwLjEzbDM5LjYxNi0yMi45NDVMMTI4LjgxNiA5Ny4yIDg5LjIgMTIwLjE0NmwzOS42NzkgMjIuODM1em02OS4zMjgtMzkuOThsMzkuNjE2LTIyLjk0NS0zOS42NzktMjIuODM2LTM5LjYxNiAyMi45NDYgMzkuNjc5IDIyLjgzNXoiLz48cGF0aCBkPSJNMTkuODU2IDEzNy41OTFsMTY4LjYzOCA5Ny4wNTEuMjA2IDE0OC43ODlMMjAuMDYzIDI4Ni4zOGwtLjIwNy0xNDguNzg5ek0xOTguMTEyIDIyLjg5bDE2OC42MzcgOTcuMDUyLTE2OC4zNjcgOTcuNTE5TDI5Ljc0NCAxMjAuNDFsMTY4LjM2OC05Ny41MnptMTc4LjU3MyAxMTQuMjA2bC4yMDcgMTQ4Ljc4OS0xNjguMzY4IDk3LjUxOS0uMjA2LTE0OC43ODkgMTY4LjM2Ny05Ny41MTl6TTE5OC4wOCAwTDAgMTE0LjcyOGwuMjU1IDE4My4xMjUgMTk4LjM5NyAxMTQuMTc4IDE5OC4wOC0xMTQuNzI4LS4yNTUtMTgzLjEyNUwxOTguMDggMHoiLz48L2c+PC9zdmc+Cg== "esy package on npm"

[console]: https://www.npmjs.com/package/@reason-native/console
[console-badge]: https://img.shields.io/npm/v/@reason-native/console/latest.svg?color=blue&label=@reason-native/console&style=flat&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAzOTcgNDE3IiB3aWR0aD0iMzk3IiBoZWlnaHQ9IjQxNyI+PGcgZmlsbD0iI0ZDRkFGQSI+PHBhdGggZD0iTTI2Ny42NDYgMTQyLjk4MmwzOS42MTYtMjIuOTQ2TDI2Ny41ODMgOTcuMmwtMzkuNjE2IDIyLjk0NiAzOS42NzkgMjIuODM2em0tNjkuMzI4IDQwLjEyOWwzOS42MTYtMjIuOTQ1LTM5LjY3OS0yMi44MzYtMzkuNjE2IDIyLjk0NiAzOS42NzkgMjIuODM1em0tNjkuNDM5LTQwLjEzbDM5LjYxNi0yMi45NDVMMTI4LjgxNiA5Ny4yIDg5LjIgMTIwLjE0NmwzOS42NzkgMjIuODM1em02OS4zMjgtMzkuOThsMzkuNjE2LTIyLjk0NS0zOS42NzktMjIuODM2LTM5LjYxNiAyMi45NDYgMzkuNjc5IDIyLjgzNXoiLz48cGF0aCBkPSJNMTkuODU2IDEzNy41OTFsMTY4LjYzOCA5Ny4wNTEuMjA2IDE0OC43ODlMMjAuMDYzIDI4Ni4zOGwtLjIwNy0xNDguNzg5ek0xOTguMTEyIDIyLjg5bDE2OC42MzcgOTcuMDUyLTE2OC4zNjcgOTcuNTE5TDI5Ljc0NCAxMjAuNDFsMTY4LjM2OC05Ny41MnptMTc4LjU3MyAxMTQuMjA2bC4yMDcgMTQ4Ljc4OS0xNjguMzY4IDk3LjUxOS0uMjA2LTE0OC43ODkgMTY4LjM2Ny05Ny41MTl6TTE5OC4wOCAwTDAgMTE0LjcyOGwuMjU1IDE4My4xMjUgMTk4LjM5NyAxMTQuMTc4IDE5OC4wOC0xMTQuNzI4LS4yNTUtMTgzLjEyNUwxOTguMDggMHoiLz48L2c+PC9zdmc+Cg== "esy package on npm"
[refmterr-badge]: https://img.shields.io/npm/v/refmterr/latest.svg?color=blue&label=refmterr&style=flat&logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAzOTcgNDE3IiB3aWR0aD0iMzk3IiBoZWlnaHQ9IjQxNyI+PGcgZmlsbD0iI0ZDRkFGQSI+PHBhdGggZD0iTTI2Ny42NDYgMTQyLjk4MmwzOS42MTYtMjIuOTQ2TDI2Ny41ODMgOTcuMmwtMzkuNjE2IDIyLjk0NiAzOS42NzkgMjIuODM2em0tNjkuMzI4IDQwLjEyOWwzOS42MTYtMjIuOTQ1LTM5LjY3OS0yMi44MzYtMzkuNjE2IDIyLjk0NiAzOS42NzkgMjIuODM1em0tNjkuNDM5LTQwLjEzbDM5LjYxNi0yMi45NDVMMTI4LjgxNiA5Ny4yIDg5LjIgMTIwLjE0NmwzOS42NzkgMjIuODM1em02OS4zMjgtMzkuOThsMzkuNjE2LTIyLjk0NS0zOS42NzktMjIuODM2LTM5LjYxNiAyMi45NDYgMzkuNjc5IDIyLjgzNXoiLz48cGF0aCBkPSJNMTkuODU2IDEzNy41OTFsMTY4LjYzOCA5Ny4wNTEuMjA2IDE0OC43ODlMMjAuMDYzIDI4Ni4zOGwtLjIwNy0xNDguNzg5ek0xOTguMTEyIDIyLjg5bDE2OC42MzcgOTcuMDUyLTE2OC4zNjcgOTcuNTE5TDI5Ljc0NCAxMjAuNDFsMTY4LjM2OC05Ny41MnptMTc4LjU3MyAxMTQuMjA2bC4yMDcgMTQ4Ljc4OS0xNjguMzY4IDk3LjUxOS0uMjA2LTE0OC43ODkgMTY4LjM2Ny05Ny41MTl6TTE5OC4wOCAwTDAgMTE0LjcyOGwuMjU1IDE4My4xMjUgMTk4LjM5NyAxMTQuMTc4IDE5OC4wOC0xMTQuNzI4LS4yNTUtMTgzLjEyNUwxOTguMDggMHoiLz48L2c+PC9zdmc+Cg== "esy package on npm"
