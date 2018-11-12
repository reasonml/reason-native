# test-runner


[![CircleCI](https://circleci.com/gh/yourgithubhandle/test-runner/tree/master.svg?style=svg)](https://circleci.com/gh/yourgithubhandle/test-runner/tree/master)


**Contains the following libraries and executables:**

```
test-runner@0.0.0
│
├─test/
│   name:    TestTestRunner.exe
│   main:    TestTestRunner
│   require: test-runner.lib
│
├─library/
│   library name: test-runner.lib
│   namespace:    TestRunner
│   require:
│
└─executable/
    name:    TestRunnerApp.exe
    main:    TestRunnerApp
    require: test-runner.lib
```

## Developing:

```
npm install -g esy
git clone <this-repo>
esy install
esy build
```

## Running Binary:

After building the project, you can run the main binary that is produced.

```
esy x TestRunnerApp.exe 
```

## Running Tests:

```
# Runs the "test" command in `package.json`.
esy test
```
