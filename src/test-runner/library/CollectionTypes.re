/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
module Core_Set = Set;

module Core_Map = Map;

module type SetMakeResult = {
  /* The types */
  type t;
  type value;
  /* Non-standard creation */
  let empty: t;
  let fromList: list(value) => t;
  let fromArray: array(value) => t;
  /* Non-standard conversion */
  let toList: t => list(value);
  let toArray: t => array(value);
  /* Standard JavaScript Set methods */
  let size: t => int;
  let add: (value, t) => t;
  let clear: t => t;
  let delete: (value, t) => t;
  let entries: t => list((value, value));
  let forEach: (value => unit, t) => t;
  let has: (value, t) => bool;
  let values: t => list(value);
  /* Non-standard JavaScript methods - Similar to JavaScript array */
  let every: (value => bool, t) => bool;
  let filter: (value => bool, t) => t;
  let map: (value => value, t) => t;
  let reduce: (('acc, value) => 'acc, 'acc, t) => 'acc;
  let some: (value => bool, t) => bool;
};

module type MutableSetMakeResult = {
  /* The types */
  type t;
  type value;
  /* Non-standard creation */
  let empty: unit => t;
  let fromList: list(value) => t;
  let fromArray: array(value) => t;
  /* Non-standard conversion */
  let toList: t => list(value);
  let toArray: t => array(value);
  /* Standard JavaScript Set methods */
  let size: t => int;
  let add: (value, t) => unit;
  let clear: t => unit;
  let delete: (value, t) => unit;
  let entries: t => list((value, value));
  let forEach: (value => unit, t) => unit;
  let has: (value, t) => bool;
  let values: t => list(value);
  /* Non-standard JavaScript methods - Similar to JavaScript array */
  let every: (value => bool, t) => bool;
  let filter: (value => bool, t) => unit;
  let map: (value => value, t) => unit;
  let reduce: (('acc, value) => 'acc, 'acc, t) => 'acc;
  let some: (value => bool, t) => bool;
};

module type MapMakeResult = {
  /* The types */
  type t(+'value);
  type key;
  /* Non-standard creation */
  let empty: t('value);
  let fromList: list((key, 'value)) => t('value);
  let fromArray: array((key, 'value)) => t('value);
  /* Non-standard conversion */
  let toList: t('value) => list((key, 'value));
  let toArray: t('value) => array((key, 'value));
  /* Standard JavaScript Set methods */
  let size: t('value) => int;
  let clear: t('value) => t('value);
  let delete: (key, t('value)) => t('value);
  let entries: t('value) => list((key, 'value));
  let forEach: (('value, key) => unit, t('value)) => t('value);
  let get: (key, ~default: 'value, t('value)) => 'value;
  let getOpt: (key, t('value)) => option('value);
  let has: (key, t('value)) => bool;
  let keys: t('value) => list(key);
  let set: (key, 'value, t('value)) => t('value);
  let values: t('value) => list('value);
  /* Non-standard JavaScript methods - Similar to JavaScript array */
  let every: (('value, key) => bool, t('value)) => bool;
  let filter: (('value, key) => bool, t('value)) => t('value);
  let map: (('value, key) => 'nextValue, t('value)) => t('nextValue);
  let reduce: (('acc, 'value, key) => 'acc, 'acc, t('value)) => 'acc;
  let some: (('value, key) => bool, t('value)) => bool;
};

module type MutableMapMakeResult = {
  /* The types */
  type t('value);
  type key;
  /* Non-standard creation */
  let empty: unit => t('value);
  let fromList: list((key, 'value)) => t('value);
  let fromArray: array((key, 'value)) => t('value);
  /* Non-standard conversion */
  let toList: t('value) => list((key, 'value));
  let toArray: t('value) => array((key, 'value));
  /* Standard JavaScript Set methods */
  let size: t('value) => int;
  let clear: t('value) => unit;
  let delete: (key, t('value)) => unit;
  let entries: t('value) => list((key, 'value));
  let forEach: (('value, key) => unit, t('value)) => unit;
  let get: (key, ~default: 'value, t('value)) => 'value;
  let getOpt: (key, t('value)) => option('value);
  let has: (key, t('value)) => bool;
  let keys: t('value) => list(key);
  let set: (key, 'value, t('value)) => unit;
  let values: t('value) => list('value);
  /* Non-standard JavaScript methods - Similar to JavaScript array */
  let every: (('value, key) => bool, t('value)) => bool;
  let filter: (('value, key) => bool, t('value)) => unit;
  let map: (('value, key) => 'value, t('value)) => unit;
  let reduce: (('acc, 'value, key) => 'acc, 'acc, t('value)) => 'acc;
  let some: (('value, key) => bool, t('value)) => bool;
};
