# May 29, 2019
### Rely 2.2.0
* expect.mock.nthCalledWith/nthReturnedWith matchers
* expect.result matchers for the built in result type
* various internal changes to support compilation to JS with Js_of_ocaml
  * it works!

# May 2, 2019
### qcheck-rely 1.0.2
* depend on @opam/qcheck-core instead of @opam/qcheck to drastically reduce
dependencies pulled in

# May 1, 2019
### qcheck-rely 1.0.1
* Remove confusing matcher hint from qcheck rely output

# April 30, 2019
### qcheck-rely 1.0.0
* Initial release, see the README in the src/qcheck-rely directory for usage

# April 26, 2019
### Rely 2.1.0
* describeOnly, testOnly
  * if there is a describeOnly or testOnly present anywhere in the set of tests
  to be run, only tests inside of a testOnly or describeOnly will be run. skip
  takes precedence over only (e.g. a testOnly inside a describeSkip will be skipped)
  * this is only intended for use in local development as a single only disables
  every other test
* ciMode
  * the ciMode option has been added to runConfig, when run with ciMode(true) or
  in CLI mode with the --ci flag ci mode will be enabled. Currently this causes
  Rely to throw an exception if there any testOnly or describeOnly. To skip
  tests in CI use testSkip/describeSkip
* expect.option matchers
  * toBeNone, toBeSome, toBe matchers

# April 15, 2019
### Rely 2.0.0
* [**BREAKING**] Add optional locations to Passed and Skipped test status variant constructors for reporters
* [**BREAKING**] TestFramework.extendDescribe now returns a record instead of a describe function
  * changing ```let describe = extendDescribe(...)``` to ```let {describe} = extendDescribe``` should be sufficient for users of this feature
    * needed for future implementation of describeOnly/testOnly
* Allow combining of test libraries via Rely.combine and TestFramework.toLibrary
* Fix bug where skipping snapshot tests would cause snapshots to be deleted
* Improve printing of exceptions with multiple lines of text

# March 19, 2019
### Rely 1.3.1
* Fix bug where expect.string.toMatch would only check matches beginning on first character,
changed to match behavior of Jest and also switched internal implementation from str to re

# March 18, 2019
### Rely 1.3.0
* jUnit reporter
* throw error when nesting top level TestFramework.describe

# March 5, 2019
### Pastel 0.2.1
* Fix bug with windows powershell support

### Console 0.1.0
* Console.Pipe feature
``` foo
|> List.map(mapper)
|> Console.Pipe.log
|> Array.of_list
```

# February 25, 2019
### file-context-printer 0.0.3
* Shorten paths for windows relocatable build support

# February 19, 2019
### Pastel 0.2.0
* Windows powershell support for colors
* Infer default mode to account for non-tty terminals and the FORCE_COLOR environment variable
### Rely 1.2.0
* Mock functions
  * Created via Mock.mock3(implementation)
  * new mock matchers
    * expect.mock(myMock).toBeCalled()
* describeSkip
* Polymorphic equality matchers (great for record comparison)
  * overwriteable equality (structural by default) expect.equal(~equals=?, thing1, thing2)
  * referential equality expect.same(thing1, thing2)
* Better default output in non-tty environment
* Public reporters API
  * Expose onTestSuiteStart, onTestSuiteEnd, onRunStart, onRunComplete test lifecycle methods
  * Allow registering custom reporters
* Update default reporter to include timing data

# January 29, 2019
### Rely 1.1.0
* Collection matchers
  * expect.list
  * expect.array
* testSkip
  * new field added to the record passed to the function passed to describe
    * this may cause additional warning 9's
* skipped tests are not run

### Pastel 0.1.0
* Added support for modes
  * Terminal
    * previous behavior
  * HumanReadable
    * (```<Pastel color=red>"HelloWorld"</Pastel> => "<red>HelloWorld</red>"```)
  * Disabled
    * (```<Pastel color=red>"HelloWorld"</Pastel> => "HelloWorld"```)
  * Pastel.getMode
  * Pastel.setMode
  * Pastel.useMode(mode, thunk)
    * saves the current mode, sets the mode to the desired mode, executes the thunk, restores the saved mode
  * Pastel.Make
    * functor to allow sandboxing of Pastel instances now that mode is a "global" setting

# 0.0.1 (November, 2018)

### Initial Release

* Initial code release. See documentation for usage.
