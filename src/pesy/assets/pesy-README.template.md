# <PACKAGE_NAME_FULL>


[![CircleCI](https://circleci.com/gh/yourgithubhandle/<PACKAGE_NAME>/tree/master.svg?style=svg)](https://circleci.com/gh/yourgithubhandle/<PACKAGE_NAME>/tree/master)


**Contains the following libraries and executables:**

```
<PACKAGE_NAME_FULL>@0.0.0
│
├─test/
│   name:    Test<PACKAGE_NAME_UPPER_CAMEL>.exe
│   main:    Test<PACKAGE_NAME_UPPER_CAMEL>
│   require: <PUBLIC_LIB_NAME>
│
├─library/
│   library name: <PUBLIC_LIB_NAME>
│   namespace:    <PACKAGE_NAME_UPPER_CAMEL>
│   require:
│
└─executable/
    name:    <PACKAGE_NAME_UPPER_CAMEL>App.exe
    main:    <PACKAGE_NAME_UPPER_CAMEL>App
    require: <PUBLIC_LIB_NAME>
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
esy x <PACKAGE_NAME_UPPER_CAMEL>App.exe 
```

## Running Tests:

```
# Runs the "test" command in `package.json`.
esy test
```
