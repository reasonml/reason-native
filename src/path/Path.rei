/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 *
 * @emails oncall+ads_front_end_infra
 */;

/**

`Path` is a library for creating and operating on file paths consistently on
all platforms.

`Path` works exactly the same on Windows, Linux, and OSX, instead of adjusting
behavior based on your current OS

*/;

type relative;
type absolute;
/**
A file system path, parameterized on the kind of file system path,
`Path.t(relative)` or `Path.t(absolute)`.
 */
type t('kind);

let drive: string => t(absolute);
let root: t(absolute);
let home: t(relative);
let dot: t(relative);

/**
 Prints absolute `Path.t` as strings, always removes the final `/` separator.
 */
let toString: t(absolute) => string;

/**
 Prints any `Path.t` for debugging, always removes the final `/` separator
 except in the case of the empty relative paths `./`, `~/`.
 */
let toDebugString: t('kind) => string;

/**
 Parses an absolute path into a `Path.t(absolute)` or returns `None` if the path
 is not a valid.
 */
let absolute: string => option(t(absolute));
/**
 Parses a relative path into a `Path.t(relative)` or returns `None` if the path
 is not a valid.
 */
let relative: string => option(t(relative));

/**
 Same as `Path.absolute` but raises a Invalid_argument if argument is not a
 valid absolute path.
 */
let absoluteExn: string => t(absolute);

/**
 Same as `Path.relative` but raises a Invalid_argument if argument is not a
 valid relative path.
 */
let relativeExn: string => t(relative);

/**
 Accepts any `Path.t` and returns a `Path.t` of the same kind.  Relative path
 inputs return relative path outputs, and absolute path inputs return absolute
 path outputs.
 */
let dirName: t('kind) => t('kind);

/**
 Accepts any `Path.t` and returns the final segment in its path string, or
 `None` if there are no segments in its path string.

    Path.baseName(Path.At(Path.dot /../ ""))
    None

    Path.baseName(Path.At(Path.dot /../ "foo"))
    Some("foo")

    Path.baseName(Path.At(Path.dot /../ "foo" /../ ""))
    None

    Path.baseName(Path.At(Path.dot /../ "foo" / "bar" /../ ""))
    Some("foo")
 */
let baseName: t('kind) => option(string);

/**
 Appends one path to another. Preserves the relative/absoluteness of the first
 arguments.
 */
let append: (t('kind), string) => t('kind);
let join: (t('kind1), t('kind2)) => t('kind1);

/**
 Syntactic forms for utilities provided above. These are included in a separate
 module so that it can be opened safely without causing collisions with other
 identifiers in scope such as "root"/"home".

 Use like this:

     Path.At(Path.root / "foo" / "bar");
     Path.At(Path.dot /../ "bar");
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
