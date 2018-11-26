/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 */
module type SnapshotDir = {let snapshotDir: string;};

module MakeTestFramework =
       (SnapshotDir: SnapshotDir)
       : TestRunner.TestFramework =>
  TestRunner.Make({
    let config =
      TestRunner.TestFrameworkConfig.initialize({
        updateSnapshotsFlag: "-u",
        snapshotDir:
          Filename.(
            Sys.executable_name
            |> dirname
            |> dirname
            |> dirname
            |> dirname
            |> dirname
            |> dirname
            |> dirname
            |> (dir => Filename.concat(dir, "src"))
            |> (dir => Filename.concat(dir, "test-runner"))
            |> (dir => Filename.concat(dir, "test"))
            |> (dir => Filename.concat(dir, "lib"))
            |> (dir => Filename.concat(dir, SnapshotDir.snapshotDir))
          ),
        projectDir: "",
      });
  });

open TestRunner.Describe;
open TestRunner.Test;

let testRunnerOutputSnapshotTest = (testName, describeUtils, testFn) =>
  describeUtils.test(
    testName,
    testUtils => {
      let blankSpaceRegex = Str.regexp_string(" ");
      module TestFramework =
        MakeTestFramework({
          let snapshotDir =
            "__snapshots__test_runner_test_"
            ++ Str.global_replace(blankSpaceRegex, "_", testName);
        });
      let (stdout, _, _) =
        IO.captureOutput(() => {
          TestFramework.describe(testName, utils =>
            testFn(utils)
          );
          TestFramework.run(
            TestRunner.RunConfig.(initialize() |> updateSnapshots(false)),
          );
          ();
        });
      testUtils.expect.string(stdout).toMatchSnapshot();
    },
  );

TestFramework.describe("TestRunner", describeUtils => {
  let {test} = describeUtils;

  testRunnerOutputSnapshotTest(
    "failing tests",
    describeUtils,
    ({test}) => {
      test("expect.string.toBeEmpty", ({expect}) =>
        expect.string("foo").toBeEmpty()
      );
      test("expect.string.not.toBeEmpty", ({expect}) =>
        expect.string("").not.toBeEmpty()
      );
      test("expect.string.toEqual", ({expect}) =>
        expect.string("bacon").toEqual("delicious")
      );
      test("expect.string.not.toEqual", ({expect}) =>
        expect.string("bacon").not.toEqual("bacon")
      );
      /* Incredibly fragile and assumes test is run from the scripts/re directory */
      test("expect.string.toEqualFile", ({expect}) =>
        expect.string("sdfsdfs").toEqualFile("tests/testFiles/foo.txt")
      );
      /* Incredibly fragile and assumes test is run from the scripts/re directory */
      test("expect.string.not.toEqualFile", ({expect}) =>
        expect.string("mmm bacon").not.toEqualFile(
          "tests/testFiles/simple.txt",
        )
      );
      test("expect.file.not.toEqualFile", ({expect}) =>
        expect.file("tests/testFiles/simple.txt").not.toEqualFile(
          "tests/testFiles/simple.txt",
        )
      );
      test("expect.bool.toBe(true)", ({expect}) =>
        expect.bool(false).toBe(true)
      );
      test("expect.bool.not.toBe(false)", ({expect}) =>
        expect.bool(false).not.toBe(false)
      );
      test("expect.bool.toBeTrue", ({expect}) =>
        expect.bool(false).toBeTrue()
      );
      test("expect.bool.not.toBeTrue", ({expect}) =>
        expect.bool(true).not.toBeTrue()
      );
      test("expect.bool.toBeFalse", ({expect}) =>
        expect.bool(true).toBeFalse()
      );
      test("expect.bool.not.toBeFalse", ({expect}) =>
        expect.bool(false).not.toBeFalse()
      );
      test("expect.int.toBe", ({expect}) =>
        expect.int(42).toBe(43)
      );
      test("expect.string.toMatchSnapshot mismatch", ({expect}) =>
        expect.string("bacon3").toMatchSnapshot()
      );
      test("expect.string.toMatchSnapshot no snapshot exists", ({expect}) =>
        expect.string("more bacon!").toMatchSnapshot()
      );
    },
  );

  testRunnerOutputSnapshotTest(
    "passing tests",
    describeUtils,
    ({test}) => {
      test("Inner test 1", ({expect}) => {
        expect.string("foo").toEqual("foo");
        expect.string("bar").toEqual("bar");
      });
      test("Inner test 2", ({expect}) => {
        expect.string("foo").toEqual("foo");
        expect.string("bar").toEqual("bar");
      });
    },
  );

  testRunnerOutputSnapshotTest(
    "string operations", describeUtils, ({describe}) =>
    describe("Inner describe 2", ({test}) =>
      test("Inner test 1", ({expect}) => {
        expect.string("").toBeEmpty();
        expect.string("foo").not.toBeEmpty();
        expect.string("foo").toEqual("foo");
        expect.string("foo").not.toEqual("bar");
        expect.string("foo").toEqualLines(["foo"]);
        expect.string("foo").not.toEqualLines(["bar"]);
        expect.string("foo").toMatch("foo");
        expect.string("foo").not.toMatch("bar");
      })
    )
  );
  testRunnerOutputSnapshotTest("mixed tests", describeUtils, ({describe}) =>
    describe("Inner describe 1", ({test}) => {
      test("Inner test 1", ({expect}) => {
        expect.string("foo").toEqual("foo");
        expect.string("foo").toEqual("foo");
      });
      test("Inner test 2", ({expect}) => {
        expect.string("foo").toEqual("foo");
        expect.string("foo").toEqual("bar");
      });
      test("Inner test 3", ({expect}) => {
        expect.string("foo").toEqual("foo");
        expect.string("foo").toEqual("foo");
      });
    })
  );
  testRunnerOutputSnapshotTest(
    "nested describes", describeUtils, ({describe}) =>
    describe("Root", ({describe}) => {
      describe("Describe 1", ({test}) => {
        test("Inner test 1.1", ({expect}) => {
          expect.string("foo").toEqual("foo");
          expect.string("foo").toEqual("foo");
        });
        test("Inner test 1.2", ({expect}) => {
          expect.string("foo").toEqual("foo");
          expect.string("foo").toEqual("foo");
        });
      });
      describe("Describe 2", ({describe, test}) => {
        test("Inner test 2.1", ({expect}) => {
          expect.string("foo").toEqual("foo");
          expect.string("foo").toEqual("foo");
        });
        describe("Describe 3", ({test}) => {
          test("Inner test 3.1", ({expect}) => {
            expect.string("foo").toEqual("foo");
            expect.string("foo").toEqual("foo");
          });
          test("Inner test 3.2", ({expect}) => {
            expect.string("foo").toEqual("foo");
            expect.string("foo").toEqual("foo");
          });
        });
        test("Inner test 2.2", ({expect}) => {
          expect.string("foo").toEqual("foo");
          expect.string("foo").toEqual("foo");
        });
      });
      test("Inner test 1", ({expect}) => {
        expect.string("").toBeEmpty();
        expect.string("foo").not.toBeEmpty();
        expect.string("foo").toEqual("foo");
        expect.string("foo").not.toEqual("bar");
        expect.string("foo").toEqualLines(["foo"]);
        expect.string("foo").not.toEqualLines(["bar"]);
        expect.string("foo").toMatch("foo");
        expect.string("foo").not.toMatch("bar");
      });
    })
  );

  testRunnerOutputSnapshotTest(
    "exceptions in tests", describeUtils, ({describe}) =>
    describe("test framework handling of exceptions", ({test}) => {
      test("normal failure", ({expect}) =>
        expect.string("foo").toEqual("bar")
      );
      test("uninlineable exception output", ({expect}) => {
        let foo = () => raise(Invalid_argument("I'm invalid :("));
        expect.string("foo").toEqual("foo");
        foo();
      });
      test("inline exception output", ({expect}) => {
        let foo = () => raise(Not_found);
        expect.string("foo").toEqual("foo");
        foo();
      });
    })
  );
});
