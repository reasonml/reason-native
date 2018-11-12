/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 */

let isSome = (o: option('a)): bool =>
  switch (o) {
  | Some(_) => true
  | None => false
  };
let isNone = (o: option('a)): bool =>
  switch (o) {
  | Some(_) => false
  | None => true
  };
let valuex = (o: option('a)): 'a =>
  switch (o) {
  | Some(value) => value
  | None => Errors.fatal("Expected option to have value but got None")
  };
let valueOr = (default: 'a, o: option('a)): 'a =>
  switch (o) {
  | Some(value) => value
  | None => default
  };
let toBool = valueOr(false);

module Infix = {
  let (>>=) = (o, f) =>
    switch (o) {
    | Some(x) => f(x)
    | None => None
    };
  let (|?:) = (o, default) =>
    switch (o) {
    | Some(value) => value
    | None => default
    };
  let (>>|) = (opt, fn) =>
    switch (opt) {
    | Some(value) => Some(fn(value))
    | None => None
    };
};
