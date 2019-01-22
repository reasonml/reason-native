# pesy

**Contains the following libraries and executables:**

```
pesy@0.0.0
│
├─test/
│   name:    TestPesy.exe
│   main:    TestPesy
│   require: pesy.lib
│
├─library/
│   library name: pesy.lib
│   namespace:    Pesy
│   require:
│
└─executable/
    name:    Pesy.exe
    main:    Pesy
    require: pesy.lib
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
esy x Pesy.exe 
```

## Running Tests:

```
# Runs the "test" command in `package.json`.
esy test
```
