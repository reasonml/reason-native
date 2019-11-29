/**
 * Copyright (c) 2017-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// See https://docusaurus.io/docs/site-config for all the possible
// site configuration options.

module.exports = {
  title: 'Reason Native', // Title for your website.
  tagline: 'Libraries for building and testing native Reason programs.',
  url: 'https://reason-native.com', // Your website URL
  baseUrl: '/', // Base URL for your project */
  // For github.io type URLs, you would set the url and baseUrl like:
  //   url: 'https://facebook.github.io',
  //   baseUrl: '/test-site/',

  // Used for publishing and more
  projectName: 'reason-native',
  organizationName: 'facebookexperimental',
  // For top-level user or org sites, the organization is still the same.
  // e.g., for the https://JoelMarcey.github.io site, it would be set like...
  //   organizationName: 'JoelMarcey'

  themeConfig: {
    navbar: {
      title: 'Reason Native',
      logo: {
        alt: 'Reason Native',
        src: 'img/logo-inverted.svg'
      },
      links: [
        { to: 'docs/introduction', label: 'Docs', position: 'left' },
        { to: 'help', label: 'Help', position: 'left' },
        { href: 'https://github.com/facebookexperimental/reason-native', label: 'GitHub', position: 'right' },
        // { to: 'blog', label: 'Blog', position: 'left' }
      ]
    },
    footer: {
      style: 'dark',
      logo: {
        alt: 'Facebook Open Source',
        src: 'img/oss-logo.png'
      },
      links: [
        {
          title: 'Projects',
          items: [
            { to: 'docs/rely/index', label: 'Rely' },
            { to: 'docs/console/index', label: 'Console' },
            { to: 'docs/pastel/index', label: 'Pastel' },
            { to: 'docs/refmterr/index', label: 'Refmterr' },
            { to: 'docs/file-context-printer/index', label: 'File Context Printer' }
          ]
        },
        {
          title: 'More',
          items: [
            { href: 'https://github.com/facebookexperimental/reason-native', label: 'GitHub' },
            { html: '<a class="github-button" href="https://github.com/ntkme/github-buttons" data-icon="octicon-star" data-show-count="true" aria-label="Star ntkme/github-buttons on GitHub">Star</a>' }
          ]
        }
      ],
      copyright: `Copyright © ${new Date().getFullYear()} Facebook Inc.`,
    },
    image: 'img/logo.png'
  },

  favicon: 'img/logo.png',

  /* Colors for website */
  // colors: {
  //   primaryColor: '#222222',
  //   secondaryColor: '#444444',
  // },

  // Add custom scripts here that would be placed in <script> tags.
  scripts: ['/js/ansi-to-html.js', 'https://buttons.github.io/buttons.js', '/js/ansi.js'],

  // Show documentation's last contributor's name.
  // enableUpdateBy: true,

  // Show documentation's last update time.
  // enableUpdateTime: true,

  // preset which bundles most of the essential plugins present in Docusaurus 1
  presets: [
    [
      '@docusaurus/preset-classic',
      {
        docs: {
          // docs folder path relative to website dir.
          path: '../docs',
          // sidebars file relative to website dir.
          sidebarPath: require.resolve('./sidebars.json'),
        }
      }
    ]
  ]
};
