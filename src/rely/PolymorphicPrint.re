/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open ObjectPrinter;

let objectPrinter = ObjectPrinter.base;

let print = o => objectPrinter.polymorphicPrint(objectPrinter, o);
