// Turn off ESLint for this file because it's sent down to users as-is.
/* eslint-disable */
window.addEventListener('load', function () {
  var Convert = require('ansi-to-html');
  var convert = new Convert({
    // sourced from https://github.com/nathanbuchar/atom-one-dark-terminal/blob/master/COLORS
    colors: {
      0: '#000',
      1: '#E06C75',
      2: '#98C379',
      3: '#D19A66',
      4: '#61AFEF',
      5: '#C678DD',
      6: '#56B6C2',
      7: '#ABB2BF',
      8: '#5C6370',
      9: '#E06C75',
      10: '#98C379',
      11: '#D19A66',
      12: '#61AFEF',
      13: '#C678DD',
      14: '#56B6C2',
      15: '#FFF'
    }
  });

  document.querySelectorAll('.language-bash-ansi').forEach(function (block) {
    var parent = block.parentElement;
    parent.className = 'example-output';
    parent.innerHTML = convert.toHtml(block.innerText);
  });
});
