/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 *
 * LICENSE file in the root directory of this source tree.
 */;

 type user = {
  name: string,
  age: int,
};

let runTest = () => {
  Console.log("");

  Console.log(<Chalk bold=true> <Chalk> "Primitive types:" </Chalk> </Chalk>);
  Console.log(<Chalk dim=true> ">  Console.log(4.6);" </Chalk>);
  Console.log(4.6);
  Console.log("");
  Console.log(
    <Chalk dim=true> ">  Console.log(\"This is a string\");" </Chalk>,
  );
  Console.log("This is a string");
  Console.log("");
  Console.log(<Chalk dim=true> ">  Console.log(a => a + 1);" </Chalk>);

  Console.log(a => a + 1);

  Console.log("");
  Console.log("");
  Console.log(<Chalk bold=true> "Lists:" </Chalk>);
  Console.log(
    <Chalk dim=true>
      ">  Console.log([\"this\", \"is\", \"a\", \"list\", \"of\", \"strings\"]"
    </Chalk>,
  );
  Console.log(["this", "is", "a", "list", "of", "strings"]);
  Console.log([4.9, 5.0, 999.99]);

  Console.log("");
  Console.log("");
  Console.log(<Chalk bold=true> "Records:" </Chalk>);
  Console.log(
    <Chalk dim=true> ">  type user = {name: string, age: int};" </Chalk>,
  );
  Console.log(
    <Chalk dim=true> ">  Console.log({name: \"joe\", age: 100});" </Chalk>,
  );
  Console.log({name: "joe", age: 100});
  Console.log("");
  Console.log(
    <Chalk dim=true>
      ">  Console.log([{name: \"joe\", age: 100}, {name: \"sue\", age: 18}]);"
    </Chalk>,
  );
  Console.log([{name: "joe", age: 100}, {name: "sue", age: 18}]);

  Console.log("");
  Console.log("");
  Console.log(<Chalk bold=true> "Lists of closures:" </Chalk>);
  Console.log(<Chalk dim=true> ">  Console.log([a=>1, b=>2, c=>3]" </Chalk>);
  Console.log([_a => 1, _b => 2, _c => 3]);

  Console.log("");
  Console.log("");
  Console.log(<Chalk bold=true> "Array of strings:" </Chalk>);
  Console.log(
    <Chalk dim=true> ">  Console.log([|\"a\", \"b\", \"c\"|]" </Chalk>,
  );
  Console.log([|"a", "b", "c"|]);

  Console.log("");
  Console.log("");
  Console.log(<Chalk bold=true> "Array of floats:" </Chalk>);
  Console.log(
    <Chalk dim=true> ">  Console.log([|\"2.3\", \"8.9\", \"9.0\"|]" </Chalk>,
  );
  Console.log([|2.3, 8.9, 9.0|]);

  Console.log("");
  Console.log("");
  Console.log(<Chalk bold=true> "Max Depth:" </Chalk>);
  Console.ObjectPrinter.setMaxDepth(3);
  Console.log(
    <Chalk dim=true> ">  Console.error([[[[[2]]], [[[]]]]], [])" </Chalk>,
  );
  let rec maxDepthHit = [[[[2]]], [[[]]], [[]]];
  Console.error(maxDepthHit);

  Console.log("");
  Console.log("");
  Console.log(<Chalk bold=true> "Max Length:" </Chalk>);
  Console.ObjectPrinter.setMaxLength(8);

  Console.log(
    <Chalk dim=true>
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
    </Chalk>,
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

Console.log(<Chalk bold=true> "Console logs without enhancements" </Chalk>);
Console.log(<Chalk bold=true> "=================================" </Chalk>);
runTest();
Console.log(
  <Chalk bold=true>
    "Console logs "
    <Chalk color=Green> "with" </Chalk>
    " enhancements"
  </Chalk>,
);
Console.log(<Chalk bold=true> "==============================" </Chalk>);
ChalkConsole.init();
Console.ObjectPrinter.setPrintWidth(120);
runTest();
