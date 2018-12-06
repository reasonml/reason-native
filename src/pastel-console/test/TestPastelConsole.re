/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

 type user = {
  name: string,
  age: int,
};

let runTest = () => {
  Console.log("");

  Console.log(<Pastel bold=true> <Pastel> "Primitive types:" </Pastel> </Pastel>);
  Console.log(<Pastel dim=true> ">  Console.log(4.6);" </Pastel>);
  Console.log(4.6);
  Console.log("");
  Console.log(
    <Pastel dim=true> ">  Console.log(\"This is a string\");" </Pastel>,
  );
  Console.log("This is a string");
  Console.log("");
  Console.log(<Pastel dim=true> ">  Console.log(a => a + 1);" </Pastel>);

  Console.log(a => a + 1);

  Console.log("");
  Console.log("");
  Console.log(<Pastel bold=true> "Lists:" </Pastel>);
  Console.log(
    <Pastel dim=true>
      ">  Console.log([\"this\", \"is\", \"a\", \"list\", \"of\", \"strings\"]"
    </Pastel>,
  );
  Console.log(["this", "is", "a", "list", "of", "strings"]);
  Console.log([4.9, 5.0, 999.99]);

  Console.log("");
  Console.log("");
  Console.log(<Pastel bold=true> "Records:" </Pastel>);
  Console.log(
    <Pastel dim=true> ">  type user = {name: string, age: int};" </Pastel>,
  );
  Console.log(
    <Pastel dim=true> ">  Console.log({name: \"joe\", age: 100});" </Pastel>,
  );
  Console.log({name: "joe", age: 100});
  Console.log("");
  Console.log(
    <Pastel dim=true>
      ">  Console.log([{name: \"joe\", age: 100}, {name: \"sue\", age: 18}]);"
    </Pastel>,
  );
  Console.log([{name: "joe", age: 100}, {name: "sue", age: 18}]);

  Console.log("");
  Console.log("");
  Console.log(<Pastel bold=true> "Lists of closures:" </Pastel>);
  Console.log(<Pastel dim=true> ">  Console.log([a=>1, b=>2, c=>3]" </Pastel>);
  Console.log([_a => 1, _b => 2, _c => 3]);

  Console.log("");
  Console.log("");
  Console.log(<Pastel bold=true> "Array of strings:" </Pastel>);
  Console.log(
    <Pastel dim=true> ">  Console.log([|\"a\", \"b\", \"c\"|]" </Pastel>,
  );
  Console.log([|"a", "b", "c"|]);

  Console.log("");
  Console.log("");
  Console.log(<Pastel bold=true> "Array of floats:" </Pastel>);
  Console.log(
    <Pastel dim=true> ">  Console.log([|\"2.3\", \"8.9\", \"9.0\"|]" </Pastel>,
  );
  Console.log([|2.3, 8.9, 9.0|]);

  Console.log("");
  Console.log("");
  Console.log(<Pastel bold=true> "Max Depth:" </Pastel>);
  Console.ObjectPrinter.setMaxDepth(3);
  Console.log(
    <Pastel dim=true> ">  Console.error([[[[[2]]], [[[]]]]], [])" </Pastel>,
  );
  let rec maxDepthHit = [[[[2]]], [[[]]], [[]]];
  Console.error(maxDepthHit);

  Console.log("");
  Console.log("");
  Console.log(<Pastel bold=true> "Max Length:" </Pastel>);
  Console.ObjectPrinter.setMaxLength(8);

  Console.log(
    <Pastel dim=true>
      {|> let rec circularList = [
>   "circular",
>   "list",
>   "should",
>   "not",
>   "error",
>   "but",
>   "detect",
>   "length-exceeded",
>   ...circularList,
> ];
> Console.error(circularList);
    |}
    </Pastel>,
  );
  let rec circularList = [
    "circular",
    "list",
    "should",
    "not",
    "error",
    "but",
    "detect",
    "length-exceeded",
    ...circularList,
  ];
  Console.error(circularList);
  Console.log("");
  Console.log("");
};

Console.log(<Pastel bold=true> "Console logs without enhancements" </Pastel>);
Console.log(<Pastel bold=true> "=================================" </Pastel>);
runTest();
Console.log(
  <Pastel bold=true>
    "Console logs "
    <Pastel color=Green> "with" </Pastel>
    " enhancements"
  </Pastel>,
);
Console.log(<Pastel bold=true> "==============================" </Pastel>);
PastelConsole.init();
Console.ObjectPrinter.setPrintWidth(120);
runTest();
