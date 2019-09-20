/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open TestFramework;

describe("Dir", ({test}) => {
  test("Basic home", ({expect}) =>
    expect.fn(() => ignore(Dir.home)).not.toThrow()
  );
  test("Getting fresh snapshot", ({expect}) => {
    let home = Dir.home;
    let cache = Dir.cache;
    let appData = Dir.App.data(~appIdentifier="the.best.app");
    module FreshDir =
      Dir.Snapshot({});
    expect.string(Fp.toString(FreshDir.home)).toEqual(
      Fp.toString(home),
    );
    expect.string(Fp.toString(FreshDir.cache)).toEqual(
      Fp.toString(cache),
    );
    expect.string(
      Fp.toString(FreshDir.App.data(~appIdentifier="the.best.app")),
    ).
      toEqual(
      Fp.toString(appData),
    );
  });
});
