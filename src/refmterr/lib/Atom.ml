module Range =
  struct
    type t = ((int* int)* (int* int)) [@@deriving yojson]
    let emptyRange = ((0, 0), (0, 0))
  end
module NuclideDiagnostic =
  struct
    type filePath = string
    type diagnosticType =
      | Error
      | Warning
    module Trace =
      struct
        type t =
          {
          typee: [ `trace ];
          text: string option;
          html: string option;
          filePath: filePath;
          range: Range.t option;}
      end
    module Message =
      struct
        type 'a fileDiagnosticMessage =
          {
          scope: [ `file ];
          providerName: string;
          typee: diagnosticType;
          filePath: filePath;
          text: string option;
          html: string option;
          range: Range.t option;
          trace: Trace.t array option;
          originalData: 'a;}
        type 'a projectDiagnosticMessage =
          {
          scope: [ `project ];
          providerName: string;
          typee: diagnosticType;
          text: string option;
          html: string option;
          range: Range.t option;
          trace: Trace.t array option;
          originalData: 'a;}
        type 'a t =
          | FileDiagnosticMessage of 'a fileDiagnosticMessage
          | ProjectDiagnosticMessage of 'a projectDiagnosticMessage
      end
  end
