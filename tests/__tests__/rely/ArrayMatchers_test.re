open TestFramework;
open Rely.Test;

module ArrayMatchersTests =
  CollectionMatchersTest.Make(
    RelyInternal.ArrayMatchers.Array,
    {
      type t('a) = array('a);
      type matchersWithNot('a) = RelyInternal.ArrayMatchers.matchersWithNot('a);
      let ofList = Array.of_list;
      let expectPath:
        (Rely.Test.testUtils('a), t('b)) => matchersWithNot('b) =
        t => t.expect.array;
      let collectionName = "array";
    },
  );
