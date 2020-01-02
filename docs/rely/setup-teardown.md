---
id: setup-teardown
title: Setup and Teardown
sidebar_label: Setup and Teardown
---

As of version 3.0.0, Rely provides the `beforeEach`, `beforeAll`, `afterEach`, and `afterAll` test lifecycle functions to safely perform setup and teardown operations while discouraging the usage of patterns that unsafely share state between tests.

In cases where teardown is not required, `beforeEach` and `beforeAll` can be helpful for code organization, but are not required to be used.

When teardown operations are required after each test it is highly recommended to use `afterEach` to ensure proper exception handling behavior. `afterAll` is the only way to perform teardown operations after an entire test suite has executed.

## When Lifecycle Functions are Called (and what they are called with)

All lifecycle functions are scoped to a particular describe block. Tests inside nested describes behave the same as those outside of the parent describe block (e.g. `beforeAll` is only called once before the outermost describe, `beforeEach` is called once before each test regardless of the level of nesting).

Within that block they work as follows:

- `beforeAll` is called once before any tests within the describe begin to execute

- `afterAll` is called once with the return value of beforeAll after all tests within the describe have executed

- `beforeEach` is called with the return value of beforeAll before each test is executed

- `afterEach` is called with the return value of beforeEach after each test is executed

For example:

```reason
open TestFramework;

let {describe, describeSkip, describeOnly} =
  describeConfig
  |> withLifecycle(testLifecycle =>
       testLifecycle
       |> beforeAll(() => print_endline("before all"))
       |> afterAll(() => print_endline("after all"))
       |> beforeEach(() => print_endline("before each"))
       |> afterEach(() => print_endline("after each"))
     )
  |> build;

describe("test lifecycle order example", ({test, describe}) => {
  test("some test", _ =>
    print_endline("test1")
  );
  test("some other test", _ =>
    print_endline("test2")
  );
  describe("nested describe", ({test}) =>
    test("nested test", _ =>
      print_endline("nested test")
    )
  );
});
```
outputs
```bashell
before all
before each
test1
after each
before each
test2
after each
before each
nested test
after each
after all
```

## Passing Data to Tests

Often a goal of test setup is to make some external resource such as a testing database or a temp directory available for use in individual tests.

The return value of `beforeEach` is made available via the "env" field that tests have access to. `beforeEach` is passed the return value of `beforeAll` (the default implementation of beforeAll just returns unit).

If `beforeEach` is not used, the identity function is used. Consequently __in the absence of a user specified beforeEach function, the return value of beforeAll is passed via the "env" field of the test record__.

If neither beforeEach nor beforeAll are specified, env is bound to unit and can safely be ignored.

### One-time setup operations

Sometimes a single setup step is sufficient for a test suite. This can happen when establishing a connection to some external resource that can be shared across individual tests without consequence.

Suppose for example that we are testing a web scraping utility and need to both start and stop a test server for use in our tests.

```reason
open TestFramework;

let {describe, describeSkip, describeOnly} =
  describeConfig
  |> withLifecycle(testLifecycle =>
       testLifecycle
       |> beforeAll(() => TestServer.start())
       |> afterAll(server => TestServer.stop(server))
     )
  |> build;

describe("My web scraper", ({test}) => {
  test("My snapshot test", ({expect, env}) => {
    let result = MyWebScraper.scrape(server.baseUrl);

    expect.string(result).toMatchSnapshot();
  })
  ...
});
```

### Repeated setup for multiple tests

Suppose that we wish to test a calendar application that interacts with a database of important historical events. Each test needs access to a fresh copy of a testing database.

```reason
let {describe, describeSkip, describeOnly} =
  describeConfig
  |> withLifecycle(testLifecycle =>
       testLifecycle
       |> beforeEach(() => EventsDB.initializeTestDatabase())
       |> afterEach(db => EventsDB.dispose(db))
     )
  |> build;

describe("My calendar app", ({test}) => {
  test("adding a historical event", ({expect, env})
    => {
      let calendar = MyCalendarApp.init(env);
      let initialEvents = calendar.getHistoricalEvents(2016, 11, 2);

      let testEvent =
        historicalEventBuilder
        |> withDate(2016, 11, 2)
        |> withDescription("Chicago Cubs win the World Series")
        |> build;

      calendar.add(testEvent);

      let resultingEvents = calendar.getHistoricalEvents(2016, 11, 2);

      expect.int(List.length(resultingEvents)).toBe(
        List.length(initialEvents) + 1,
      );
    })
    ...
});
```

### Combined setup operations

Sometimes there are setup operations you want to do once before any tests execute as well as operations that you want to do before each test executes.

Suppose that we have a chatbot application that we want to test. We need to resolve the chatbot's credentials (which we can reuse) from a secure location because we don't want to source control them, but we want to ensure that each test runs in a separate session.

```reason
open TestFramework;

type testData = {session: ChatBot.session};

let {describe, describeSkip, describeOnly} =
  describeConfig
  |> withLifecycle(testLifecycle =>
       testLifecycle
       |> beforeAll(() => resolveChatbotCredentials())
       |> beforeEach(credentials => {
            let session = ChatBotAPI.login(credentials);
            session;
          })
     )
  |> afterEach(session => session.close())
  |> build;

describe("My chat bot", ({test}) => {
  test("My test", ({expect, env}) => {
    let myChatBot = MyChatBot.init(env.session);

    myChatBot.message(MyChatBot.user, "hello self!");
    let receivedMessages = myChatBot.getReceivedMessages();

    expect.list(receivedMessages).toContain("hello self!");
  })
});
```


