# ------ Warning: Beta version ------ #

### This project is not ready for production yet

----------


[![Build Status](https://travis-ci.org/flaupretre/pecl-pcs.svg?branch=master)](https://travis-ci.org/flaupretre/pecl-pcs)
[![GitHub release](https://img.shields.io/github/release/flaupretre/pecl-pcs.svg)](https://pecl.php.net/package/pcs)
[![License](https://img.shields.io/badge/License-PHP-blue.svg)](http://php.net/license/3_01.txt)

This project is a [PHP PECL extension](http://pecl.php.net/package/pcs "PHP PECL extension").

This extension provides a service to other PHP extensions, allowing
PHP extension developers to write parts of their extensions in PHP.

Generally, extensions are written 100% in C but, in many cases, it would be more efficient, especially in terms of maintainability, to write parts of it in PHP.
While it is theoritically possible today, it is rather complex and, as far as I know, almost never done.

PCS makes it much easier because interactions with 'client' extensions (the extensions mixing C and PHP code) are reduced
to the bare minimum : each client extension just registers its PHP code into PCS once at MINIT time (just once C call). During this registration, PCS scans the PHP code and registers the symbols defined by the registered scripts in its integrated autoloader. Then, the classes contained in the registered
PHP code are automatically autoloaded everytime they are accessed.

For examples of 'client' extensions, please look into the 'examples' subdirectory.

<u>PHP version compatibility</u> : This extension should work with every PHP version above or equal to 5.3.0, including PHP 7.

Releases should be downloaded from the [PECL website](https://pecl.php.net/package/pcs).
