/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open Types;

/*
 * TODO: test normalization of CRLF even on linux.
 * Comment about exception catching in core libraries.
 * https://discuss.ocaml.org/t/how-to-create-a-temporary-directory-in-ocaml/1815/3
 */
let readText = path => {
  let stringPath = Fp.toString(path);
  let impl = inChan => {
    let lines = {contents: []};
    let continue = {contents: true};
    /* TODO: Better Unix usage and robust error handling */
    while (continue.contents) {
      try (
        {
          let line = input_line(inChan);
          let len = String.length(line);
          /* Opened the channel in binary mode so we could manually do
           * the same line normalization in linux/osx. */
          let line =
            if (len > 0 && line.[len - 1] === '\r') {
              String.sub(line, 0, len - 1);
            } else {
              line;
            };
          lines.contents = [line, ...lines.contents];
        }
      ) {
      | End_of_file => continue.contents = false
      };
    };
    close_in(inChan);
    Ok(List.rev(lines.contents));
  };
  try (
    {
      let inChan = open_in_bin(stringPath);
      Util.withInChannel(inChan, impl);
    }
  ) {
  | Sys_error(msg) as e => Error(e)
  };
};
let readTextExn = path => Util.throwErrorResult(readText(path));

let writeText = (~lineEnds=PlatformDefault, path, lines) => {
  let stringPath = Fp.toString(path);
  let impl = outChan => {
    List.iter(
      line => {
        output_string(outChan, line);
        switch (lineEnds, Sys.win32) {
        | (PlatformDefault, true)
        | (Windows, _) => output_string(outChan, "\r\n")
        | (PlatformDefault, false) => output_string(outChan, "\n")
        | (Unix, _) => output_string(outChan, "\n")
        };
      },
      lines,
    );
    Ok();
  };
  try (
    {
      let outChan = open_out_bin(stringPath);
      Util.withOutChannel(outChan, impl);
    }
  ) {
  /* Containing directory likely doesn't exist */
  | Sys_error(_) as e => Error(e)
  };
};

let writeTextExn = (~lineEnds=?, path, lines) =>
  Util.throwErrorResult(writeText(~lineEnds?, path, lines));
