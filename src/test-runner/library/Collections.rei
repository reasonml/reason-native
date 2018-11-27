/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
/**
 * This module defines some core collections that have consistent methods more
 * similar to JavaScript conventions than OCaml/Reason conventions.
 *
 * This collections do not expose method that will ever throw an exception.
 * That behavior is hidden away by returning options or requiring a default
 * for methods that could fail when something is not present in the collection.
 *
 * Mutable versions of the collections are also exposed. They are just a
 * light-weight wrapper around a `ref` of the immutable versions. The
 * collections are not truly optimized to be mutable.
 */
open CollectionTypes;

module Set: {
  module type OrderedType = Core_Set.OrderedType;
  module Make: (Ord: OrderedType) => SetMakeResult with type value = Ord.t;
};

module IntSet: SetMakeResult with type value = int;

module StringSet: SetMakeResult with type value = String.t;

module MutableSet: {
  module type OrderedType = Set.OrderedType;
  module Make:
    (Ord: OrderedType) => MutableSetMakeResult with type value = Ord.t;
};

module MIntSet: MutableSetMakeResult with type value = int;

module MStringSet: MutableSetMakeResult with type value = String.t;

module Map: {
  module type OrderedType = Core_Map.OrderedType;
  module Make: (Ord: OrderedType) => MapMakeResult with type key = Ord.t;
};

module IntMap: MapMakeResult with type key = int;

module StringMap: MapMakeResult with type key = String.t;

module MutableMap: {
  module type OrderedType = Core_Map.OrderedType;
  module Make:
    (Ord: OrderedType) => MutableMapMakeResult with type key = Ord.t;
};

module MIntMap: MutableMapMakeResult with type key = int;

module MStringMap: MutableMapMakeResult with type key = String.t;
