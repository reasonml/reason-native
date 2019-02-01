/**
 * Copyright (c) 2017-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// See https://docusaurus.io/docs/site-config for all the possible
// site configuration options.

const siteConfig = {
  title: 'Reason Native', // Title for your website.
  tagline: 'Libraries for building and testing native Reason programs.',
  url: 'https://facebookexperimental.github.io', // Your website URL
  baseUrl: '/reason-native/', // Base URL for your project */
  // For github.io type URLs, you would set the url and baseUrl like:
  //   url: 'https://facebook.github.io',
  //   baseUrl: '/test-site/',

  // Used for publishing and more
  projectName: 'reason-native',
  organizationName: 'facebookexperimental',
  // For top-level user or org sites, the organization is still the same.
  // e.g., for the https://JoelMarcey.github.io site, it would be set like...
  //   organizationName: 'JoelMarcey'

  // For no header links in the top nav bar -> headerLinks: [],
  headerLinks: [
    { doc: 'introduction', label: 'Docs' },
    { page: 'help', label: 'Help' },
    // { blog: true, label: 'Blog' },
  ],

  /* path to images for header/footer */
  headerIcon: 'img/logo-inverted.svg',
  footerIcon: 'img/logo-inverted.svg',
  favicon: 'img/logo.png',

  /* Colors for website */
  colors: {
    primaryColor: '#222222',
    secondaryColor: '#444444',
  },

  /* Custom fonts for website */
  /*
  fonts: {
    myFont: [
      "Times New Roman",
      "Serif"
    ],
    myOtherFont: [
      "-apple-system",
      "system-ui"
    ]
  },
  */

  // This copyright info is used in /core/Footer.js and blog RSS/Atom feeds.
  copyright: `Copyright © ${new Date().getFullYear()} Facebook Inc.`,

  highlight: {
    theme: 'atom-one-light',
    hljs: function (hljs) {
      hljs.registerLanguage('reason', require('reason-highlightjs'))
    }
  },

  // Add custom scripts here that would be placed in <script> tags.
  scripts: ['/reason-native/js/ansi-to-html.js', 'https://buttons.github.io/buttons.js', '/reason-native/js/ansi.js'],

  // On page navigation for the current documentation page.
  onPageNav: 'separate',
  // No .html extensions for paths.
  cleanUrl: true,

  // Open Graph and Twitter card images.
  ogImage: 'img/docusaurus.png',
  twitterImage: 'img/docusaurus.png',
  disableHeaderTitle: true,

  // Show documentation's last contributor's name.
  // enableUpdateBy: true,

  // Show documentation's last update time.
  // enableUpdateTime: true,

  // You may provide arbitrary config keys to be used as needed by your
  // template. For example, if you need your repo's URL...
  repoUrl: 'https://github.com/facebookexperimental/reason-native',
};

module.exports = siteConfig;
