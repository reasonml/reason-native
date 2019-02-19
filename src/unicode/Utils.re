/**
 * Copyright 2004-present Facebook. AllRights Reserved.
 *
 * This file is generated, do not modify it manually.
 *
 * - Modify UnicodeGen.re
 * - Then run `scripts/re/unicode-gen/run`
 *
 * @generated SignedSource<<cec5194c1a67dfffbbe4c777ba322616>>
 * @emails oncall+reason
 */;

let u = n => {
  let char = Uchar.of_int(n);
  let buffer = Buffer.create(1);
  Buffer.add_utf_8_uchar(buffer, char);
  Buffer.contents(buffer);
};
