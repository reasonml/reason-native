/**

`Fp` is a library for creating and operating on file paths consistently on all
platforms.

`Fp` works exactly the same on Windows, Linux, and OSX, instead of adjusting
behavior based on your current OS

The `Fp` API uses the following conventions:

- Accepts/returns only `t(absolute))` for values that must be absolute paths.
- Accepts/returns only `t(relative))` for values that must be absolute paths.
- Accepts `t('any)` for values that may be either absolute or relative paths.
- Returns `firstClass = Absolute(t(absolute)) | Relative(t(relative)` for
  return values that could be either. Consumers must pattern match on it.
- Wraps return values in `Some(..)` / `None` when it is possible that no
  value may be computed even when the caller supplies valid data.
- Wraps return values in `Ok(..)` / `Error(exn)` when it is possible that no
  value may be computed due to an error occuring in either user input or system
  failure.
- For every `functionName` that wraps return values in `Ok`/`Error`, an
  alternative form `functionNameExn` is also supplied which does not wrap
  the return value, and instead raises an exception.

TODO: Consider the following universal convention instead:

    type specificUsageError = UserNameInvalid | LoggedOut;
    type blame('usage) = | Caller('usage) | Implementation(exn);
    // Returns Error only for system blame:
    result(x, exn)
    // Returns Error for caller/system blame
    result(x, blame(usage))
    // Returns Error only for caller blame
    result(x, usage)
    // Returns Error for caller/system blame, but no value expected.
    result(option(x), blame(usage))
    // Returns Error for system blame, but no value expected.
    result(option(x), exn)
*/

type relative;
type absolute;
/**
A file system path, parameterized on the kind of file system path,
`Fp.t(relative)` or `Fp.t(absolute)`.
*/
type t('kind);

/**
Used to allow dynamically checking whether or not a path is absolute or
relative. Use seldomly.
*/
type firstClass =
  | Absolute(t(absolute))
  | Relative(t(relative));

let drive: string => t(absolute);
let root: t(absolute);
let home: t(relative);
let dot: t(relative);

/**
The `platform` describes how path handling should be performed - using
Windows-style paths, or Posix-style paths.

In Windows-style Win32 paths, both the back-slash and forward-slash are considered
viable path separators. In Posix style paths, only the forward-slash is used
as a path separator

Posix-style paths are used in the following systems:
- Linux variants
- Mac OSX
- Cygwin on Windows
*/
type windows =
  | Win32
  | Cygwin;
type platform =
  | Windows(windows)
  | Posix;

let platform: Lazy.t(platform);

/**
Queries whether a path is absolute or relative. Use seldomly, and typically
only on end-user input. Once queried, use the wrapped `t(absolute)/t(relative)`
as the primary path passed around.
*/
let testForPath: string => option(firstClass);

/**
Same as `testForPath`, but raises `Invalid_argument` if no path could be
detected.
*/
let testForPathExn: string => firstClass;

/**
Creates a "first class" path could be _either_ a relative path or an absolute
one.

This allows you to return values from functions that might be absolute or might
be relative. It also allows relative and absolute paths to coexist inside of a
list together.
For example, if you create a polymorphic function that accepts any kind of
path, and then you want to do something differently based on whether or not the
path is relative or absolute, you would first use `firstClass(path)` and then
pattern match on the result `Absolute(p) => .. | Relative(p) => ...`.
*/
let firstClass: t('any) => firstClass;

/**
 Prints absolute `Fp.t` as strings, always removes the final `/` separator.
 */
let toString: t(absolute) => string;

/**
 Prints any `Fp.t` for debugging, always removes the final `/` separator
 except in the case of the empty relative paths `./`, `~/`.
 */
let toDebugString: t('kind) => string;

/**
Parses an absolute path into a `Fp.t(absolute)` or returns `None` if the path
is not a absolute, yet still valid. Raises Invalid_argument if the path is
invalid.

The `fromPlatform` argument specifies how the path should be parsed,
as Windows supports backslashes as path separators.
 */
let absolutePlatform:
  (~fromPlatform: platform, string) => option(t(absolute));

/**
Same as `Fp.absolutePlatform`, except `fromPlatform` is set to the current platform
*/
let absoluteCurrentPlatform: string => option(t(absolute));

/**
 Parses a relative path into a `Fp.t(relative)` or returns `None` if the path
 is not a valid.
 */
let relative: string => option(t(relative));

/**
 Same as `Fp.absolutePlatform` but raises a Invalid_argument if argument is not a
 valid absolute path.

 The `fromPlatform` argument specifies how the path should be parsed,
 as Windows supports backslashes as path separators.
 */
let absolutePlatformExn: (~fromPlatform: platform, string) => t(absolute);

/**
 Same as `Fp.absolutePlatformPathExn`, with `fromPlatform` set to the current platform.
 */
let absoluteCurrentPlatformExn: string => t(absolute);

/**
 Same as `Fp.relative` but raises a Invalid_argument if argument is not a
 valid relative path.
 */
let relativeExn: string => t(relative);

/**
Creates a relative path from two paths, which is the relative path that is
required to arive at the `dest`, if starting from `source` directory. The
`source` and `dest` must both be `t(absolute)` or `t(relative)`, but the
returned path is always of type `t(relative)`.

If `source` and `dest` are relative, it is assumed that the two relative paths
are relative to the same yet-to-be-specified absolute path.

relativize(~source=/a, ~dest=/a)                == ./
relativize(~source=/a/b/c/d /a/b/qqq            == ../c/d
relativize(~source=/a/b/c/d, ~dest=/f/f/zzz)    == ../../../../f/f/zz
relativize(~source=/a/b/c/d, ~dest=/a/b/c/d/q)  == ../q
relativize(~source=./x/y/z, ~dest=./a/b/c)      == ../../a/b/c
relativize(~source=./x/y/z, ~dest=../a/b/c)     == ../../../a/b/c

Unsupported:
`relativize` only accepts `source` and `dest` of the same kind of path because
the following are meaningless:

relativize(~source=/x/y/z, ~dest=./a/b/c) == ???
relativize(~source=./x/y/z, ~dest=/a/b/c) == ???

Exceptions:
If it is impossible to create a relative path from `source` to `dest` an
exception is raised.
If `source`/`dest` are absolute paths, the drive must match or an exception is
thrown. If `source`/`dest` are relative paths, they both must be relative to
`"~"` vs. `"."`. If both are relative, but the source has more `..` than the
dest, then it is also impossible to create a relative path and an exception is
raised.

relativize(~source=./foo/bar, ~dest=~/foo/bar)       == raise(Invalid_argument)
relativize(~source=~/foo/bar, ~dest=./foo/)          == raise(Invalid_argument)
relativize(~source=C:/foo/bar, ~dest=/foo/bar)       == raise(Invalid_argument)
relativize(~source=C:/foo/bar, ~dest=F:/foo/bar)     == raise(Invalid_argument)
relativize(~source=/foo/bar, ~dest=C:/foo/)          == raise(Invalid_argument)
relativize(~source=../x/y/z, ~dest=./a/b/c)          == raise(Invalid_argument)
relativize(~source=../x/y/z, ~dest=../foo/../a/b/c)  == raise(Invalid_argument)
*/
let relativizeExn: (~source: t('kind), ~dest: t('kind)) => t(relative);
/**
Same as `relativizeExn` but returns `result(Fp.t(Fp.absolute), exn)` instead
of throwing an exception.
*/
let relativize:
  (~source: t('kind), ~dest: t('kind)) => result(t(relative), exn);

/**
Accepts any `Fp.t` and returns a `Fp.t` of the same kind.  Relative path
inputs return relative path outputs, and absolute path inputs return absolute
path outputs.
*/
let dirName: t('kind) => t('kind);

/**
Accepts any `Fp.t` and returns the final segment in its path string, or `None`
if there are no segments in its path string.

   Fp.baseName(Fp.At(Fp.dot /../ ""))
   None

   Fp.baseName(Fp.At(Fp.dot /../ "foo"))
   Some("foo")

   Fp.baseName(Fp.At(Fp.dot /../ "foo" /../ ""))
   None

   Fp.baseName(Fp.At(Fp.dot /../ "foo" / "bar" /../ ""))
   Some("foo")
*/
let baseName: t('kind) => option(string);

/**
Appends one segment to a path. Preserves the relative/absoluteness of the first
arguments.
*/
let append: (t('kind), string) => t('kind);

/**
Appends one path to another. Preserves the relative/absoluteness of the first
arguments.
*/
let join: (t('kind1), t('kind2)) => t('kind1);

let eq: (t('kind1), t('kind2)) => bool;

/**
Tests for path equality of two absolute paths.
*/
let absoluteEq: (t(absolute), t(absolute)) => bool;

/**
Tests for path equality of two absolute paths.
*/
let relativeEq: (t(relative), t(relative)) => bool;

/**
Tests whether or not an absolute path has a parent path. Absolute paths such as
"C:/" and "/" have no parent dir.
*/
let hasParentDir: t(absolute) => bool;

/**
Returns `true` if a path exists inside another path `~ofPath` or is equal to
`~ofPath`.
*/
let isDescendent: (~ofPath: t('kind), t('kind)) => bool;

/**
Syntactic forms for utilities provided above. These are included in a separate
module so that it can be opened safely without causing collisions with other
identifiers in scope such as "root"/"home".

Use like this:

    Fp.At(Fp.root / "foo" / "bar");
    Fp.At(Fp.dot /../ "bar");
*/
module At: {
  /**
   Performs `append` with infix syntax.
   */
  let (/): (t('kind), string) => t('kind);
  /**
   `dir /../ s` is equivalent to `append(dirName(dir), s)`
   */
  let (/../): (t('kind), string) => t('kind);
  /**
   `dir /../../ s` is equivalent to `append(dirName(dirName(dir)), s)`
   */
  let (/../../): (t('kind), string) => t('kind);
  /**
   `dir /../../../ s` is equivalent to
   `append(dirName(dirName(dirName(dir))), s)`
   */
  let (/../../../): (t('kind), string) => t('kind);
  /**
   `dir /../../../../ s` is equivalent to
   `append(dirName(dirName(dirName(dirName(dir)))), s)`
   */
  let (/../../../../): (t('kind), string) => t('kind);
  /**
   `dir /../../../../../ s` is equivalent to
   `append(dirName(dirName(dirName(dirName(dirName(dir))))), s)`
   */
  let (/../../../../../): (t('kind), string) => t('kind);
  /**
   `dir /../../../../../../ s` is equivalent to
   `append(dirName(dirName(dirName(dirName(dirName(dirName(dir)))))), s)`
   */
  let (/../../../../../../): (t('kind), string) => t('kind);
};
