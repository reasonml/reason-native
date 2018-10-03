# @reason-native/chalk


[![CircleCI](https://circleci.com/gh/jordwalke/chalk/tree/master.svg?style=svg)](https://circleci.com/gh/jordwalke/chalk/tree/master)


**Contains the following libraries and executables:**

```
@reason-native/chalk@0.0.0
│
├─test/
│   name:    TestChalk.exe
│   main:    TestChalk
│   require: chalk.lib
│
├─library/
│   library name: chalk.lib
│   namespace:    Chalk
│   require:
│
└─executable/
    name:    ChalkApp.exe
    main:    ChalkApp
    require: chalk.lib
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
esy x ChalkApp.exe 
```

## Running Tests:

```
# Runs the "test" command in `package.json`.
esy test
```
