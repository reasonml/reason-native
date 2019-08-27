/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open PastelInternal;

include Mode;
include Decorators;
include ColorName;
module ColorName = ColorName;

module Make = PastelFactory.Make;
include PastelFactory.Make({});
