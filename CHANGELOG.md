# February 19, 2019
### Pastel 0.2.0
* Windows powershell support for colors
* Infer default mode to account for non-tty terminals and the FORCE_COLOR environment variable

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
