---
id: getting-started
title: Getting Started
---

## Install Esy

We recommend using [Esy](https://esy.sh) as your package manager in native Reason projects. Esy is a native, npm-esque package manager with built-in dependency isolation and fast, incremental builds.

The easiest way to install **Esy** is with [npm](https://nodejs.org):

```sh
npm install -g esy
```

## Create a fresh project

First, clone a copy of the [`esy-ocaml/hello-reason`](https://github.com/esy-ocaml/hello-reason.git) project:

```sh
git clone https://github.com/esy-ocaml/hello-reason.git
cd hello-reason
```

Then, run `esy` to install relevant dependencies and build your app:

```sh
esy
```

## Run your first app

Run

```sh
esy x Hello.exe # runs Hello.exe
```

If everything went as expected, you should see the following output:

```sh
Running Test Program:
[31mHello[39m, [32mWorld[39m!
```

Believe it or not, you're already using two Reason Native packages!

### Pastel

Open up `lib/Util.re` and you might see something interesting:

```reason
/* lib/Util.re */
let hello = () =>
  Pastel.(
    <Pastel>
      <Pastel color=Red> "Hello" </Pastel>
      ", "
      <Pastel color=Green> "World" </Pastel>
      "!"
    </Pastel>
  );
```

[Pastel](./pastel/index.md) is a React-like text formatting library for the terminal; one of its core features is the ability to format console text with [ANSI Escape Codes](https://en.wikipedia.org/wiki/ANSI_escape_code#Colors).

Let's try making `Hello` yellow:

```reason
/* lib/Util.re */
/* ... */
    <Pastel>
      <Pastel color=Yellow> "Hello" </Pastel>
      ", "
/* ... */
```

Run `esy build` to rebuild your app, and run `esy x Hello.exe` to see your changes:

```sh
Running Test Program:
[33mHello[39m, [32mWorld[39m!
```

### Console

Similarly, let's take a look at `bin/Hello.re`:

```reason
/* bin/Hello.re */
Console.log("Running Test Program:");
let () = print_endline(Lib.Util.hello());
```

The [Console](console/index.md) package allows you to log nearly anything without having to define any printers. Let's try logging a nested tuple:

```reason
/* bin/Hello.re */
Console.log((1, (2, 3)));
let () = print_endline(Lib.Util.hello());
```

Running `esy build && esy x Hello.exe` should produce the following:

```sh
{1, {2, 3}}
[33mHello[39m, [32mWorld[39m!
```
