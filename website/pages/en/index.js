/**
 * Copyright (c) 2017-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

const React = require('react');

const CompLibrary = require('../../core/CompLibrary.js');

const MarkdownBlock = CompLibrary.MarkdownBlock; /* Used to read markdown */
const Container = CompLibrary.Container;
const GridBlock = CompLibrary.GridBlock;


const PastelExample = `re
<Pastel bold=true color=Pastel.Yellow>
  "This will be formatted as bold, yellow text"
</Pastel>
`;

const ConsoleExample = `re
Console.log(4.6);
Console.log("This is a string");
Console.log(a => a + 1);
`;

const FilePrinterExample = `re
FCP.printFile(
  "src/file-context-printer/test/DummyFile.re",
  ((7, 1), (7, 11)),
);
`;

const RefmterrExample = `bash
refmterr dune build -p my-project
`

const RelyExample = `re
describe("FnMatchers", describeUtils => {
  let test = describeUtils.test;
  test("should work for multiple return types", ({expect}) => {
    expect.fn(() => 7).not.toThrow();
    expect.fn(() => "this should be defined as well").not.toThrow();
    expect.fn(() => 42.42).not.toThrow();
  });
});
`

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
        {siteConfig.title}
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

    const Description = ({ title, content, example }) => (
      <Block>
        {[
          {
            content,
            title
          }, {
            content:
              `\`\`\`${example}\`\`\``
          }
        ]}
      </Block>
    );

    return (
      <div>
        <HomeSplash siteConfig={siteConfig} language={language} />
        <div className="mainContainer" style={{ padding: 0 }}>
          <Description
            title="Pastel"
            content="Terminal highlighting with support for nesting and style propagation."
            example={PastelExample}
          />
          <Description
            title="Console"
            content="A web-influenced polymorphic console API for native Console.log(anything) with runtime printing."
            example={ConsoleExample}
          />
          <Description
            title="File Context Printer"
            content="Utility for displaying snippets of files on the command line."
            example={FilePrinterExample}
          />
          <Description
            title="Refmterr"
            content="Utility for extracting structure from unstructured ocaml compiler errors, and displaying them."
            example={RefmterrExample}
          />
          <Description
            title="Rely"
            content="Fast, native, Jest-style test framework"
            example={RelyExample}
          />
        </div>
      </div>
    );
  }
}

module.exports = Index;
