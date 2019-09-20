# fp

`Fp` is a human usable library for creating and operating on file paths
consistently on all platforms.

Similar to the [File
URI](https://tools.ietf.org/id/draft-kerwin-file-scheme-07.html) scheme, `Fp`
provides one consistent API that can be used to refer to files in a platform
agnostic manner but `Fp` differs in that `Fp` focuses on programmers
instead of on descriptions of files to be sent across the wire in networking
applications. For example, `Fp` expands upon the concept of a file URI by
also allowing the representation of a "relative" file path - something left
unspecified by file URI protocol.

Programmers can use `Fp` to reason about file system paths inside of their
code, and then convert the `Fp` data into `URI`s, or other platform specific
formats.


#### Typed Paths:

`Fp` keeps track of which paths are relative, absolute, so that you can write
libraries that demand only absolute paths. The types also allow APIs
that can accept either kind and even preserve the nature of the type
polymorphically.

For example, Fp is able to preserve its "absoluteness" across `append`
operations:


```reason
let newAbs : Fp.t(absolute) = Fp.append(myAbs, "foo");

let newRel : Fp.t(relative) = Fp.append(myRel, "foo");
```


#### Syntax:

`Fp` includes a `Fp.At` module that can be opened using the `Fp.At.(...)`
syntax. It provides path operations such as `/`, `/../`, and `/../../`.
```reason
let path = Fp.At.(rootDir / "usr" / "bin" / "example.exe");
let relative = Fp.At.(rootDir /../ "home" /../../ "apps");
```



#### More Examples:

Safe absolute and relative path parsing:
```reason
/* Some(Fp.t) */
let bin = Fp.absolute("/usr/bin");
/* Some(Fp.t) */
let bin = Fp.relative("./bin");
```

Less safe absolute and relative path parsing:
```reason
/* Fp.t */
let bin = Fp.absoluteExn("/usr/bin");
/* Fp.t */
let bin = Fp.relativeExn("./bin");
```

Constructing paths safely, segment by segment:
```reason
/* "/" */
let root = Fp.root;
/* /foo */
let foo = Fp.append(root, "foo");
/* /foo/bar */
let fooBar = Fp.append(root, "bar");
```
Or, alternatively:
```reason
let fooBar = Fp.At.(Fp.root / "foo" / "bar");
```

Common utility functions:
```reason
/* bar */
Fp.baseName(fooBar);
/* foo */
Fp.dirName(fooBar);
```

#### Goals:
- Developer usability.
- Very few dependencies if any.
- Small code size suitable for compiling to JavaScript/PHP.


#### Protocol:

Path parsing in the `Fp.absolute`/`Fp.relative` functions follow the `Fp`
convention, not any operating system convention. You should prefer the
functions `append` and `/` when possible, but if using the
`Fp.absolute/relative` functions, the following is the convention for parsing
the supplied paths.

- Path separators must always be forward slash and backslash is reserved for
  escaping.
- The grammar for paths is as follows:

```
ESCAPED_SEP=\/
DIR_CHARS= a-z | A-Z | 0-9 | _ | DOT | ......
DIR_NAME=(DIR_CHARS - DOT)+ (DIR_CHARS | ESCAPED_SEP)
SEGMENT=DOT | DOTDOT | DIR_NAME | EMPTY
ABSOLUTE=EMPTY
(DRIVE|RELATIVE|ABSOLUTE) (SEP SEGMENT)*
DRIVE=CAP_LETTER:/
RELATIVE=DOTDOT SLASH | DOT SLASH | DOT
```

Note that the above grammar doesn't specify how to interpret: `"a/b"`. If
`Fp` cannot parse the path with a drive, relative, or absolute, it will
consider the path relative (that is, `"./a/b"` in this case).

Eventually `Fp` should include functions for parsing OS specific path formats
into the canonical `Fp.t`.
