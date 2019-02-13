open TestFramework;

describe("mock matchers", ({describe}) => {
  describe("passing output", ({test}) => {
    test("expect.mock.toThrow", ({expect}) => {
      let mock = Mock.mock1(a => raise(Not_found));

      try (Mock.fn(mock, 7)) {
      | _ => ()
      };

      expect.mock(mock).toThrow();
    });
    test("expect.mock.not.toThrow", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 7);

      expect.mock(mock).not.toThrow();
    });
    test("expect.mock.toThrowException", ({expect}) => {
      let mock = Mock.mock1(a => raise(Not_found));

      try (Mock.fn(mock, 7)) {
      | _ => ()
      };

      expect.mock(mock).toThrowException(Not_found);
    });
    test("expect.mock.toBeCalled", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 7);

      expect.mock(mock).toBeCalled();
    });
    test("expect.mock.not.toBeCalled", ({expect}) => {
      let mock = Mock.mock1(a => a);

      expect.mock(mock).not.toBeCalled();
    });
    test("expect.mock.toBeCalledTimes", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 7);
      let _ = Mock.fn(mock, 7);

      expect.mock(mock).toBeCalledTimes(2);
    });
    test("expect.mock.not.toBeCalledTimes wasn't called", ({expect}) => {
      let mock = Mock.mock1(a => a);

      expect.mock(mock).not.toBeCalledTimes(2);
    });
    test(
      "expect.mock.not.toBeCalledTimes called different number of times",
      ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 7);

      expect.mock(mock).not.toBeCalledTimes(2);
    });
    test("expect.mock.toBeCalledWith only call", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 1);

      expect.mock(mock).toBeCalledWith(1);
    });
    test("expect.mock.toBeCalledWith other calls", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 2);
      let _ = Mock.fn(mock, 1);
      let _ = Mock.fn(mock, 3);

      expect.mock(mock).toBeCalledWith(1);
    });
    test("expect.mock.toBeCalledWith custom equals", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 1.);
      let equals = (a, b) => abs_float(a -. b) < 0.1;

      expect.mock(mock).toBeCalledWith(~equals, 0.99);
    });
    test("expect.mock.not.toBeCalledWith", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 2);
      let _ = Mock.fn(mock, 1);
      let _ = Mock.fn(mock, 3);

      expect.mock(mock).not.toBeCalledWith(4);
    });
    test("expect.mock.not.toBeCalledWith custom equals", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 1.);
      let equals = (a, b) => abs_float(a -. b) < 0.1;

      expect.mock(mock).not.toBeCalledWith(~equals, 0.8);
    });
    test("expect.mock.lastCalledWith single call", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 2);

      expect.mock(mock).lastCalledWith(2);
    });
    test("expect.mock.lastCalledWith custom equals", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 2.);
      let equals = (a, b) => abs_float(a -. b) < 0.1;

      expect.mock(mock).lastCalledWith(~equals, 1.99);
    });
    test("expect.mock.lastCalledWith multiple calls", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 1);
      let _ = Mock.fn(mock, 2);

      expect.mock(mock).lastCalledWith(2);
    });
    test("expect.mock.not.lastCalledWith single call", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 2);

      expect.mock(mock).not.lastCalledWith(1);
    });
    test("expect.mock.not.lastCalledWith multiple calls", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 1);
      let _ = Mock.fn(mock, 2);

      expect.mock(mock).not.lastCalledWith(1);
    });
    test("expect.mock.not.lastCalledWith custom equals", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 2.);
      let equals = (a, b) => abs_float(a -. b) < 0.1;

      expect.mock(mock).not.lastCalledWith(~equals, 1.);
    });
    test("expect.mock.toReturnTimes", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 7);
      let _ = Mock.fn(mock, 7);

      expect.mock(mock).toReturnTimes(2);
    });
    test("expect.mock.not.toReturnTimes wasn't called", ({expect}) => {
      let mock = Mock.mock1(a => a);

      expect.mock(mock).not.toReturnTimes(2);
    });
    test("expect.mock.not.toReturnTimes threw exception", ({expect}) => {
      let mock = Mock.mock1(a => raise(Not_found));

      try (Mock.fn(mock, 1)) {
      | _ => ()
      };

      expect.mock(mock).not.toReturnTimes(1);
    });
    test(
      "expect.mock.not.toReturnTimes returned different number of times",
      ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 7);

      expect.mock(mock).not.toReturnTimes(2);
    });
    test("expect.mock.toReturnWith only call", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 1);

      expect.mock(mock).toReturnWith(1);
    });
    test("expect.mock.toReturnWith other calls", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 2);
      let _ = Mock.fn(mock, 1);
      let _ = Mock.fn(mock, 3);

      expect.mock(mock).toReturnWith(1);
    });
    test("expect.mock.toReturnWith custom equals", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 1.);
      let equals = (a, b) => abs_float(a -. b) < 0.1;

      expect.mock(mock).toReturnWith(~equals, 0.99);
    });
    test("expect.mock.not.toReturnWith", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 2);
      let _ = Mock.fn(mock, 1);
      let _ = Mock.fn(mock, 3);

      expect.mock(mock).not.toReturnWith(4);
    });
    test("expect.mock.not.toReturnWith custom equals", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 1.);
      let equals = (a, b) => abs_float(a -. b) < 0.1;

      expect.mock(mock).not.toReturnWith(~equals, 0.8);
    });
    test("expect.mock.lastReturnedWith single call", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 2);

      expect.mock(mock).lastReturnedWith(2);
    });
    test("expect.mock.lastReturnedWith custom equals", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 2.);
      let equals = (a, b) => abs_float(a -. b) < 0.1;

      expect.mock(mock).lastReturnedWith(~equals, 1.99);
    });
    test("expect.mock.lastReturnedWith multiple calls", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 1);
      let _ = Mock.fn(mock, 2);

      expect.mock(mock).lastReturnedWith(2);
    });
    test("expect.mock.not.lastReturnedWith single call", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 2);

      expect.mock(mock).not.lastReturnedWith(1);
    });
    test("expect.mock.not.lastReturnedWith multiple calls", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 1);
      let _ = Mock.fn(mock, 2);

      expect.mock(mock).not.lastReturnedWith(1);
    });
    test("expect.mock.not.lastReturnedWith custom equals", ({expect}) => {
      let mock = Mock.mock1(a => a);

      let _ = Mock.fn(mock, 2.);
      let equals = (a, b) => abs_float(a -. b) < 0.1;

      expect.mock(mock).not.lastReturnedWith(~equals, 1.);
    });
  });

  describe("failing output", ({test}) => {
    test("expect.mock.not.toThrow single exception", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => raise(Not_found));

          try (Mock.fn(mock, 7)) {
          | _ => ()
          };

          expect.mock(mock).not.toThrow();
        },
      )
    );
    test("expect.mock.not.toThrow multiple exceptions", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => raise(Not_found));

          try (Mock.fn(mock, 7)) {
          | _ => ()
          };

          try (Mock.fn(mock, 7)) {
          | _ => ()
          };

          expect.mock(mock).not.toThrow();
        },
      )
    );
    test("expect.mock.toThrow", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 7);

          expect.mock(mock).toThrow();
        },
      )
    );
    test("expect.mock.toThrowException no exception", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 7);

          expect.mock(mock).toThrowException(Not_found);
        },
      )
    );
    test("expect.mock.toThrowException wrong exception", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => raise(Division_by_zero));

          try (Mock.fn(mock, 7)) {
          | _ => ()
          };

          expect.mock(mock).toThrowException(Not_found);
        },
      )
    );
    test("expect.mock.toThrowException multiple wrong exceptions", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => raise(Division_by_zero));

          try (Mock.fn(mock, 7)) {
          | _ => ()
          };
          try (Mock.fn(mock, 7)) {
          | _ => ()
          };

          expect.mock(mock).toThrowException(Not_found);
        },
      )
    );
    test("expect.mock.toBeCalled", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          expect.mock(mock).toBeCalled();
        },
      )
    );
    test("expect.mock.not.toBeCalled don't overflow print", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock2((a, b) => a + b);

          let _ = Mock.fn(mock, 1, 2);
          let _ = Mock.fn(mock, 3, 4);

          expect.mock(mock).not.toBeCalled();
        },
      )
    );
    test("expect.mock.not.toBeCalled overflow print", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock2((a, b) => a + b);

          let _ = Mock.fn(mock, 1, 2);
          let _ = Mock.fn(mock, 3, 4);
          let _ = Mock.fn(mock, 5, 6);
          let _ = Mock.fn(mock, 7, 8);
          let _ = Mock.fn(mock, 10, 11);

          expect.mock(mock).not.toBeCalled();
        },
      )
    );
    test("expect.mock.toBeCalledTimes wasn't called", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          expect.mock(mock).toBeCalledTimes(2);
        },
      )
    );
    test(
      "expect.mock.toBeCalledTimes called different number of times", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 7);

          expect.mock(mock).toBeCalledTimes(2);
        },
      )
    );
    test("expect.mock.not.toBeCalledTimes", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 7);
          let _ = Mock.fn(mock, 7);

          expect.mock(mock).not.toBeCalledTimes(2);
        },
      )
    );
    test("expect.mock.not.toBeCalledWith only call", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 1);

          expect.mock(mock).not.toBeCalledWith(1);
        },
      )
    );
    test("expect.mock.not.toBeCalledWith other call", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 2);
          let _ = Mock.fn(mock, 1);
          let _ = Mock.fn(mock, 3);

          expect.mock(mock).not.toBeCalledWith(1);
        },
      )
    );
    test("expect.mock.not.toBeCalledWith custom equals", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 1.);
          let equals = (a, b) => abs_float(a -. b) < 0.1;

          expect.mock(mock).not.toBeCalledWith(~equals, 0.99);
        },
      )
    );
    test("expect.mock.toBeCalledWith multiple other calls", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 2);
          let _ = Mock.fn(mock, 1);
          let _ = Mock.fn(mock, 3);

          expect.mock(mock).toBeCalledWith(4);
        },
      )
    );
    test("expect.mock.not.toBeCalledWith custom equals", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 1.);
          let equals = (a, b) => abs_float(a -. b) < 0.1;

          expect.mock(mock).toBeCalledWith(~equals, 0.8);
        },
      )
    );
    test("expect.mock.not.lastCalledWith single call", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 2);

          expect.mock(mock).not.lastCalledWith(2);
        },
      )
    );
    test("expect.mock.not.lastCalledWith custom equals", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 2.);
          let equals = (a, b) => abs_float(a -. b) < 0.1;

          expect.mock(mock).not.lastCalledWith(~equals, 1.99);
        },
      )
    );
    test("expect.mock.not.lastCalledWith multiple calls", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 1);
          let _ = Mock.fn(mock, 2);

          expect.mock(mock).not.lastCalledWith(2);
        },
      )
    );
    test("expect.mock.lastCalledWith single call", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 2);

          expect.mock(mock).lastCalledWith(1);
        },
      )
    );
    test("expect.mock.lastCalledWith multiple calls", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 1);
          let _ = Mock.fn(mock, 2);

          expect.mock(mock).lastCalledWith(1);
        },
      )
    );
    test("expect.mock.lastCalledWith custom equals", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 2.);
          let equals = (a, b) => abs_float(a -. b) < 0.1;

          expect.mock(mock).lastCalledWith(~equals, 1.);
        },
      )
    );
    test("expect.mock.not.toReturnTimes", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 7);
          let _ = Mock.fn(mock, 7);

          expect.mock(mock).not.toReturnTimes(2);
        },
      )
    );
    test("expect.mock.toReturnTimes wasn't called", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          expect.mock(mock).toReturnTimes(2);
        },
      )
    );
    test("expect.mock.toReturnTimes threw exception", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => raise(Not_found));

          try (Mock.fn(mock, 1)) {
          | _ => ()
          };

          expect.mock(mock).toReturnTimes(1);
        },
      )
    );
    test(
      "expect.mock.toReturnTimes returned different number of times", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 7);

          expect.mock(mock).toReturnTimes(2);
        },
      )
    );
    test("expect.mock.not.toReturnWith only call", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 1);

          expect.mock(mock).not.toReturnWith(1);
        },
      )
    );
    test("expect.mock.not.toReturnWith other call", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 2);
          let _ = Mock.fn(mock, 1);
          let _ = Mock.fn(mock, 3);

          expect.mock(mock).not.toReturnWith(1);
        },
      )
    );
    test("expect.mock.not.toReturnWith custom equals", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 1.);
          let equals = (a, b) => abs_float(a -. b) < 0.1;

          expect.mock(mock).not.toReturnWith(~equals, 0.99);
        },
      )
    );
    test("expect.mock.toReturnWith multiple other calls", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 3);
          let _ = Mock.fn(mock, 3);
          let _ = Mock.fn(mock, 2);
          let _ = Mock.fn(mock, 1);
          let _ = Mock.fn(mock, 3);

          expect.mock(mock).toReturnWith(4);
        },
      )
    );
    test("expect.mock.not.toReturnWith custom equals", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 1.);
          let equals = (a, b) => abs_float(a -. b) < 0.1;

          expect.mock(mock).toReturnWith(~equals, 0.8);
        },
      )
    );
    test("expect.mock.not.lastReturnedWith single call", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 2);

          expect.mock(mock).not.lastReturnedWith(2);
        },
      )
    );
    test("expect.mock.not.lastReturnedWith custom equals", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 2.);
          let equals = (a, b) => abs_float(a -. b) < 0.1;

          expect.mock(mock).not.lastReturnedWith(~equals, 1.99);
        },
      )
    );
    test("expect.mock.not.lastReturnedWith multiple calls", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 1);
          let _ = Mock.fn(mock, 2);

          expect.mock(mock).not.lastReturnedWith(2);
        },
      )
    );
    test("expect.mock.lastReturnedWith single call", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 2);

          expect.mock(mock).lastReturnedWith(1);
        },
      )
    );
    test("expect.mock.lastReturnedWith multiple calls", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 1);
          let _ = Mock.fn(mock, 2);

          expect.mock(mock).lastReturnedWith(1);
        },
      )
    );
    test("expect.mock.lastReturnedWith custom equals", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          let _ = Mock.fn(mock, 2.);
          let equals = (a, b) => abs_float(a -. b) < 0.1;

          expect.mock(mock).lastReturnedWith(~equals, 1.);
        },
      )
    );

    test("expect.mock.lastReturnedWith wasn't called", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => a);

          expect.mock(mock).lastReturnedWith(2);
        },
      )
    );
    test("expect.mock.lastReturnedWith threw exception", testUtils =>
      MatcherSnapshotTestRunner.snapshotFailingTestCase(
        testUtils,
        ({expect}) => {
          let mock = Mock.mock1(a => raise(Not_found));

          try (
            {
              let _ = Mock.fn(mock, 7);
              ();
            }
          ) {
          | _ => ()
          };

          expect.mock(mock).lastReturnedWith(7);
        },
      )
    );
  });
});
