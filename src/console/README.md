# @reason-native/console


[![CircleCI](https://circleci.com/gh/jordwalke/console/tree/master.svg?style=svg)](https://circleci.com/gh/jordwalke/console/tree/master)


**Contains the following libraries and executables:**

```
@reason-native/console@0.0.0
│
├─test/
│   name:    TestConsole.exe
│   main:    TestConsole
│   require: console.lib
│
├─library/
│   library name: console.lib
│   namespace:    Console
│   require:
│
└─executable/
    name:    ConsoleApp.exe
    main:    ConsoleApp
    require: console.lib
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
esy x ConsoleApp.exe 
```

## Running Tests:

```
# Runs the "test" command in `package.json`.
esy test
```
