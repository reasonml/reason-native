open TestFramework;
open Rely.Test;

module ArrayMatchersTests =
  CollectionMatchersTest.Make(
    Rely.ArrayMatchers.Array,
    {
      type t('a) = array('a);
      type matchersWithNot('a) = Rely.ArrayMatchers.matchersWithNot('a);
      let ofList = Array.of_list;
      let expectPath:
        (Rely.Test.testUtils('a), t('b)) => matchersWithNot('b) =
        t => t.expect.array;
      let collectionName = "array";
    },
  );
