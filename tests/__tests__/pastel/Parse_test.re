/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestFramework;

let runTestSuite = (mode, name) => {
  describe(
    "pastel " ++ name ++ " parsing",
    ({test}) => {
      test(
        "should split string into two regions based on styles", ({expect}) =>
        Pastel.useMode(
          mode,
          () => {
            let input =
              <Pastel color=Green>
                "hello"
                <Pastel color=Red> "world" </Pastel>
              </Pastel>;
            let parts = Pastel.unstable_parse(input);
            expect.int(List.length(parts)).toBe(2);
            let (firstStyle, firstText, secondStyle, secondText) =
              switch (parts) {
              | [(firstStyle, firstText), (secondStyle, secondText)] => (
                  firstStyle,
                  firstText,
                  secondStyle,
                  secondText,
                )
              | _ => raise(Invalid_argument("already verified length"))
              };
            expect.string(firstText).toEqual("hello");
            expect.string(secondText).toEqual("world");
            expect.option(Pastel.getColor(firstStyle)).toBe(Some(Green));
            expect.option(Pastel.getColor(secondStyle)).toBe(Some(Red));
          },
        )
      );
      test(
        "should split string into three regions based on styles", ({expect}) =>
        Pastel.useMode(
          mode,
          () => {
            let input =
              <Pastel color=Green>
                "hello"
                <Pastel color=Red> "world" </Pastel>
                <Pastel color=Blue> "and goodbye" </Pastel>
              </Pastel>;
            let parts = Pastel.unstable_parse(input);

            expect.int(List.length(parts)).toBe(3);
            let (
              firstStyle,
              firstText,
              secondStyle,
              secondText,
              thirdStyle,
              thirdText,
            ) =
              switch (parts) {
              | [
                  (firstStyle, firstText),
                  (secondStyle, secondText),
                  (thirdStyle, thirdText),
                ] => (
                  firstStyle,
                  firstText,
                  secondStyle,
                  secondText,
                  thirdStyle,
                  thirdText,
                )
              | _ => raise(Invalid_argument("already verified length"))
              };
            expect.string(firstText).toEqual("hello");
            expect.string(secondText).toEqual("world");
            expect.string(thirdText).toEqual("and goodbye");
            expect.option(Pastel.getColor(firstStyle)).toBe(Some(Green));
            expect.option(Pastel.getColor(secondStyle)).toBe(Some(Red));
            expect.option(Pastel.getColor(thirdStyle)).toBe(Some(Blue));
          },
        )
      );
      test(
        "should be able to modify string and get back expected regions",
        ({expect}) =>
        Pastel.useMode(
          mode,
          () => {
            let input =
              <Pastel color=Green>
                "hello"
                <Pastel color=Red> "world" </Pastel>
              </Pastel>;
            let parts = Pastel.unstable_parse(input);

            expect.int(List.length(parts)).toBe(2);
            let (firstRegion, secondRegion) =
              switch (parts) {
              | [(firstRegion, firstText), (secondRegion, secondText)] => (
                  firstRegion,
                  secondRegion,
                )
              | _ => raise(Invalid_argument("already verified length"))
              };
            let reconstructed =
              Pastel.unstable_apply([
                (firstRegion, "goodbye"),
                (secondRegion, "everybody"),
              ]);
            let reconstructedParts = Pastel.unstable_parse(reconstructed);

            expect.int(List.length(reconstructedParts)).toBe(2);
            let (firstText, secondText) =
              switch (reconstructedParts) {
              | [(firstRegion, firstText), (secondRegion, secondText)] => (
                  firstText,
                  secondText,
                )
              | _ => raise(Invalid_argument("already verified length"))
              };
            expect.string(firstText).toEqual("goodbye");
            expect.string(secondText).toEqual("everybody");
          },
        )
      );
      test(
        "should be able to modify string, insert empty style, and get back expected regions",
        ({expect}) =>
        Pastel.useMode(
          mode,
          () => {
            let input =
              <Pastel color=Green>
                "hello"
                <Pastel color=Red> "world" </Pastel>
              </Pastel>;
            let parts = Pastel.unstable_parse(input);

            expect.int(List.length(parts)).toBe(2);
            let (firstRegion, secondRegion) =
              switch (parts) {
              | [(firstRegion, firstText), (secondRegion, secondText)] => (
                  firstRegion,
                  secondRegion,
                )
              | _ => raise(Invalid_argument("already verified length"))
              };
            let reconstructed =
              Pastel.unstable_apply([
                (firstRegion, "goodbye"),
                (Pastel.emptyStyle, "unstyled text!"),
                (secondRegion, "everybody"),
              ]);
            let reconstructedParts = Pastel.unstable_parse(reconstructed);

            expect.int(List.length(reconstructedParts)).toBe(3);
            let (firstText, secondText, thirdText, secondStyle) =
              switch (reconstructedParts) {
              | [(_, firstText), (secondStyle, secondText), (_, thirdText)] => (
                  firstText,
                  secondText,
                  thirdText,
                  secondStyle,
                )
              | _ => raise(Invalid_argument("already verified length"))
              };
            expect.string(firstText).toEqual("goodbye");
            expect.string(secondText).toEqual("unstyled text!");
            expect.string(thirdText).toEqual("everybody");
            expect.equal(Pastel.emptyStyle, secondStyle);
          },
        )
      );
      test(
        "Should be able to modify parsed styles and have them persist after application",
        ({expect}) =>
        Pastel.useMode(
          mode,
          () => {
            let input =
              <Pastel color=Green>
                "hello"
                <Pastel color=Red> "world" </Pastel>
              </Pastel>;
            let parts = Pastel.unstable_parse(input);
            expect.int(List.length(parts)).toBe(2);
            let (firstStyle, firstText, secondStyle, secondText) =
              switch (parts) {
              | [(firstStyle, firstText), (secondStyle, secondText)] => (
                  firstStyle,
                  firstText,
                  secondStyle,
                  secondText,
                )
              | _ => raise(Invalid_argument("already verified length"))
              };
            let result =
              Pastel.unstable_apply([
                (firstStyle |> Pastel.withUnderline, firstText),
                (
                  secondStyle |> Pastel.withDim |> Pastel.withColor(Magenta),
                  secondText,
                ),
              ]);
            let resultParts = Pastel.unstable_parse(result);
            expect.int(List.length(resultParts)).toBe(2);
            let (firstStyle, firstText, secondStyle, secondText) =
              switch (resultParts) {
              | [(firstStyle, firstText), (secondStyle, secondText)] => (
                  firstStyle,
                  firstText,
                  secondStyle,
                  secondText,
                )
              | _ => raise(Invalid_argument("already verified length"))
              };
            expect.bool(Pastel.isUnderline(firstStyle)).toBeTrue();
            expect.option(Pastel.getColor(firstStyle)).toBe(Some(Green));
            expect.option(Pastel.getColor(secondStyle)).toBe(
              Some(Magenta),
            );
            expect.bool(Pastel.isDim(secondStyle)).toBeTrue();
          },
        )
      );
    },
  );
};

runTestSuite(Terminal, "terminal");
runTestSuite(HumanReadable, "human readable");

describe("disabled deconstruction", ({test}) => {
  test("parse should return single string and empty style", ({expect}) =>
    Pastel.useMode(
      Disabled,
      () => {
        let input =
          <Pastel color=Green>
            "hello "
            <Pastel color=Red> "world" </Pastel>
          </Pastel>;
        let parts = Pastel.unstable_parse(input);

        expect.int(List.length(parts)).toBe(1);
        let (style, text) = List.hd(parts);

        expect.string(text).toEqual("hello world");
        expect.equal(Pastel.emptyStyle, style);
      },
    )
  );
  test("apply should just concat text", ({expect}) =>
    Pastel.useMode(
      Disabled,
      () => {
        let result =
          Pastel.unstable_apply([
            (Pastel.emptyStyle |> Pastel.withColor(Black), "hello "),
            (Pastel.emptyStyle |> Pastel.withHidden, "world"),
          ]);

        expect.string(result).toEqual("hello world");
      },
    )
  );
});
