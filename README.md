# ------ Warning: Alpha version ------ #

### This project is not ready for production yet

----------


[![Build Status](https://travis-ci.org/flaupretre/pecl-pcs.svg?branch=master)](https://travis-ci.org/flaupretre/pecl-pcs)
[![GitHub release](https://img.shields.io/github/release/flaupretre/pecl-pcs.svg)](https://pecl.php.net/package/pcs)
[![License](https://img.shields.io/badge/License-PHP-blue.svg)](http://php.net/license/3_01.txt)

This project is a [PHP PECL extension](http://pecl.php.net/package/pcs "PHP PECL extension").

This extension provides services available to other extensions, allowing
a PHP extension developer to write parts of its extension in PHP.

Traditionnally, extensions are written 100% in C, but there are a lot of cases
where it would be much more interesting, especially in terms of maintainability, to mix C and PHP code.
While it is theoritically possible today, it is rather complex in practice and, as far as I know, nobody's doing it.

PCS makes it much easier as interactions with 'client' extensions (the extensions containing PHP code) are reduced
to the bare minimum : a client extension just needs to register its PHP code in PCS once at MINIT time. That's just one line of code. Then the classes contained in the registered
PHP code are automatically autoloaded everytime they are referenced.

For examples of 'client' extensions, please look into the 'examples' subdirectory.

Note that this extension is written for PHP 7. It won't compile under PHP 5.x environments.

Please download releases from the [PECL website](https://pecl.php.net/package/pcs).
