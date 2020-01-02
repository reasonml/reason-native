/**
 * Copyright (c) 2017-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import React from 'react';
import Ansi from 'ansi-to-react';
import classnames from 'classnames';
import useBaseUrl from '@docusaurus/useBaseUrl';
import useDocusaurusContext from '@docusaurus/useDocusaurusContext';

import Layout from '@theme/Layout';
import CodeBlock from '@theme/CodeBlock';

import styles from './index.module.css';

const PastelExample = `
<Pastel bold=true color=Pastel.Yellow>
  "This will be formatted as bold, yellow text"
</Pastel>
`;
const PastelExampleOutput = `
[33m[1mThis will be formatted as bold, yellow text[22m[39m
`;

const ConsoleExample = `
Console.log(4.6);
Console.log("This is a string");
Console.log(a => a + 1);
`;
const ConsoleExampleOutput = `
4.6
This is a string
closure(2182546092)
`;

const FilePrinterExample = `
FCP.printFile(
  "src/file-context-printer/test/DummyFile.re",
  ((7, 1), (7, 11)),
);
`;
const FilePrinterExampleOutput = `
[2m 4 ┆ [22m[2m * [22m[34m[2mThis[22m[39m[2m source code is licensed under the ...
[2m 5 ┆ [22m[2m * [22m[34m[2mLICENSE[22m[39m[2m file in the root directory of ...
[2m 6 ┆ [22m[2m */;[22m
[31m[2m 7 ┆ [22m[39m[31m[1m[4mlet myFunc[24m[22m[39m[2mtion = ()[22m[31m[2m => [22m[39m[2mprint_endline([22m[32m[2m"do something"[22m[39m[2m);[22m
[2m 8 ┆ [22m
[2m 9 ┆ [22m[35m[2mlet[22m[39m[2m ex = (arg)[22m[31m[2m => [22m[39m[2m{[22m
[2m10 ┆ [22m[2m    [22m[33m[2mif[22m[39m[2m(arg === true) {[22m
`;

const RefmterrExample = `
refmterr dune build -p my-project
`;
const RefmterrExampleOutput = `
[1m[2m# Unformatted Error Output:[22m[22m
[2m# [22m[2mFile "myFile.ml", line 2, characters 13-21:[22m
[2m# [22m[2mError: Unbound type constructor whereAmI[22m


[31m[1m[7m ERROR [27m[22m[39m [36m[4mmyFile.ml[24m[39m[2m[4m:2 13-21[24m[22m

[2m1 ┆ [22m[35m[2mtype[22m[39m[2m asd =[22m
[31m[2m2 ┆ [22m[39m[2m  | [22m[34m[2mHello[22m[39m[2m of [22m[31m[1m[4mwhereAmI[24m[22m[39m
[2m3 ┆ [22m[2m  | [22m[34m[2mGoodbye[22m[39m

The type [31m[1mwhereAmI[22m[39m can't be found.
`;

const RelyExample = `
describe("Example", ({test}) => {
  test("ints", ({expect}) =>
    expect.int(7).toBe(5)
  );
  test("bools", ({expect}) =>
    expect.bool(true).not.toBe(false)
  );
});
`;
const RelyExampleOutput = `
Running 1 test suite

[31m[1m[7m FAIL [27m[22m[39m Example[39m
[1m[31m  • Example › ints[39m[22m

    [2mexpect.int([22m[31mreceived[39m[2m).toBe([22m[32mexpected[39m[2m)[22m

    Expected: [32m3[39m
    Received: [31m2[39m

      [2m 3 ┆ [22m
      [2m 4 ┆ [22m[2mdescribe([22m[32m[2m"Example"[22m[39m[2m, ({test})[22m[31m[2m => [22m[39m[2m{[22m
      [2m 5 ┆ [22m[2m  test([22m[32m[2m"ints"[22m[39m[2m, ({expect})[22m[31m[2m => [22m[39m[2m{[22m
      [31m[2m 6 ┆ [22m[39m[2m    [22m[31m[1m[4mexpect.int(1 + 1).toBe(3)[24m[22m[39m[2m;[22m
      [2m 7 ┆ [22m[2m    ();[22m
      [2m 8 ┆ [22m[2m  });[22m
      [2m 9 ┆ [22m[2m  test([22m[32m[2m"bools"[22m[39m[2m, ({expect})[22m[31m[2m => [22m[39m[2m{[22m

      [2mRaised by primitive operation at [22m[36m./tests/ExampleTest.re:6:4[39m
      [2mCalled from [22m[36m./map.ml:291:20[39m
      [2mCalled from [22m[36m./common/CommonCollections.re:171:8[39m


Test Suites: [22m[39m[31m[1m1 failed[22m[39m, 0 passed, 1 total
Tests:       [22m[39m[31m[1m1 failed[22m[39m, [32m[1m1 passed[22m[39m, 2 total
Time:        [22m[39m0.002s
`;

function Index() {
  const { siteConfig = {} } = useDocusaurusContext();

  const Block = ({ title, content, example, output, doc, language = '' }) => {
    return (
      <div className={classnames('row', styles.blockBorder, styles.blockPadding)}>
        <div className="col col--6">
          <h3>{title}</h3>
          <p>{content}</p>
          <a className="button button--outline button--lg button--secondary" href={useBaseUrl(`docs/${doc}`)} target="_self">
            Learn More
          </a>
        </div>
        <div className="col col--6">
          <div className={styles.codeBlockMargin}>
            <CodeBlock className={classnames(`language-${language}`)}>{example}</CodeBlock>
          </div>
          <div className="output">
            <pre>
              <Ansi>{output.trim()}</Ansi>
            </pre>
          </div>
        </div>
      </div>
    );
  };

  return (
    <Layout>
      <div className={classnames('hero', styles.heroBanner)}>
        <div className="container">
          <img alt={siteConfig.title} src="./img/full-inverted.svg" style={{ height: '8rem' }} />
          <p className={classnames('hero__subtitle', styles.heroSubtitleColor)}>{siteConfig.tagline}</p>
        </div>
      </div>
      <div className="container">
        <Block
          title="Rely"
          doc="rely/introduction"
          content="Fast, native, Jest-style test framework."
          language="reason"
          example={RelyExample}
          output={RelyExampleOutput}
        />
        <Block
          title="Console"
          doc="console/introduction"
          content="A web-influenced polymorphic console API for native Console.log(anything) with runtime printing."
          language="reason"
          example={ConsoleExample}
          output={ConsoleExampleOutput}
        />
        <Block
          title="Pastel"
          doc="pastel/introduction"
          content="Terminal highlighting with support for nesting and style propagation."
          language="reason"
          example={PastelExample}
          output={PastelExampleOutput}
        />
        <Block
          title="Refmterr"
          doc="refmterr/introduction"
          content="Utility for extracting structure from unstructured ocaml compiler errors, and displaying them."
          language="bash"
          example={RefmterrExample}
          output={RefmterrExampleOutput}
        />
        <Block
          title="File Context Printer"
          doc="file-context-printer/introduction"
          content="Utility for displaying snippets of files on the command line."
          language="reason"
          example={FilePrinterExample}
          output={FilePrinterExampleOutput}
        />
      </div>
    </Layout>
  );
}

export default Index;
