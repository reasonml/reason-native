/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 *
 * LICENSE file in the root directory of this source tree.
 */;
open Collections;

type difference =
  | Old(string)
  | New(string)
  | Both(string);

type operation =
  | Delete
  | Insert
  | Move(int, int);

type counter =
  | Zero
  | One
  | Many;

type tableEntry = {
  key: string,
  mutable oCounter: counter,
  mutable nCounter: counter,
  mutable oTokenNumber: option(int),
};

type arrayEntry =
  | PosInOther(int)
  | TableRef(tableEntry);

let incrememntCounter = (counter: counter) : counter =>
  switch (counter) {
  | Zero => One
  | One => Many
  | Many => Many
  };

let split = (splitRe: Str.regexp, s: string) : array(string) =>
  s
  |> Str.full_split(splitRe)
  |> List.map(part =>
       switch (part) {
       | Str.Delim(value) => value
       | Str.Text(value) => value
       }
     )
  |> Array.of_list;

let diff =
    (~delimeter: Str.regexp, old: string, new_: string)
    : list(difference) => {
  /*
   * Throughout the algorithm the comments walk through example of:
   * `diff("foo apple foo orange", "foo orange foo apple")`
   *
   * Note: There would be spaces in here too, we leave them out to make
   * the example simpler.
   */
  /* ["foo", "apple", "foo", "orange"] */
  let oTokens: array(string) = split(delimeter, old);
  /* ["foo", "orange", "foo", "apple"] */
  let nTokens: array(string) = split(delimeter, new_);
  let oLen = Array.length(oTokens);
  let nLen = Array.length(nTokens);
  let table: ref(StringMap.t(tableEntry)) = ref(StringMap.empty);
  /* First pass */
  let nArray: array(arrayEntry) =
    Array.mapi(
      (index, token) => {
        if (! StringMap.has(token, table^)) {
          table :=
            StringMap.set(
              token,
              {
                key: token,
                oCounter: Zero,
                nCounter: Zero,
                oTokenNumber: None,
              },
              table^,
            );
        };
        let entry = StringMap.getOpt(token, table^) |> Option.valuex;
        entry.nCounter = incrememntCounter(entry.nCounter);
        TableRef(entry);
      },
      nTokens,
    );
  /*
   * After pass 1 table is now:
   *
   * {
   *   "foo": {o: 0, n: many, olno: _}
   *   "orange": {o: 0, n: 1, olno: _}
   *   "apple": {o: 0, n: 1, olno: _}
   * }
   *
   * NA is: [ref("foo"), ref("orange"), ref("foo"), ref("apple")]
   * OA is: []
   */
  /* Second pass */
  let oArray: array(arrayEntry) =
    Array.mapi(
      (index, token) => {
        if (! StringMap.has(token, table^)) {
          table :=
            StringMap.set(
              token,
              {
                key: token,
                oCounter: Zero,
                nCounter: Zero,
                oTokenNumber: None,
              },
              table^,
            );
        };
        let entry = StringMap.getOpt(token, table^) |> Option.valuex;
        entry.oCounter = incrememntCounter(entry.oCounter);
        entry.oTokenNumber = Some(index);
        TableRef(entry);
      },
      oTokens,
    );
  /*
   * After pass 2 table is now:
   *
   * {
   *   "foo": {o: 2, n: many, olno: 2}
   *   "orange": {o: 1, n: 1, olno: 3}
   *   "apple": {o: 1, n: 1, olno: 1}
   * }
   *
   * NA is: [ref("foo"), ref("orange"), ref("foo"), ref("apple")]
   * OA is: [ref("foo"), ref("apple"), ref("foo"), ref("orange")]
   */
  /* Third pass */
  let nArray: array(arrayEntry) =
    Array.mapi(
      (index, arrayEntry) => {
        ();
        switch (arrayEntry) {
        /*
         * Note that if oCounter is one, logically oTokenNumber must have
         * Some value set.
         */
        | TableRef({oCounter: One, nCounter: One, oTokenNumber: Some(olno)}) =>
          /* Set OA[olno] to i */
          oArray[olno] = PosInOther(index);
          /* Set NA[i] to OLNO */
          PosInOther(olno);
        | _ => arrayEntry
        };
      },
      nArray,
    );
  /*
   * After pass 3 table is now:
   *
   * {
   *   "foo": {o: 2, n: many, olno: 2}
   *   "orange": {o: 1, n: 1, olno: 3}
   *   "apple": {o: 1, n: 1, olno: 1}
   * }
   *
   * NA is: [ref("foo"), token(3), ref("foo"), token(1)]
   * OA is: [ref("foo"), token(3), ref("foo"), token(1)]
   */
  /* Fourth pass */
  for (i in 0 to nLen - 1) {
    switch (nArray[i]) {
    | PosInOther(j) =>
      if (i + 1 < nLen && j + 1 < oLen) {
        switch (nArray[i + 1], oArray[j + 1]) {
        | (TableRef({key: nKey}), TableRef({key: oKey})) when nKey == oKey =>
          nArray[i + 1] = PosInOther(j + 1);
          oArray[j + 1] = PosInOther(i + 1);
        | _ => ()
        };
      }
    | _ => ()
    };
  };
  /*
   * After pass 4 table is now:
   *
   * {
   *   "foo": {o: 2, n: many, olno: 2}
   *   "orange": {o: 1, n: 1, olno: 3}
   *   "apple": {o: 1, n: 1, olno: 1}
   * }
   *
   * NA is: [ref("foo"), token(3), ref("foo"), token(1)]
   * OA is: [ref("foo"), token(3), ref("foo"), token(1)]
   */
  /* Fifth pass */
  for (i in nLen - 1 downto 0) {
    switch (nArray[i]) {
    | PosInOther(j) =>
      if (i - 1 >= 0 && j - 1 >= 0) {
        switch (nArray[i - 1], oArray[j - 1]) {
        | (TableRef({key: nKey}), TableRef({key: oKey})) when nKey == oKey =>
          nArray[i - 1] = PosInOther(j - 1);
          oArray[j - 1] = PosInOther(i - 1);
        | _ => ()
        };
      }
    | _ => ()
    };
  };
  /*
   * After pass 5 table is now:
   *
   * {
   *   "foo": {o: 2, n: many, olno: 2}
   *   "orange": {o: 1, n: 1, olno: 3}
   *   "apple": {o: 1, n: 1, olno: 1}
   * }
   *
   * NA is: [token(2), token(3), token(0), token(1)]
   * OA is: [token(2), token(3), token(0), token(1)]
   */
  /*
   * Now we construct the differences from the data we have. There are a few
   * things to note. If NA has a TableRef it will always be an Insert, if
   * OA has a TableRef it will always be a Delete. If they have a pointer to
   * a token in the other array you can think of it as a Move operation.
   *
   * The move operation is a bit tricky. Consider `diff("x y", "y x");`. This is
   * just a simple swap that would be composed entirely of "Move" operations as
   * described above. However to communicate the diff via Old, New, Both chunks
   * we must have some Insert and Delete operations. The trivial solution would
   * be to always Insert/Delete and we get:
   *
   *   ```
   *   [Old(x), New(y), Old(y), New(x)]
   *   ```
   * Clearly we can do better with either of these solutions:
   *
   *   ```
   *   [Old(x), Both(y), New(x)]
   *   [New(y), Both(x), Old(y)]
   *   ```
   * But how do we pick one? First draw lines between the two arrays
   * corresponding to the moves. This should result in a bipartite graph. In
   * order to pick a valid set of "Both" chunks we want to pick the largest set
   * of edges in the pipartite graph that has no intersections.
   *
   * This boils down to longest increasing sub-sequence, which we will implement
   * below. (explanation: https://stackoverflow.com/a/4772518)
   */
  let hasMove = ref(false);
  let oProcessed: array(operation) =
    Array.mapi(
      (j, o) =>
        switch (o) {
        | TableRef(_) => Delete
        /* We will number the "edges" by index in old array, j */
        | PosInOther(i) =>
          hasMove := true;
          Move(j, i);
        },
      oArray,
    );
  let nProcessed: array(operation) =
    Array.mapi(
      (i, o) =>
        switch (o) {
        | TableRef(_) => Insert
        /* We will number the "edges" by index in old array, j */
        | PosInOther(j) =>
          hasMove := true;
          Move(j, i);
        },
      nArray,
    );
  /*
   * Now the edges in oProcessed are already in increasing order, the edges
   * in nProcessed are not. We need to find the longest increasing sub-sequence
   * in nProcessed then update all of the edges not in that sub-sequence.
   *
   * This is the easier O(n^2) solution. (TODO: improve to O(n*log(n)) solution)
   */
  if (hasMove^) {
    let dp = Array.make(nLen, 0);
    /* Prev will be used to re-build the answer */
    let prev = Array.make(nLen, -1);
    /* This is the best place to end */
    let best = ref(0);
    for (i in 0 to nLen - 1) {
      /*
       * Only edges can have a length, we know we will hit one eventually
       * due to the has move check earlier.
       */
      dp[i] = (
        switch (nProcessed[i]) {
        | Move(_, _) => 1
        | _ => 0
        }
      );
      if (dp[i] > 0) {
        for (j in i - 1 downto 0) {
          /*
           * Check if we can improve dp[i], by using the answer to dp[j] + 1. We
           * can only do this if the value at j is less than the value at i to
           * maintain the fact that the subsequence is increasing.
           */
          switch (nProcessed[i], nProcessed[j]) {
          | (Move(edge1, _), Move(edge2, _))
              when edge2 < edge1 && dp[j] + 1 > dp[i] =>
            dp[i] = dp[j] + 1;
            prev[i] = j;
          | _ => ()
          };
        };
        /* Now we check if i is better to end at than the previous best */
        if (dp[i] > dp[best^]) {
          best := i;
        };
      };
    };
    /* We construct a set of the edges that are okay to keep */
    let validEdges = ref(IntSet.empty);
    let getEdge = (index: ref(int)) : int =>
      switch (nProcessed[index^]) {
      | Move(edge, _) => edge
      | _ => failwith("Unreachable: This should only be processing edges.")
      };
    while (prev[best^] !== (-1)) {
      validEdges := IntSet.add(getEdge(best), validEdges^);
      best := prev[best^];
    };
    /* Add the final edge */
    validEdges := IntSet.add(getEdge(best), validEdges^);
    let validEdges = validEdges^;
    /* Then we update each array accordingly */
    for (j in 0 to oLen - 1) {
      switch (oProcessed[j]) {
      | Move(edge, _) when ! IntSet.has(edge, validEdges) =>
        oProcessed[j] = Delete
      | _ => ()
      };
    };
    for (i in 0 to nLen - 1) {
      switch (nProcessed[i]) {
      | Move(edge, _) when ! IntSet.has(edge, validEdges) =>
        nProcessed[i] = Insert
      | _ => ()
      };
    };
  };
  let differences = ref([]);
  let i = ref(0);
  for (j in 0 to oLen - 1) {
    switch (oProcessed[j]) {
    | Insert => failwith("Unreachable: Inserts cannot be from old string.")
    /*
     * If it's a delete, mark it as old.
     */
    | Delete => differences := differences^ @ [Old(oTokens[j])]
    /*
     * If it's a move, then we first make sure to catch up our i pointer
     * to add any inserts we haven't done yet. Then we add a Both entry.
     */
    | Move(j2, i2) =>
      while (i^ < i2) {
        /* TODO: Should we assert that these are all Inserts? They should be. */
        differences := differences^ @ [New(nTokens[i^])];
        incr(i);
      };
      differences := differences^ @ [Both(oTokens[j])];
      incr(i);
    };
  };
  /* Finally we account for any differences at the end we may have missed */
  while (i^ < nLen) {
    differences := differences^ @ [New(nTokens[i^])];
    incr(i);
  };
  let differences = differences^;
  /* Now we collapse duplicate nodes*/
  /* This is a reverse stack, the first element is the top of the stack */
  let stack = ref([]);
  let _ =
    differences
    |> List.map(part => {
         if (List.length(stack^) === 0) {
           stack := [part];
         } else {
           let top = List.hd(stack^);
           let rest = List.tl(stack^);
           switch (top, part) {
           | (Old(top), Old(part)) => stack := [Old(top ++ part)] @ rest
           | (New(top), New(part)) => stack := [New(top ++ part)] @ rest
           | (Both(top), Both(part)) => stack := [Both(top ++ part)] @ rest
           | _ => stack := [part] @ stack^
           };
         };
         ();
       });
  List.rev(stack^);
};

let diffWords = diff(~delimeter=Str.regexp("\\b+"));

let diffLines = diff(~delimeter=Str.regexp("[\n]+"));

let printDiff =
    (
      ~oldFormatter=s => Chalk.(strikethrough(red(s))),
      ~newFormatter=s => Chalk.(underline(green(s))),
      ~bothFormatter=s => Chalk.blackBright(s),
      differences: list(difference),
    )
    : string =>
  differences
  |> List.map(d => {
       let result =
         switch (d) {
         | Old(value) => oldFormatter(value)
         | New(value) => newFormatter(value)
         | Both(value) => bothFormatter(value)
         };
       result;
     })
  |> String.concat("");

let printDiffBg =
  printDiff(
    ~oldFormatter=s => Chalk.(bg.red(black(s))),
    ~newFormatter=s => Chalk.(bg.green(black(s))),
    ~bothFormatter=s => s,
  );

let md5 = string => string |> Digest.string |> Digest.to_hex;
