/**
 * Copyright (c) 2017-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

import React from 'react';
import Layout from '@theme/Layout';
import useDocusaurusContext from '@docusaurus/useDocusaurusContext';

function Help() {
  const context = useDocusaurusContext();
  const { siteConfig = {} } = context;

  return (
    <Layout
      title={`Hello from ${siteConfig.title}`}
      description="Description will go into a meta tag in <head />"
    >
    <div className="container">
        <div className="row">
          <div className="col col--6 col--offset-3 padding-vert--lg">
            <h1>Need Help?</h1>
            <p>Found an issue? Have a question?</p>

            <h2>File an Issue</h2>
            <p>Check our <a href="https://github.com/facebookexperimental/reason-native/issues">GitHub issues page</a></p>

            <h2>Join the Community</h2>
            <p>Join the <a href="https://discord.gg/reasonml">ReasonML Community Discord</a>!</p>
          </div>
        </div>
      </div>
    </Layout>
  );
}

export default Help;
