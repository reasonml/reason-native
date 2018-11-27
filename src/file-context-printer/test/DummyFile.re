/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 *
 * LICENSE file in the root directory of this source tree.
 */;
let myFunction = () => print_endline("do something");

let ex = (arg) => {
    if(arg === true) {
        print_endline("raising an error");
        raise(Not_found);
    }
}

let executeCode = () => {
    myFunction();
    let _ = try(ex(true)) {
    | Not_found => print_endline("exception")
    };
}
