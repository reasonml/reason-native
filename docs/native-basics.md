---
id: native-basics
title: Native Reason Basics
sidebar_label: Native Basics
---

## Relation to OCaml

The [ReasonML What and Why](https://reasonml.github.io/docs/en/what-and-why) page provides a good description for what ReasonML is:
> Reason is not a new language; it's a new syntax and toolchain powered by the battle-tested language, OCaml. Reason gives OCaml a familiar syntax geared toward JavaScript programmers, and caters to the existing NPM/Yarn workflow folks already know.

What this effectively means is that
- Reason projects **are** OCaml projects
- Reason code **is** OCaml code
- Projects are free to harness the power of the [OCaml ecosystem](https://opam.ocaml.org/packages/).

The [hello-reason](https://github.com/esy-ocaml/hello-reason.git) starter described in [Getting Started](./getting-started) is based off of [Dune](https://dune.build), an OCaml/Reason build system used by ~40% of the OCaml community.

## Native Reason vs Reason Native

**Reason Native** is a set of packages designed to assist development of native Reason projects.

**Native Reason** projects are Reason projects that target native system binaries. We say native Reason projects because Reason can also be compiled to JavaScript through [BuckleScript](https://bucklescript.github.io/).

If you are new to Reason and would like to learn more about the language, please visit the [ReasonML Docs](https://reasonml.github.io/docs/en/what-and-why).
