/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * This file is generated, do not modify it manually.
 *
 * - Modify unicode-config/UnicodeConfig.re
 * - Run `scripts/re/unicode-gen/run`
 *   - (script is internal only for now, we can help in the PR) 
 * - Then submit your changes
 *
 * @generated SignedSource<<0bfd79a265c60f3be7fdf6b3458854e0>>
 */;

let u = n => {
  let char = Uchar.of_int(n);
  let buffer = Buffer.create(1);
  Buffer.add_utf_8_uchar(buffer, char);
  Buffer.contents(buffer);
};
