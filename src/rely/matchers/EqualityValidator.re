/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

type referentialEqualityValidationFailureTag =
  | DoubleTag
  | DoubleArrayTag
  | StringTag
  | ObjectTag;

type referentialEqualityValidationResult =
  | Pass
  | Fail(referentialEqualityValidationFailureTag);

let validateUsageOfReferentialEquality = value =>
  switch (Obj.tag(Obj.magic(value))) {
  | tag when tag == Obj.object_tag => Fail(ObjectTag)
  | tag when tag == Obj.string_tag => Fail(StringTag)
  | tag when tag == Obj.double_tag => Fail(DoubleTag)
  | tag when tag == Obj.double_array_tag => Fail(DoubleArrayTag)
  | _ => Pass
  };

type structuralEqualityValidationFailureTag =
  | DoubleTag
  | DoubleArrayTag;

type structuralEqualityValidationResult =
  | Pass
  | Fail(structuralEqualityValidationFailureTag);

let validateUsageOfStructuralEquality = value =>
  switch (Obj.tag(Obj.magic(value))) {
  | tag when tag == Obj.double_tag => Fail(DoubleTag)
  | tag when tag == Obj.double_array_tag => Fail(DoubleArrayTag)
  | _ => Pass
  };
