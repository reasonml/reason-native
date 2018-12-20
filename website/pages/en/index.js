/**
 * Copyright (c) 2017-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

const React = require('react');

var AU = require('ansi_up');
var ansi_up = new AU.default;

const ansiBlock = (ansi) => {
  return '<pre class="example-output">' + ansi_up.ansi_to_html(ansi) + '</pre>';
}

const CompLibrary = require('../../core/CompLibrary.js');

const MarkdownBlock = CompLibrary.MarkdownBlock; /* Used to read markdown */
const Container = CompLibrary.Container;
const GridBlock = CompLibrary.GridBlock;


const PastelExample = `re
<Pastel bold=true color=Pastel.Yellow>
  "This will be formatted as bold, yellow text"
</Pastel>
`;
const PastelExampleOutput = `
[33m[1mThis will be formatted as bold, yellow text[22m[39m
`;

const ConsoleExample = `re
Console.log(4.6);
Console.log("This is a string");
Console.log(a => a + 1);
`;
const ConsoleExampleOutput = `
4.6
This is a string
closure(2182546092)
`;

const FilePrinterExample = `re
FCP.printFile(
  "src/file-context-printer/test/DummyFile.re",
  ((7, 1), (7, 11)),
);
`;
const FilePrinterExampleOutput = `
[2m 4 ┆ [22m[2m * [22m[34m[2mThis[22m[39m[2m source code is licensed under the [22m[34m[2mMIT[22m[39m[2m license found in the[22m
[2m 5 ┆ [22m[2m * [22m[34m[2mLICENSE[22m[39m[2m file in the root directory of this source tree.[22m
[2m 6 ┆ [22m[2m */;[22m
[31m[2m 7 ┆ [22m[39m[31m[1m[4mlet myFunc[24m[22m[39m[2mtion = ()[22m[31m[2m => [22m[39m[2mprint_endline([22m[32m[2m"do something"[22m[39m[2m);[22m
[2m 8 ┆ [22m
[2m 9 ┆ [22m[35m[2mlet[22m[39m[2m ex = (arg)[22m[31m[2m => [22m[39m[2m{[22m
[2m10 ┆ [22m[2m    [22m[33m[2mif[22m[39m[2m(arg === true) {[22m
`;

const RefmterrExample = `bash
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

The type [31m[1mwhereAmI[22m[39m can't be found.`;

const RelyExample = `re
describe("Example", describeUtils => {
  let test = describeUtils.test;
  test("ints", ({expect}) =>
    expect.int(7).toBe(5)
  );
  test("bools", ({expect}) =>
    expect.bool(true).not.toBe(false)
  );
});
`;
const RelyExampleOutput = `
[97mExample[39m
[2m[0/2] Pending[22m  [2m[1/2] Passed[22m  [31m[1/2] Failed[39m
[1m[31m  • Example › ints
[39m[22m    [2mexpect.int([22m[31mreceived[39m[2m).toBe([22m[32mexpected[39m[2m)[22m\n‌‌
    Expected: [32m5[39m
    Received: [31m7[39m`

class HomeSplash extends React.Component {
  render() {
    const { siteConfig, language = '' } = this.props;
    const { baseUrl, docsUrl } = siteConfig;
    const docsPart = `${docsUrl ? `${docsUrl}/` : ''}`;
    const langPart = `${language ? `${language}/` : ''}`;
    const docUrl = doc => `${baseUrl}${docsPart}${langPart}${doc}`;

    const SplashContainer = props => (
      <div className="homeContainer" style={{ background: siteConfig.colors.primaryColor, padding: '4rem 0' }}>
        <div className="homeSplashFade">
          <div className="wrapper homeWrapper">{props.children}</div>
        </div>
      </div>
    );

    const ProjectTitle = () => (
      <h2 className="projectTitle" style={{ color: '#FFFFFF' }}>
        <img alt={siteConfig.title} src="./img/full-inverted.svg" style={{ height: '8rem' }} />
        <small>{siteConfig.tagline}</small>
      </h2>
    );

    return (
      <SplashContainer>
        <div className="inner">
          <ProjectTitle siteConfig={siteConfig} />
        </div>
      </SplashContainer>
    );
  }
}

class Index extends React.Component {
  docUrl(doc, language) {
    const baseUrl = this.props.config.baseUrl;
    const docsUrl = this.props.config.docsUrl;
    const docsPart = `${docsUrl ? `${docsUrl}/` : ''}`;
    const langPart = `${language ? `${language}/` : ''}`;
    return `${baseUrl}${docsPart}${langPart}${doc}`;
  }
  render() {
    const { config: siteConfig, language = '' } = this.props;
    const { baseUrl } = siteConfig;

    const Block = props => (
      <div style={{ borderTop: '1px solid #DDD' }}>
        <Container
          padding={['bottom', 'top']}>
          <GridBlock
            align="left"
            contents={props.children}
          />
        </Container>
      </div >
    );

    const Description = ({ title, content, example, output = '', doc }) => (
      <Block>
        {[
          {
            content: content +
              `<br/><br/><a class="button" href="${this.docUrl(doc, this.props.language)}" target="_self">
                Learn More
              </a>`,
            title
          }, {
            content:
              '```' + example + '```\n' +
              ansiBlock(output)
          }
        ]}
      </Block>
    );

    return (
      <div>
        <HomeSplash siteConfig={siteConfig} language={language} />
        <div className="mainContainer examples" style={{ padding: 0 }}>
          <Description
            title="Pastel"
            doc="pastel"
            content="Terminal highlighting with support for nesting and style propagation."
            example={PastelExample}
            output={PastelExampleOutput}
          />
          <Description
            title="Console"
            doc="console"
            content="A web-influenced polymorphic console API for native Console.log(anything) with runtime printing."
            example={ConsoleExample}
            output={ConsoleExampleOutput}
          />
          <Description
            title="File Context Printer"
            doc="file-context-printer"
            content="Utility for displaying snippets of files on the command line."
            example={FilePrinterExample}
            output={FilePrinterExampleOutput}
          />
          <Description
            title="Refmterr"
            doc="refmterr"
            content="Utility for extracting structure from unstructured ocaml compiler errors, and displaying them."
            example={RefmterrExample}
            output={RefmterrExampleOutput}
          />
          <Description
            title="Rely"
            doc="rely"
            content="Fast, native, Jest-style test framework"
            example={RelyExample}
            output={RelyExampleOutput}
          />
        </div>
      </div>
    );
  }
}

module.exports = Index;
