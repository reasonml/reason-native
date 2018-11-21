module type FileContextPrinterConfig = {
    let linesBefore: int;
    let linesAfter: int;
}

type rowColumnRange = ((int, int), (int, int))

module Make: (Config: FileContextPrinterConfig, Styl: Stylish.StylishSig) => {
    /* todo name better... I think we actually want to return a string and not actually print? */
    let print: (~path:string, ~range:rowColumnRange) => string
}
