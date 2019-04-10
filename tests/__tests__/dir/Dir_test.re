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
    expect.string(Path.toString(FreshDir.home)).toEqual(
      Path.toString(home),
    );
    expect.string(Path.toString(FreshDir.cache)).toEqual(
      Path.toString(cache),
    );
    expect.string(
      Path.toString(FreshDir.App.data(~appIdentifier="the.best.app")),
    ).
      toEqual(
      Path.toString(appData),
    );
  });
});
