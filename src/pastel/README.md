# @reason-native/pastel

**Contains the following libraries and executables:**

```
@reason-native/pastel@0.0.0
│
├─test/
│   name:    TestPastel.exe
│   main:    TestPastel
│   require: pastel.lib
│
├─library/
│   library name: pastel.lib
│   namespace:    Pastel
│   require:
│
└─executable/
    name:    PastelApp.exe
    main:    PastelApp
    require: pastel.lib
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
esy x PastelApp.exe
```

## Running Tests:

```
# Runs the "test" command in `package.json`.
esy test
```
