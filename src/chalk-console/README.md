# @reason-native/chalk-console


[![CircleCI](https://circleci.com/gh/jordwalke/chalk-console/tree/master.svg?style=svg)](https://circleci.com/gh/jordwalke/chalk-console/tree/master)


**Contains the following libraries and executables:**

```
@reason-native/chalk-console@0.0.0
│
├─test/
│   name:    TestChalkConsole.exe
│   main:    TestChalkConsole
│   require: chalk-console.lib
│
└─library/
    library name: chalk-console.lib
    namespace:    ChalkConsole
    require:
 
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
esy x TestChalkConsole.exe 
```

## Running Tests:

```
# Runs the "test" command in `package.json`.
esy test
```
