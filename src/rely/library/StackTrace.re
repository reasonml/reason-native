/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
module type StackTrace = {
  type t;
  let getStackTrace: unit => t;
  let getExceptionStackTrace: unit => t;
  let stackTraceToString: (t, int) => string;
  let getTopLocation: t => option(Printexc.location);
  let formatLocation: (Printexc.location, bool) => string;
};

module type Config = {
  let exclude: list(string);
  let baseDir: string;
  let formatLink: string => string;
  let formatText: string => string;
};

let (>>=) = (opt, fn) =>
  switch (opt) {
  | Some(x) => fn(x)
  | None => None
  };

module Make = (Config: Config) => {
  open Printexc;
  let maxStacklength = 1000;
  let filters =
    Config.exclude
    |> List.map(Str.regexp)
    |> List.append([Str.regexp("StackTrace.re")]);

  type t = option(list(backtrace_slot));

  let getTopLocation = optSlots =>
    switch (optSlots) {
    | None => None
    | Some(slots) => Slot.location(List.hd(slots))
    };

  let getCallstack = () => get_callstack(maxStacklength);

  let hasMatchingSubstring = (s, re) =>
    switch (Str.search_forward(re, s, 0)) {
    | _index => true
    | exception Not_found => false
    };

  let formatLocation = (loc: Printexc.location, isInline: bool): string => {
    let path = Filename.concat(Config.baseDir, loc.filename);
    let message =
      String.concat(
        ":",
        [
          path,
          string_of_int(loc.line_number),
          string_of_int(loc.start_char),
        ],
      )
      |> Config.formatLink;

    isInline ? message ++ Config.formatText(" (inlined)") : message;
  };

  let appendSlot = (acc, slot) => {
    let formatArg =
      switch (acc) {
      /* Printexc prints 0-th element different from all others */
      | [] => 0
      | _ => 1
      };
    switch (Printexc.Slot.format(formatArg, slot)) {
    | Some(line) =>
      switch (Printexc.Slot.location(slot)) {
      | Some(loc) =>
        /* replacing default non terminal-clickable file information */
        let fileRegex = Str.regexp("file \".*$");
        let isInline = Printexc.Slot.is_inline(slot);
        let prefix =
          Config.formatText(Str.global_replace(fileRegex, "", line));
        acc @ [String.concat("", [prefix, formatLocation(loc, isInline)])];
      | None => acc
      }
    | None => acc
    };
  };

  let take = (list, n) => {
    let rec takeHelper = (list, n, acc) =>
      n <= 0 ?
        acc :
        (
          switch (list) {
          | [] => acc
          | [hd, ...tl] => takeHelper(tl, n - 1, acc @ [hd])
          }
        );
    takeHelper(list, n, []);
  };

  let stackTraceToString = (optSlots, maxSlots) =>
    switch (optSlots) {
    | None => ""
    | Some(list) =>
      let output =
        List.fold_left((acc, slot) => appendSlot(acc, slot), [], list);

      String.concat("\n", take(output, maxSlots));
    };

  let filterBacktrace = (slots, filter) =>
    Array.fold_left(
      (acc, slot: Printexc.backtrace_slot) =>
        filter(slot) ? acc @ [slot] : acc,
      [],
      slots,
    );

  let slotIsntFiltered = slot =>
    switch (Printexc.Slot.location(slot)) {
    | Some(location) =>
      !List.exists(hasMatchingSubstring(location.filename), filters)
    | None => false
    };

  let getStackTrace = () =>
    getCallstack()
    |> backtrace_slots
    >>= (slots => Some(filterBacktrace(slots, slotIsntFiltered)));

  let getExceptionStackTrace = () =>
    get_raw_backtrace()
    |> backtrace_slots
    >>= (slots => Some(filterBacktrace(slots, slotIsntFiltered)));
};
