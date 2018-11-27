/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 *
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
module Core_Set = Set;

module Set = {
  module type OrderedType = Core_Set.OrderedType;
  module Make = (Ord: OrderedType) => {
    module Set_Impl = Core_Set.Make(Ord);
    /* The types */
    type t = Set_Impl.t;
    type value = Set_Impl.elt;
    /* Non-standard creation */
    let empty: t = Set_Impl.empty;
    let fromList: list(value) => t = Set_Impl.of_list;
    let fromArray: array(value) => t =
      arr => arr |> Array.to_list |> Set_Impl.of_list;
    /* Non-standard conversion */
    let toList: t => list(value) = Set_Impl.elements;
    let toArray: t => array(value) = set => set |> toList |> Array.of_list;
    /* Standard JavaScript Set methods */
    let size: t => int = Set_Impl.cardinal;
    let add: (value, t) => t = Set_Impl.add;
    let clear: t => t = (_) => Set_Impl.empty;
    let delete: (value, t) => t = Set_Impl.remove;
    let entries: t => list((value, value)) =
      set => set |> toList |> List.map(el => (el, el));
    let forEach: (value => unit, t) => t =
      (fn, set) => {
        Set_Impl.iter(fn, set);
        set;
      };
    let has: (value, t) => bool = Set_Impl.mem;
    let values: t => list(value) = toList;
    /* Non-standard JavaScript methods - Similar to JavaScript array */
    let every: (value => bool, t) => bool = Set_Impl.for_all;
    let filter: (value => bool, t) => t = Set_Impl.filter;
    let map: (value => value, t) => t =
      (fn, set) => set |> values |> List.map(fn) |> fromList;
    let reduce: (('acc, value) => 'acc, 'acc, t) => 'acc =
      (fn, initial, set) =>
        Set_Impl.fold((value, acc) => fn(acc, value), set, initial);
    let some: (value => bool, t) => bool = Set_Impl.exists;
  };
};

module IntSet =
  Set.Make(
    {
      type t = int;
      let compare = compare;
    },
  );

module StringSet = Set.Make(String);

module MutableSet = {
  module type OrderedType = Set.OrderedType;
  module Make = (Ord: OrderedType) => {
    module Set_Impl = Set.Make(Ord);
    /* The types */
    type t = ref(Set_Impl.t);
    type value = Set_Impl.value;
    /* Non-standard creation */
    let empty: unit => t = () => ref(Set_Impl.empty);
    let fromList: list(value) => t = list => ref(Set_Impl.fromList(list));
    let fromArray: array(value) => t = arr => ref(Set_Impl.fromArray(arr));
    /* Non-standard conversion */
    let toList: t => list(value) = set => Set_Impl.toList(set^);
    let toArray: t => array(value) = set => Set_Impl.toArray(set^);
    /* Standard JavaScript Set methods */
    let size: t => int = set => Set_Impl.size(set^);
    let add: (value, t) => unit =
      (value, set) => {
        set := Set_Impl.add(value, set^);
        ();
      };
    let clear: t => unit =
      set => {
        set := Set_Impl.clear(set^);
        ();
      };
    let delete: (value, t) => unit =
      (value, set) => {
        set := Set_Impl.delete(value, set^);
        ();
      };
    let entries: t => list((value, value)) = set => Set_Impl.entries(set^);
    let forEach: (value => unit, t) => unit =
      (fn, set) => {
        let _ = Set_Impl.forEach(fn, set^);
        ();
      };
    let has: (value, t) => bool = (value, set) => Set_Impl.has(value, set^);
    let values: t => list(value) = toList;
    /* Non-standard JavaScript methods - Similar to JavaScript array */
    let every: (value => bool, t) => bool =
      (fn, set) => Set_Impl.every(fn, set^);
    let filter: (value => bool, t) => unit =
      (fn, set) => {
        set := Set_Impl.filter(fn, set^);
        ();
      };
    let map: (value => value, t) => unit =
      (fn, set) => {
        set := Set_Impl.map(fn, set^);
        ();
      };
    let reduce: (('acc, value) => 'acc, 'acc, t) => 'acc =
      (fn, initial, set) => Set_Impl.reduce(fn, initial, set^);
    let some: (value => bool, t) => bool =
      (fn, set) => Set_Impl.some(fn, set^);
  };
};

module MIntSet =
  MutableSet.Make(
    {
      type t = int;
      let compare = compare;
    },
  );

module MStringSet = MutableSet.Make(String);

module Core_Map = Map;

module Map = {
  module type OrderedType = Core_Map.OrderedType;
  module Make = (Ord: OrderedType) => {
    module Map_Impl = Core_Map.Make(Ord);
    /* The types */
    type t(+'value) = Map_Impl.t('value);
    type key = Map_Impl.key;
    /* Non-standard creation */
    let empty: t('value) = Map_Impl.empty;
    let fromList: list((key, 'value)) => t('value) =
      list =>
        List.fold_left(
          (acc, (key, value)) => Map_Impl.add(key, value, acc),
          empty,
          list,
        );
    let fromArray: array((key, 'value)) => t('value) =
      arr => arr |> Array.to_list |> fromList;
    /* Non-standard conversion */
    let toList: t('value) => list((key, 'value)) = Map_Impl.bindings;
    let toArray: t('value) => array((key, 'value)) =
      map => map |> Map_Impl.bindings |> Array.of_list;
    /* Standard JavaScript Set methods */
    let size: t('value) => int = Map_Impl.cardinal;
    let clear: t('value) => t('value) = (_) => Map_Impl.empty;
    let delete: (key, t('value)) => t('value) = Map_Impl.remove;
    let entries: t('value) => list((key, 'value)) = toList;
    let forEach: (('value, key) => unit, t('value)) => t('value) =
      (fn, map) => {
        Map_Impl.iter((key, value) => fn(value, key), map);
        map;
      };
    let get: (key, ~default: 'value, t('value)) => 'value =
      (key, ~default, map) =>
        try (Map_Impl.find(key, map)) {
        | Not_found => default
        };
    let getOpt: (key, t('value)) => option('value) =
      (key, map) =>
        try (Some(Map_Impl.find(key, map))) {
        | Not_found => None
        };
    let has: (key, t('value)) => bool = Map_Impl.mem;
    let keys: t('value) => list(key) =
      map => map |> toList |> List.map(((key, value)) => key);
    let set: (key, 'value, t('value)) => t('value) = Map_Impl.add;
    let values: t('value) => list('value) =
      map => map |> toList |> List.map(((key, value)) => value);
    /* Non-standard JavaScript methods - Similar to JavaScript array */
    let every: (('value, key) => bool, t('value)) => bool =
      (fn, map) => Map_Impl.for_all((key, value) => fn(value, key), map);
    let filter: (('value, key) => bool, t('value)) => t('value) =
      (fn, map) => Map_Impl.filter((key, value) => fn(value, key), map);
    let map: (('value, key) => 'nextValue, t('value)) => t('nextValue) =
      (fn, map) => Map_Impl.mapi((key, value) => fn(value, key), map);
    let reduce: (('acc, 'value, key) => 'acc, 'acc, t('value)) => 'acc =
      (fn, initial, map) =>
        Map_Impl.fold(
          (key, value, acc) => fn(acc, value, key),
          map,
          initial,
        );
    let some: (('value, key) => bool, t('value)) => bool =
      (fn, map) => Map_Impl.exists((key, value) => fn(value, key), map);
  };
};

module IntMap =
  Map.Make(
    {
      type t = int;
      let compare = compare;
    },
  );

module StringMap = Map.Make(String);

module MutableMap = {
  module type OrderedType = Core_Map.OrderedType;
  module Make = (Ord: OrderedType) => {
    module Map_Impl = Map.Make(Ord);
    /* The types */
    type t('value) = ref(Map_Impl.t('value));
    type key = Map_Impl.key;
    /* Non-standard creation */
    let empty: unit => t('value) = () => ref(Map_Impl.empty);
    let fromList: list((key, 'value)) => t('value) =
      list => ref(Map_Impl.fromList(list));
    let fromArray: array((key, 'value)) => t('value) =
      arr => ref(Map_Impl.fromArray(arr));
    /* Non-standard conversion */
    let toList: t('value) => list((key, 'value)) =
      map => Map_Impl.toList(map^);
    let toArray: t('value) => array((key, 'value)) =
      map => Map_Impl.toArray(map^);
    /* Standard JavaScript Set methods */
    let size: t('value) => int = map => Map_Impl.size(map^);
    let clear: t('value) => unit =
      map => {
        map := Map_Impl.clear(map^);
        ();
      };
    let delete: (key, t('value)) => unit =
      (key, map) => {
        map := Map_Impl.delete(key, map^);
        ();
      };
    let entries: t('value) => list((key, 'value)) = toList;
    let forEach: (('value, key) => unit, t('value)) => unit =
      (fn, map) => {
        let _ = Map_Impl.forEach(fn, map^);
        ();
      };
    let get: (key, ~default: 'value, t('value)) => 'value =
      (key, ~default, map) => Map_Impl.get(key, ~default, map^);
    let getOpt: (key, t('value)) => option('value) =
      (key, map) => Map_Impl.getOpt(key, map^);
    let has: (key, t('value)) => bool =
      (key, map) => Map_Impl.has(key, map^);
    let keys: t('value) => list(key) = map => Map_Impl.keys(map^);
    let set: (key, 'value, t('value)) => unit =
      (key, value, map) => {
        map := Map_Impl.set(key, value, map^);
        ();
      };
    let values: t('value) => list('value) = map => Map_Impl.values(map^);
    /* Non-standard JavaScript methods - Similar to JavaScript array */
    let every: (('value, key) => bool, t('value)) => bool =
      (fn, map) => Map_Impl.every(fn, map^);
    let filter: (('value, key) => bool, t('value)) => unit =
      (fn, map) => {
        map := Map_Impl.filter(fn, map^);
        ();
      };
    let map: (('value, key) => 'value, t('value)) => unit =
      (fn, map) => {
        map := Map_Impl.map(fn, map^);
        ();
      };
    let reduce: (('acc, 'value, key) => 'acc, 'acc, t('value)) => 'acc =
      (fn, initial, map) => Map_Impl.reduce(fn, initial, map^);
    let some: (('value, key) => bool, t('value)) => bool =
      (fn, map) => Map_Impl.some(fn, map^);
  };
};

module MIntMap =
  MutableMap.Make(
    {
      type t = int;
      let compare = compare;
    },
  );

module MStringMap = MutableMap.Make(String);
