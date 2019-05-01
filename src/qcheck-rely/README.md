# qcheck-rely

`qcheck-rely` contains custom Rely matchers that allow for easily using [QCheck](https://github.com/c-cube/qcheck) with Rely. QCheck is a "QuickCheck inspired property-based testing for OCaml, and combinators to generate random values to run tests on."

### Installation (assuming [esy](https://esy.sh/) for package management and dune for building)

You will want to add both `@opam/qcheck` and `@reason-native/qcheck-rely` to your `package.json` or `eys.json` file and
`qcheck-core` and `qcheck-rely` to your `dune` file.

### Usage

For writing QCheck tests, see [QCheck](https://github.com/c-cube/qcheck). Integration with Rely can be done as below:

```reason
open TestFramework;
open QCheckRely;

let {describe, _} = extendDescribe(QCheckRely.Matchers.matchers);

describe("qcheck-rely", ({test, _}) => {
  test("qcheck tests", ({expect}) => {
    let myQCheckTest = ...
    expect.ext.qCheckTest(myQCheckTest);
    ();
  });
  test("qcheck cells", ({expect}) => {
    let myQCheckCell = ...
    expect.ext.qCheckCell(myQCheckCell);
    ();
  });
  test("qcheck tests with custom random seed", ({expect}) => {
    let myQCheckTest = ...
    /* can also be specified by the QCHECK_SEED environment variable */
    let customRandomSeed = Random.State.make([|42|]);
    expect.ext.qCheckTest(~rand=customRandomSeed, myQCheckTest);
    ();
  });
```

