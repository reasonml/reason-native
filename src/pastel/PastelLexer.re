/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

type token =
  | Starts(string)
  | Stops(string)
  | Text(string);

module Make = (M: {
                 let startRegex: Re.re;
                 let stopRegex: Re.re;
               }) => {
  open M;
  let tokenize = (s: string): list(token) => {
    let parseNonStarts = nonStarts =>
      Re.split_full(stopRegex, nonStarts)
      |> List.map(nonStartSplitResult =>
           switch (nonStartSplitResult) {
           | `Delim(g) => Stops(Re.Group.get(g, 0))
           | `Text(text) => Text(text)
           }
         );
    s
    |> Re.split_full(startRegex)
    |> List.fold_left(
         (tokens, splitResult) =>
           switch (splitResult) {
           | `Delim(g) => tokens @ [Starts(Re.Group.get(g, 0))]
           | `Text(nonStarts) => tokens @ parseNonStarts(nonStarts)
           },
         [],
       );
  };

  let unformattedText = s => {
    List.fold_left(
      (acc, token) =>
        switch (token) {
        | Starts(_)
        | Stops(_) => acc
        | Text(t) => acc @ [t]
        },
      [],
      tokenize(s),
    )
    |> String.concat("");
  };

  let length = s => {
    List.fold_left(
      (acc, part) =>
        switch (part) {
        | Starts(_)
        | Stops(_) => acc
        | Text(t) => acc + String.length(t)
        },
      0,
      tokenize(s),
    );
  };

  let printTokens = tokens => {
    let tokenStr =
      List.map(
        token =>
          switch (token) {
          | Starts(starts) => "Starts(" ++ String.escaped(starts) ++ ")"
          | Stops(stops) => "Stops(" ++ String.escaped(stops) ++ ")"
          | Text(text) => "Text(" ++ String.escaped(text) ++ ")"
          },
        tokens,
      )
      |> String.concat(", ");

    print_endline("tokens: [" ++ tokenStr ++ "]");
  };

  let tokensToStr = tokens =>
    tokens
    |> List.map(token =>
         switch (token) {
         | Starts(s)
         | Stops(s)
         | Text(s) => s
         }
       )
    |> String.concat("");

  /** everything before index is one string, everything else is the other */
  let partition = (index, s) => {
    let tokens = tokenize(s);
    let firstHalfANSI = ref([]);
    let secondHalfANSI = ref([]);
    let partition1 = ref([]);
    let partition2 = ref([]);
    let charsSeen = ref(0);
    ();
    List.iter(
      token =>
        if (charsSeen^ < index) {
          switch (token) {
          | Starts(_)
          | Stops(_) =>
            firstHalfANSI := firstHalfANSI^ @ [token];
            partition1 := partition1^ @ [token];
          | Text(t) =>
            let textLength = String.length(t);
            if (charsSeen^ + textLength < index) {
              partition1 := partition1^ @ [token];
            } else {
              let numToTake = index - charsSeen^;
              let firstPart = String.sub(t, 0, numToTake);
              let secondPart =
                String.sub(
                  t,
                  index - charsSeen^,
                  String.length(t) - numToTake,
                );
              partition1 := partition1^ @ [Text(firstPart)];
              partition2 := firstHalfANSI^ @ [Text(secondPart)];
            };
            charsSeen := charsSeen^ + textLength;
          };
        } else {
          switch (token) {
          | Starts(_)
          | Stops(_) =>
            secondHalfANSI := secondHalfANSI^ @ [token];
            partition2 := partition2^ @ [token];
          | Text(_) => partition2 := partition2^ @ [token]
          };
        },
      tokens,
    );

    partition1 := partition1^ @ secondHalfANSI^;

    (tokensToStr(partition1^), tokensToStr(partition2^));
  };
};
