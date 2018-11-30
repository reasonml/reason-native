/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
let maxStacklength = 1000;

let getStackTraceLines = backtrace =>
  Printexc.raw_backtrace_to_string(backtrace) |> String.split_on_char('\n');

let getCallstack = () => Printexc.get_callstack(maxStacklength);

type stackTrace = string;

type stackUtils = {
  getStackTrace: unit => stackTrace,
  stackTraceToString: stackTrace => string,
  getTopFrame: unit => string,
  getTopLocation: unit => option(Printexc.location),
};

type stackUtilsConfig = {
  exclude: list(Str.regexp),
  baseDir: string,
};

let filterBacktrace = (backtrace, filter) =>
  backtrace
  |> Printexc.backtrace_slots
  |> (
    optSlots =>
      switch (optSlots) {
      | Some(slots) =>
        Some(
          Array.fold_left(
            (acc, slot: Printexc.backtrace_slot) =>
              filter(slot) ? acc @ [slot] : acc,
            [],
            slots,
          ),
        )
      | None => None
      }
  );

let whenSome = (yes, opt) =>
  switch (opt) {
  | Some(value) => Some(yes(value))
  | None => None
  };

let whenNone = (no, opt) =>
  switch (opt) {
  | Some(value) => value
  | None => no
  };

let hasMatchingSubstring = (s, re) =>
  switch (Str.search_forward(re, s, 0)) {
  | index => true
  | exception Not_found => false
  };

let slotMatchesAny = (expressions, slot) =>
  switch (Printexc.Slot.location(slot)) {
  | Some(location) =>
    List.exists(hasMatchingSubstring(location.filename), expressions)
  | None => false
  };

let getTopSlot = (predicate, backtrace) =>
  backtrace
  |> Printexc.backtrace_slots
  |> whenSome(slots =>
       Array.to_list(slots) |> List.find(slot => predicate(slot))
     );

let formatLocation = (baseDir: string, loc: Printexc.location): string => {
  let path = Filename.concat(baseDir, loc.filename);
  let message =
    path
    ++ ":"
    ++ string_of_int(loc.line_number)
    ++ " "
    ++ string_of_int(loc.start_char)
    ++ "-"
    ++ string_of_int(loc.end_char);
  message;
};

let formatSlot = (slot, baseDir) =>
  whenSome(formatLocation(baseDir), Printexc.Slot.location(slot))
  |> whenNone("");

let makeStackUtils = (config: stackUtilsConfig): stackUtils => {
  let filters = config.exclude |> List.append([Str.regexp("StackUtils.re")]);

  let getStackTrace = () =>
    getCallstack()
    |> getStackTraceLines
    |> List.filter(line =>
         !List.exists(hasMatchingSubstring(line), filters)
       )
    |> String.concat("\n");

  let getTopFrame = () =>
    getCallstack()
    |> getTopSlot(slot => !slotMatchesAny(filters, slot))
    |> (
      opt =>
        switch (opt) {
        | Some(line) => formatSlot(line, config.baseDir)
        | None => ""
        }
    );

  let getTopLocation = () =>
    getCallstack()
    |> getTopSlot(slot => !slotMatchesAny(filters, slot))
    |> (
      opt =>
        switch (opt) {
        | Some(slot) =>
          switch (Printexc.Slot.location(slot)) {
          | Some(location) => Some(location)
          | None => None
          }
        | None => None
        }
    );

  {getStackTrace, getTopFrame, stackTraceToString: s => s, getTopLocation};
};
