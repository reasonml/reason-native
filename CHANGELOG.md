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
