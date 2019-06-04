module TopNotchModule = struct
  type jeSuisString = string
  type jeSuisStrings = jeSuisString list list
  type myRecord = {nihao: int}
  type myRecordList = myRecord
  type myRecordListList = myRecordList list
  type myRecordListListList = myRecordListList list

  module Nested = struct
    type wow = myRecordListListList list
  end
end

let asd: TopNotchModule.jeSuisStrings = ([[[{nihao = 1}]]]: TopNotchModule.Nested.wow)
