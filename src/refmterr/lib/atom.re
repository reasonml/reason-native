/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
open Types_t;
module Range = {
  type t = rangeType;
  let emptyRange = ((0, 0), (0, 0));
};
module NuclideDiagnostic = {
  type filePath = string;
  type diagnosticType =
    | Error
    | Warning;
  module Trace = {
    type t = {
      typee: [ | `trace],
      text: option(string),
      html: option(string),
      filePath,
      range: option(Range.t),
    };
  };
  module Message = {
    type fileDiagnosticMessage('a) = {
      scope: [ | `file],
      providerName: string,
      typee: diagnosticType,
      filePath,
      text: option(string),
      html: option(string),
      range: option(Range.t),
      trace: option(array(Trace.t)),
      originalData: 'a,
    };
    type projectDiagnosticMessage('a) = {
      scope: [ | `project],
      providerName: string,
      typee: diagnosticType,
      text: option(string),
      html: option(string),
      range: option(Range.t),
      trace: option(array(Trace.t)),
      originalData: 'a,
    };
    type t('a) =
      | FileDiagnosticMessage(fileDiagnosticMessage('a))
      | ProjectDiagnosticMessage(projectDiagnosticMessage('a));
  };
};
