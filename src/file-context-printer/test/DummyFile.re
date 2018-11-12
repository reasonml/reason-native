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
