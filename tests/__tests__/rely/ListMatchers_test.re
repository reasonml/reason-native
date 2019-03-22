open TestFramework;
open Rely.Test;

module ListMatchersTests =
  CollectionMatchersTest.Make(
    RelyInternal.ListMatchers.List,
    {
      type t('a) = list('a);
      type matchersWithNot('a) = RelyInternal.ListMatchers.matchersWithNot('a);
      let ofList = l => l;
      let expectPath:
        (Rely.Test.testUtils('a), t('b)) => matchersWithNot('b) =
        t => t.expect.list;
      let collectionName = "list";
    },
  );
