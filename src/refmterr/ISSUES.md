Record fields that aren't found might be include long paths.

      Here is an example of a case that is not matched:
      (DeclareInterface _|DeclareTypeAlias _|DeclareOpaqueType _|OpaqueType _)
        > Unformatted Error Output:
        > File "main.re", line 1388, characters 25-29:
        > Error: Unbound record field Property.kind

      Error: main.re:1388 25-29
      1385 ¦   {
      1386 ¦     Property.key: Property.Identifier((_, name)),
      1387 ¦     value: (_, value) as valueWrap,
      1388 ¦     kind: Property.Init,
      1389 ¦     _
      1390 ¦   }
      1391 ¦ )) =>

      Record field Property; can't be found in any record type.

      Record fields must be "in scope". That means you need to `open TheModule` where the record type is defined.
      Alternatively, instead of opening a module, you can prefix the record field name like {TheModule.x: 0, y: 100}.


Newer compiler versions spit out warnings that are turned to errors like this:


File "src/lib/LayoutSupport.re", line 561, characters 31-36:
Error (warning 27): unused variable width.
