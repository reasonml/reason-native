# test-runner

[![CircleCI](https://circleci.com/gh/yourgithubhandle/test-runner/tree/master.svg?style=svg)](https://circleci.com/gh/yourgithubhandle/test-runner/tree/master)

## Installation

We recommend that you use [`esy`](https://esy.sh/) to handle your package management. To install esy using npm, run
```bash
npm install -g esy
```

Add it as a dev dependency to your package.json (or esy.json) and run ```esy install```

**package.json**
```
...

devDependencies": {
    ...
    "@reason-native/test-runner": "*",
    ...
},
...
```
## Creating a test project

Let's start by creating a library for our tests inside a directory called test and create a dune file to handle building our code (if you wish to use another build system, the important thing here is to pass the -linkall flag to the compiler)
```
│
├─test/
│   lib/
|       dune
│
```

#### dune
```
(library
   (name MyLibTest)
   (public_name my-lib.test)
   ; the linkall flag ensures that all of our tests are compiled and the
   ; -g flag emits debugging information
   (ocamlopt_flags -linkall -g)
   ; you will want to depend on the library you are testing as well, however for
   ; the purposes of this example we are only depending on the test runner itself
   (libraries test-runner.lib )
)
```

Now let's create a file to initialize the test framework. Here we are specifying where snapshots should be stored as well as the root directory of your project for the formatting of terminal output.
```
│
├─test/
│   lib/
|       dune
|       TestFramework.re
```

#### TestFramework.re
```reason
include TestRunner.Make({
  let config =
    TestRunner.TestFrameworkConfig.initialize({
      snapshotDir: "path/to/test/lib/__snapshots__"
      projectDir: "path/to/your/project"
    });
});
```

Now we can finally write our first test!
```
│
├─test/
│   lib/
|       dune
|       TestFramework.re
|       MyFirstTest.re
```

```reason
open TestFramework;

describe("my first test suite", ({test}) => {
  test("1 + 1 should equal 2", ({expect}) => {
    expect.int(1 + 1).toBe(2);
  });
});
```

From here let's create an executable to actually run our tests.
```
│
├─test/
│   lib/
|       dune
|       TestFramework.re
|       MyFirstTest.re
|   exe/
|       dune
|       MyLibTest.re
```

#### dune
```
(executable
   (name MyLibTest)
   (public_name MyLibTest.exe)
   (libraries  my-lib.test )
)
```

#### MyLibTest.re
```reason
MyLibTest.TestFramework.cli()
```

Finally we can run `esy build && esy x MyLibTest.exe` to build and run our tests.

## Developing:

```
npm install -g esy
git clone <this-repo>
esy install
esy build
```

## Running Tests:

```
esy x TestRunnerTest.exe
```

## License
@reason-native/test-runner is MIT licensed, as found in the LICENSE file at the root of the reason-native repository.
