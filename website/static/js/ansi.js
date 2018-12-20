// Turn off ESLint for this file because it's sent down to users as-is.
/* eslint-disable */
window.addEventListener('load', function () {
  var ansi_up = new AnsiUp;
  document.querySelectorAll('.language-sh').forEach(function (block) {
    var parent = block.parentElement;
    parent.className = 'example-output';
    parent.innerHTML = ansi_up.ansi_to_html(block.innerText);
  });
});
