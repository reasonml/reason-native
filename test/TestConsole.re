Console.log("Console.log output of a string will not print quotes");
Console.error("Console.error output of a string will not print quotes");
Console.warn("Console.warn output of a string will not print quotes");
Console.debug("Console.debug output of a string will not print quotes");

Console.out("Console.out of a string will not print quotes, or the newline.");
Console.out(" See how this line continues.");

Console.log("");

Console.log([Some(["console"]), Some(["dot"]), Some(["log"]), None]);

Console.warn([Some(["console"]), Some(["dot"]), Some(["warn"]), None]);

Console.error([Some(["console"]), Some(["dot"]), Some(["error"]), None]);

Console.ObjectPrinter.setMaxDepth(3);

let rec maxDepthHit = [[[[2, 3]]], [[[5, 6]]]];
Console.error(maxDepthHit);

Console.ObjectPrinter.setMaxLength(8);

let rec maxLengthHit = [2, 3, 4, 5, 6];

Console.error(maxLengthHit);

let rec circularList = [
  Some(["circular"]),
  Some(["list"]),
  Some(["should"]),
  Some(["not"]),
  Some(["error"]),
  Some(["but"]),
  Some(["detect"]),
  Some(["length-exceeded"]),
  ...circularList,
];
Console.error(circularList);
