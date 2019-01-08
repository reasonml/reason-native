/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open TestFramework;
open Rely.MatcherTypes;

let basePrinter = Console.ObjectPrinter.base;

 module User = {
   type universe =
     | Facebook
     | Instagram
     | Oculus
     | Unknown;
   type t('a) = {
     name: string,
     universe,
     age: int,
     data: 'a,
   };
   let universeToString = universe =>
     switch (universe) {
     | Facebook => "Facebook"
     | Instagram => "Instagram"
     | Oculus => "Oculus"
     | Unknown => "Unknown"
     };
   /*
    * Custom matchers and related logic should be separated into a separate
    * Test module for the structure.
    */
   module Test = {
     /*
      * Define all the relevant structures. Use of `.not` must be manually
      * defined, like in this example.
      */
     type matchers('a) = {
       inFacebook: unit => unit,
       inInstagram: unit => unit,
       inOculus: unit => unit,
       toHaveData: 'a => unit,
     };
     type matchersWithNot('a) = {
       not: matchers('a),
       inFacebook: unit => unit,
       inInstagram: unit => unit,
       inOculus: unit => unit,
       toHaveData: 'a => unit,
       toHaveAge: (~olderThan: int, ~youngerThan: int) => unit,
     };
     /*
      * This is the type of `.ext`, (short for extensions) that will eventually
      * get exposed via `expect.ext` in tests.
      */
     type ext = {user: 'a. t('a) => matchersWithNot('a)};
     /*
      * This is a function that will get access to some utilities defined in
      * FB.Test, it will use the utilities to create register matchers and
      * then return the extension object, `.ext`.
      */
     let matchers = ({createMatcher}) => {
       let noError = () => "";
       /*
        * IMPORTANT: Due to how the types are inferred matchers need to be
        * created within this function, after the first "argument" is passed
        * to the matcher. Otherwise you may get weird errors about types
        * being less general than required, or that types can't be inferred.
        */
       let user = user => {
        let dataToString = basePrinter.polymorphicPrint(basePrinter);
         /* We will generalize the universe matchers */
         let createUniverseMatcher = (~isNot, expectedUniverse) => {
           let expectedUniverseString = universeToString(expectedUniverse);
           createMatcher((_, actualThunk, _) => {
               /*
                * The actual and expected values are all wrapped in thunks,
                * functions that accept no arguments and return a value. This
                * allows matchers to be more general and include things like named
                * arguments, we just place a burden on the matchers that they
                * must package and unpackage the arguments via thunks.
                */
               let actual = actualThunk();
               let result =
                 if (isNot) {
                   /* We expect the universes to not be equal */
                   if (actual.universe === expectedUniverse) {
                     (
                       () =>
                         "Expected user to not be in the "
                         ++ expectedUniverseString
                         ++ " universe",
                       false,
                     );
                   } else {
                     (noError, true);
                   };
                 } else if
                   /* We expect the universes to be equal */
                   (actual.universe !== expectedUniverse) {
                   (
                     () =>
                       "Expected user to be in the "
                       ++ expectedUniverseString
                       ++ " universe, but user was in the "
                       ++ universeToString(actual.universe)
                       ++ " universe",
                     false,
                   );
                 } else {
                   (noError, true);
                 };
               result;
             },
           );
         };
         let notInFacebook = createUniverseMatcher(~isNot=true, Facebook);
         let notInInstagram = createUniverseMatcher(~isNot=true, Instagram);
         let notInOculus = createUniverseMatcher(~isNot=true, Oculus);
         let inFacebook = createUniverseMatcher(~isNot=false, Facebook);
         let inInstagram = createUniverseMatcher(~isNot=false, Instagram);
         let inOculus = createUniverseMatcher(~isNot=false, Oculus);
         /* Generalize the data matcher */
         let createDataMatcher = (~isNot) => {
           createMatcher((_, actualThunk, expectedThunk) => {
             let actual = actualThunk();
             let expected = expectedThunk();
             let result =
               if (isNot) {
                 /* Error case, data should not equal what was given. */
                 if (actual.data == expected) {
                   (
                     () =>
                       "Expected user not to have the data:\n  "
                       ++ dataToString(expected),
                     false,
                   );
                 } else {
                   (noError, true);
                 };
               } else {
                 ();
                 /* Error case, data should be equal to what was given. */
                 if (actual.data != expected) {
                   (
                     () =>
                       "Expected user to have the data:\n  "
                       ++ dataToString(expected)
                       ++ "\nBut data was\n  "
                       ++ dataToString(actual.data),
                     false,
                   );
                 } else {
                   (noError, true);
                 };
               };
             result;
           });
         };
         let notToHaveData = createDataMatcher(~isNot=true);
         let toHaveData = createDataMatcher(~isNot=false);
         let toHaveAge =
           createMatcher((_, actualThunk, expectedThunk) => {
             let user = actualThunk();
             /*
              * This is an example of using thunks to unpackage the named
              * arguments for `toHaveAge`. This tuple is tied exactly to the
              * structure that is build below. It doesn't have to be a tuple, it
              * could be any single value. If this were more complex a record
              * might be more appropriate.
              */
             let (olderThan, youngerThan) = expectedThunk();
             let result =
               if (user.age < olderThan) {
                 (
                   () =>
                     "Expected user to be older than:\n  "
                     ++ string_of_int(olderThan)
                     ++ "\nBut user was\n  "
                     ++ string_of_int(user.age),
                   false,
                 );
               } else if (user.age > youngerThan) {
                 (
                   () =>
                     "Expected user to be younger than:\n  "
                     ++ string_of_int(youngerThan)
                     ++ "\nBut user was\n  "
                     ++ string_of_int(user.age),
                   false,
                 );
               } else {
                 (noError, true);
               };
             result;
           });
         /*
          * Now we build the actual extensions structure. Here we pass in
          * thunks to the result of our createMatcher calls above. This is how
          * we package up actual and expected values in a general way.
          */
         let matchers = {
           not: {
             inFacebook: () => notInFacebook(() => user, () => ()),
             inInstagram: () => notInInstagram(() => user, () => ()),
             inOculus: () => notInOculus(() => user, () => ()),
             toHaveData: expected => notToHaveData(() => user, () => expected),
           },
           inFacebook: () => inFacebook(() => user, () => ()),
           inInstagram: () => inInstagram(() => user, () => ()),
           inOculus: () => inOculus(() => user, () => ()),
           toHaveData: expected => toHaveData(() => user, () => expected),
           /*
            * This is where we accept the named arguments and then package them
            * into a thunk that can be unpackaged later on.
            */
           toHaveAge: (~olderThan, ~youngerThan) =>
             toHaveAge(() => user, () => (olderThan, youngerThan)),
         };
         matchers;
       };
       let ext = {user: user};
       ext;
     };
   };
 };

 /*
  * This is typically where the normal test code would start. The structure and
  * matchers would be defined externally and reused across many test files.
  */
 open User;

 let describe = extendDescribe(User.Test.matchers);

 /* Alice is a Facebook user with string data. */
 let alice = {
   name: "Alice",
   universe: Facebook,
   age: 25,
   data: "some random string data",
 };

 /* Bob is an Oculus user with int data. */
 let bob = {name: "Bob", universe: Oculus, age: 30, data: 42};

 describe("Custom Matchers", ({test}) => {
   test("Alice tests", ({expect}) => {
     expect.ext.user(alice).inFacebook();
     expect.ext.user(alice).not.inInstagram();
     expect.ext.user(alice).not.inOculus();
     expect.ext.user(alice).toHaveData("some random string data");
     expect.ext.user(alice).not.toHaveData("the wrong data");
     expect.ext.user(alice).toHaveAge(~olderThan=18, ~youngerThan=65);
     /* A non-custom test could be */
     expect.string(alice.data).toEqual("some random string data");
   });
   test("Bob tests", ({expect}) => {
     expect.ext.user(bob).not.inFacebook();
     expect.ext.user(bob).not.inInstagram();
     expect.ext.user(bob).inOculus();
     expect.ext.user(bob).toHaveData(42);
     expect.ext.user(bob).not.toHaveData(100);
     expect.ext.user(bob).toHaveAge(~olderThan=18, ~youngerThan=65);
     /* A non-custom test could be */
     expect.int(bob.data).toBe(42);
   });
   test("Alice invalid tests", ({expect}) => {
     expect.ext.user(alice).inFacebook();
     expect.ext.user(alice).not.inOculus();
     /* This test fails at compile time, Alice has string not int data. */
     /* expect.ext.user(alice).toHaveData(1); */
     /* These tests fails at run time with helpful error messages. */
     /* expect.ext.user(alice).not.inFacebook(); */
     /* expect.ext.user(alice).inOculus(); */
     /* expect.ext.user(alice).toHaveData("the wrong data"); */
     /* expect.ext.user(alice).toHaveAge(~olderThan=50, ~youngerThan=65); */
     ();
   });
   test("Bob invalid tests", ({expect}) => {
     expect.ext.user(bob).not.inFacebook();
     expect.ext.user(bob).inOculus();
     /* This test fails at compile time, Bob has int not string data. */
     /* expect.ext.user(bob).toHaveData("string data"); */
     /* These tests fails at run time with helpful error messages. */
     /* expect.ext.user(bob).inFacebook(); */
     /* expect.ext.user(bob).not.inOculus(); */
     /* expect.ext.user(bob).toHaveData(100); */
     /* expect.ext.user(bob).toHaveAge(~olderThan=18, ~youngerThan=22); */
     ();
   });
 });

 /*
  * Let's pretend like there is another custom set of matchers for "Accounts"
  * rather than users. How can we compose the matchers and use both?
  */
 module Account = {
   module Test = {
     /*
      * We will just steal the same types so we don't have to implement more
      * matchers. This could be an entirely different set of matchers.
      */
     type ext = {account: 'a. User.t('a) => User.Test.matchersWithNot('a)};
     let matchers = utils => {
       let userExt = User.Test.matchers(utils);
       let ext = {account: userExt.user};
       ext;
     };
   };
 };

 /*
  * We can switch over to using the Account matchers, and prevent the usage of
  * the User matchers. This is typically recommended and sufficient. It's not
  * necessary to combine the custom matchers and use them within the same
  * describe block.
  */
 let describe = extendDescribe(Account.Test.matchers);

 describe("Separate type of Custom Matchers", ({test}) => {
   test("Alice tests", ({expect}) => {
     expect.ext.account(alice).inFacebook();
     expect.ext.account(alice).not.inOculus();
     /* We can use account, but not user anymore: */
     /* expect.ext.user(alice).inFacebook(); */
     ();
   });
   ();
 });

 /*
  * If you really need to combine matchers you can do so like this. It does
  * require duplicating the types of the other defined matchers.
  *
  * If there are sets of custom matchers that are frequently used together, it's
  * okay to define them in the same place even if the structures are logically
  * unique. Then this combining step can be avoided.
  */
 module Combined = {
   type ext = {
     account: 'a. User.t('a) => User.Test.matchersWithNot('a),
     user: 'a. User.t('a) => User.Test.matchersWithNot('a),
   };
   let matchers = utils => {
     let accountExt = Account.Test.matchers(utils);
     let userExt = User.Test.matchers(utils);
     let ext = {account: accountExt.account, user: userExt.user};
     ext;
   };
 };

 /* Use the combined matchers */
 let describe = extendDescribe(Combined.matchers);

 describe("Combined Custom Matchers", ({test}) => {
   test("Alice tests", ({expect}) => {
     /* Now we can use both account and user! */
     expect.ext.account(alice).inFacebook();
     expect.ext.account(alice).not.inOculus();
     expect.ext.user(alice).toHaveData("some random string data");
   });
   ();
 });
