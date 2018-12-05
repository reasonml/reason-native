# @reason-native/pastel-console

**Contains the following libraries and executables:**

```
@reason-native/pastel-console@0.0.0
│
├─test/
│   name:    TestPastelConsole.exe
│   main:    TestPastelConsole
│   require: pastel-console.lib
│
└─library/
    library name: pastel-console.lib
    namespace:    PastelConsole
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
esy x TestPastelConsole.exe
```

## Running Tests:

```
# Runs the "test" command in `package.json`.
esy test
```
