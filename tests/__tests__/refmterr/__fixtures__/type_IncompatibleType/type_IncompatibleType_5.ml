module PettyModule = struct
  type youAreAString = string

  module Nested = struct
    type weAreStrings = youAreAString list
  end
end

module TopNotchModule = struct
  type stringListList = string list list
  type myRecord = {nihao: int}
  type myRecordList = myRecord list
  type myRecordListList = myRecordList list
end

let asd: PettyModule.Nested.weAreStrings =
  ([[{nihao = 1}]]: TopNotchModule.myRecordListList)
