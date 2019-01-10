# rely

## Installation

We recommend that you use [`esy`](https://esy.sh/) to handle your package management. To install esy using npm, run
```bash
npm install -g esy
```

Add it as a dependency to your package.json (or esy.json) and run ```esy install```. If you don't want to distribute your tests as part of your release, you can utilize  [multiple sandboxes](https://esy.sh/docs/en/multiple-sandboxes.html) and .opam files to separate your dependencies

**package.json**
```
...

dependencies": {
    ...
    "@reason-native/rely": "*",
    ...
},
...
```
## Creating a test package

Let's start by creating a library for our tests. First create an opam file for your test package (it should be empty). Then let's create a directory called test and create a dune file to define compilation targets for our test library and executable (if you wish to use another build system, the important thing here is to pass the -linkall flag to the compiler)
```
│
├─my-lib-test.opam
├─test/
│   dune
│
```

#### dune
```
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
```
│
├─my-lib-test.opam
├─test/
│   dune
│   TestFramework.re
```

#### TestFramework.re
```reason
include Rely.Make({
  let config =
    Rely.TestFrameworkConfig.initialize({
      snapshotDir: "path/to/test/lib/__snapshots__",
      projectDir: "path/to/your/project"
    });
});
```

Now we can finally write our first test!
```
│
├─my-lib-test.opam
├─test/
│   dune
│   TestFramework.re
│   MyFirstTest.re
```

```reason
open TestFramework;

describe("my first test suite", ({test}) => {
  test("1 + 1 should equal 2", ({expect}) => {
    expect.int(1 + 1).toBe(2);
  });
});
```

From here let's create an executable to actually run our tests (the name of this file corresponds to the name specified in the executable stanza in the dune file).
```
│
├─my-lib-test.opam
├─test/
│   dune
│   TestFramework.re
│   MyFirstTest.re
│   RunTests.re
```

#### RunTests.re
```reason
MyLibTest.TestFramework.cli()
```

Finally we can run `esy build && esy x RunTests.exe` to build and run our tests.

## Developing:

```
npm install -g esy
git clone <this-repo>
esy install
esy build
```

## Running Tests:

```
esy test
```

## License
@reason-native/rely is MIT licensed, as found in the LICENSE file at the root of the reason-native repository.

