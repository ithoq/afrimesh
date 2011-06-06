#!/usr/bin/env js

/**
 * Read: http://www.fleegix.org/articles/2008-09-06-jslint-in-vim-through-lynx          DONE
 *       http://mikecantelon.com/story/javascript-syntax-checking-vim                   DONE
 *       http://www.emacswiki.org/emacs/FlymakeJavaScript                               TODO
 *       http://www.corybennett.org/projects/                                           TODO
 * Run:  ./runjslint.js "`cat afrimesh.js`" | lynx --force-html /dev/fd/5 -dump 5<&0
 */

load("./fulljslint.js");

var body = arguments[0];

var result = JSLINT(body, {
    browser: true,
    forin: true
  });
if (result) {
  print('How are you gentlemen. :)');
} else {
  print('Somebody set up us the bomb! :/');
}
print(JSLINT.report());

