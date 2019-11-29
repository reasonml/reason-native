---
id: quickstart
title: Rely Quickstart
sidebar_label: Quickstart
---
> This quickstart builds off [Getting Started](../getting-started) and assumes you have a native Reason project set up.

## Install Rely

To install Rely in your project with esy, run
```sh
esy add @reason-native/rely
```

This will add `@reason-native/rely` into your `package.json`.

In order to use Rely, you must add it to your build system. With dune, `rely.lib` must be added to [the relevant `dune` file](https://jbuilder.readthedocs.io/en/latest/dune-files.html#library-dependencies):

```lisp
(libraries ... rely.lib)
```

## Create a test

Let's start by creating a library for our tests. Create a directory called test and create a dune file for our library (if you wish to use another build system, the important thing here is to pass the -linkall flag to the compiler)
```sh
├─test/
│   lib/
│       dune
```

```lisp
;; dune file
(library
   (name MyLibTest)
   (public_name my-lib-test.lib)
   ; the linkall flag ensures that all of our tests are compiled and the
   ; -g flag emits debugging information
   (ocamlopt_flags -linkall -g)
   ; you will want to depend on the library you are testing as well, however for
   ; the purposes of this example we are only depending on the test runner itself
   (libraries rely.lib )
   (modules (:standard \ RunTests))
)
(executable
  ; the for the library is automatically detected because of the name, but we
  ; need to explicitly specify the package here
  (package my-lib-test)
  (name RunTests)
  (public_name RunTests.exe)
  (libraries
    my-lib-test.lib
  )
  (modules RunTests)
)
```

Now let's create a file to initialize the test framework. Here we are specifying where snapshots should be stored as well as the root directory of your project for the formatting of terminal output.
```sh
├─test/
│   lib/
│       dune
│       TestFramework.re
```

```re
/* TestFramework.re */
include Rely.Make({
  let config =
    Rely.TestFrameworkConfig.initialize({
      snapshotDir: "path/to/test/lib/__snapshots__",
      projectDir: "path/to/your/project"
    });
});
```

Now we can finally write our first test!
```sh
├─test/
│   lib/
│       dune
│       TestFramework.re
│       MyFirstTest.re
```

```re
/* MyFirstTest.re */
open TestFramework;

describe("my first test suite", ({test}) => {
  test("1 + 1 should equal 2", ({expect}) => {
    expect.int(1 + 1).toBe(2);
  });
});
```

From here let's create an executable to actually run our tests (the name of this file corresponds to the name specified in the executable stanza in the dune file).
```sh
│
├─my-lib-test.opam
├─test/
│   dune
│   TestFramework.re
│   MyFirstTest.re
│   RunTests.re
```

```re
/* RunTests.re */
MyLibTest.TestFramework.cli()
```

Finally we can run `esy build && esy x RunTests.exe` to build and run our tests.
