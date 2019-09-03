/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
let colWidth = {contents: 70};

let maxDepth = {contents: 70};

let maxLength = {contents: 20};

include ObjectPrinterTypes.ObjectPrinter;

let setPrintWidth = w => colWidth.contents = w;

let setMaxDepth = d => maxDepth.contents = d;

let setMaxLength = l => {
  assert(l > 2);
  maxLength.contents = l;
};

/**
 * TODO: You can rule out lists (which you know are homogeneous in contents)
 * by examining if one but not all of the contents are (strings, floats, or
 * other special types). If something appears to be a list apart from the
 * discovered heterogeneous content, we can print something called
 * "list-like".
 *
 * ~maxLength: Length to examine before making a decision. Must correspond to
 * the printing max length.
 */
external _out: string => unit = "native_out";
let rec detectList: type o. (~maxLength: int, o) => bool =
  (~maxLength, o) =>
    if (maxLength === 0) {
      true;
    } else {
      let magicO = Obj.magic(o);
      let tag = Obj.tag(magicO);
      tag === Obj.string_tag ? false
        : tag === Obj.int_tag
            ? magicO === Obj.repr([])
            : {
              let size = Obj.size(magicO);
              tag === Obj.first_non_constant_constructor_tag
              && size === 2
              && detectList(~maxLength=maxLength - 1, Obj.field(magicO, 1));
            };
    };

/**
 * Returns (didTruncate, extractedList).
 */
let rec extractList = (~maxNum, o) =>
  if (maxNum === 0) {
    (!Obj.is_int(o), []);
  } else if (Obj.is_int(o)) {
    (false, []);
  } else {
    let (restWasTruncated, rest) =
      extractList(~maxNum=maxNum - 1, Obj.field(o, 1));
    (restWasTruncated, [Obj.field(o, 0), ...rest]);
  };

let extractFields = (~maxNum, o) => {
  let rec extractFields = (~maxNum, fieldsSoFar, numFields) =>
    if (maxNum === 0) {
      (numFields > 0, fieldsSoFar);
    } else if (numFields === 0) {
      (false, fieldsSoFar);
    } else {
      extractFields(
        ~maxNum=maxNum - 1,
        [Obj.field(o, numFields - 1), ...fieldsSoFar],
        numFields - 1,
      );
    };
  extractFields(~maxNum, [], Obj.size(o));
};
let getBreakData = itms => {
  let (allItemsLen, someChildBroke) =
    List.fold_left(
      ((curTotalLen, curDidBreak), itm) => {
        let containsNewline = String.contains(itm, '\n');
        (
          curTotalLen + String.length(itm) + 2, /* Add two for comma+space */
          curDidBreak || containsNewline,
        );
      },
      (0, false),
      itms,
    );
  (allItemsLen, someChildBroke);
};

let rec indentForDepth = n =>
  switch (n) {
  | 0 => ""
  | 1 => "  "
  | 2 => "    "
  | 3 => "      "
  | 4 => "        "
  | 5 => "          "
  | 6 => "            "
  | 7 => "              "
  | 8 => "                "
  | _ => indentForDepth(n - 1) ++ "  "
  };

let printTreeShape = (pair, self, ~depth, o: Obj.t) => {
  let (left, right) = pair;
  let (wasTruncated, lst) = extractFields(~maxNum=maxLength.contents, o);
  let dNext = 1 + depth;
  let indent = indentForDepth(depth);
  let indentNext = indentForDepth(dNext);
  let itms =
    List.map(o => self.polymorphicPrint(self, ~depth=dNext, o), lst);
  let (allItemsLen, someChildBroke) = getBreakData(itms);
  if (String.length(indent)
      + 2
      + allItemsLen >= colWidth.contents
      || someChildBroke) {
    let truncationMsg =
      wasTruncated ? ",\n" ++ indentNext ++ self.maxLengthExceeded(self) : "";
    left
    ++ "\n"
    ++ indentNext
    ++ String.concat(",\n" ++ indentNext, itms)
    ++ truncationMsg
    ++ "\n"
    ++ indent
    ++ right;
  } else {
    let truncationMsg =
      wasTruncated ? ", " ++ self.maxLengthExceeded(self) : "";
    left ++ String.concat(", ", itms) ++ truncationMsg ++ right;
  };
};

let printListShape = (self, ~depth, o) => {
  let (wasTruncated, lst) = extractList(~maxNum=maxLength.contents, o);
  let dNext = 1 + depth;
  let indent = indentForDepth(depth);
  let indentNext = indentForDepth(dNext);
  let itms =
    List.map(o => self.polymorphicPrint(self, ~depth=dNext, o), lst);
  /* Very crude, probably wrong - breaking logic */
  let (allItemsLen, someChildBroke) = getBreakData(itms);
  /* +2 for brackets, + len(s) */
  if (String.length(indent)
      + 2
      + allItemsLen >= colWidth.contents
      || someChildBroke) {
    let truncationMsg =
      wasTruncated ? ",\n" ++ indentNext ++ self.maxLengthExceeded(self) : "";
    "["
    ++ "\n"
    ++ indentNext
    ++ String.concat(",\n" ++ indentNext, itms)
    ++ truncationMsg
    ++ "\n"
    ++ indent
    ++ "]";
  } else {
    let truncationMsg =
      wasTruncated ? ", " ++ self.maxLengthExceeded(self) : "";
    "[" ++ String.concat(", ", itms) ++ truncationMsg ++ "]";
  };
};

let base = {
  int: (_self, i) => string_of_int(i),
  string: (_self, s) => s,
  quotedString: (self, s) => "\"" ++ self.string(self, s) ++ "\"",
  float: (_self, f) => string_of_float(f),
  maxDepthExceeded: self => "@max-depth",
  maxLengthExceeded: self => "@max-length",
  unknown: (_self, _o: Obj.t) => "~unknown",
  lazy_: (_self, _o: Obj.t) => "~lazy",
  closure: (_self, f: Obj.t) =>
    "closure(" ++ string_of_int(0 + Obj.magic(Obj.repr(f))) ++ ")",
  doubleArray: (self, ~depth=0, o: Obj.t) =>
    printTreeShape(("[|", "|]"), self, ~depth, o),
  block: (self, ~depth=0, o) => printTreeShape(("{", "}"), self, ~depth, o),
  list: (self, ~depth=0, o) => printListShape(self, ~depth, o),
  polymorphicPrint: (self, ~depth=0, o) =>
    if (depth > maxDepth.contents) {
      self.maxDepthExceeded(self);
    } else {
      let oDynamic: Obj.t = Obj.magic(o);
      let tag = Obj.tag(oDynamic);
      if (tag === Obj.string_tag) {
        depth === 0 ?
          self.string(self, Obj.magic(o)) :
          self.quotedString(self, Obj.magic(o));
      } else if (tag === Obj.int_tag) {
        self.int(self, Obj.magic(o));
      } else if (tag === Obj.double_tag) {
        self.float(self, Obj.magic(o));
      } else if (tag === Obj.closure_tag) {
        self.closure(self, Obj.magic(o));
      } else if (tag === Obj.double_array_tag) {
        self.doubleArray(self, Obj.magic(o));
      } else if (tag === Obj.lazy_tag) {
        self.lazy_(self, Obj.magic(o));
      } else if (detectList(~maxLength=maxLength.contents, o)) {
        self.list(self, ~depth, Obj.magic(o));
      } else if (Obj.is_block(oDynamic)) {
        self.block(self, ~depth, Obj.magic(o));
      } else {
        /* Some kind a block. */
        self.unknown(self, Obj.magic(o));
      };
    },
};
