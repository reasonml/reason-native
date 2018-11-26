# `refmterr`: Reason Formatting of Errors for Native Builds.

#### Demo
![Demo](https://raw.githubusercontent.com/esy-ocaml/BetterErrors/master/demo.jpg)


## Support

- This is an [`esy`](https://github.com/esy-ocaml/esy) compatible package.
- Supports `ocamlopt/ocamlc` versions `4.02.3`-`4.05`.

## Usage

**Package Builds:**

Add a dependency on `refmterr`, and `@opam/reason` in your `dependencies`, and
then you will be able to wrap your build commands with `refmterr`. For example,
instead of using `make build` as your build command, use `remfterr make build`.

```json
{
  "esy": {
    "build":  [
      ["refmterr make build"]
    ],
    ...
  },

  "dependencies": {
    "refmterr": "^3.0.0",
    "@opam/reason": "^3.0.0"
  }
}
```

**Command Line Usage:**

You can wrap any command that outputs `ocamlopt/ocamlc` error messages.  For
example, from your sandboxed project with the above `package.json`, you could
run:

```sh
esy refmterr ocamlopt -pp refmt someFile.re
```

and have any errors/warnings printed more readably.

## Contributing

- Install the latest esy `npm install -g esy`.
- `git clone` this repo, `cd` into it, then run:
```sh
esy install
esy build
esy ./runTests.sh
```
