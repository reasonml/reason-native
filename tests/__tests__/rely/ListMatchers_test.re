open TestFramework;
open Rely.Test;

module Foo =
  CollectionMatchersTest.Make(
    Rely.ListMatchers.List,
    {
      type t('a) = list('a);
      type matchersWithNot('a) = Rely.ListMatchers.matchersWithNot('a);
      let ofList = l => l;
      let expectPath:
        (Rely.Test.testUtils('a), t('b)) => matchersWithNot('b) =
        t => t.expect.list;
      let collectionName = "list";
    },
  );
