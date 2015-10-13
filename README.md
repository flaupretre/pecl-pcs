# --------------- Work in progress -------------- #

This project is not ready for distribution yet.

----------


[![Build Status](https://travis-ci.org/flaupretre/pecl-pcs.svg?branch=master)](https://travis-ci.org/flaupretre/pecl-pcs)
[![GitHub release](https://img.shields.io/github/release/flaupretre/pecl-pcs.svg)](https://pecl.php.net/package/pcs)
[![License](https://img.shields.io/badge/License-PHP-blue.svg)](http://php.net/license/3_01.txt)

This project is a [PHP PECL extension](http://pecl.php.net/package/PCS "PHP PECL extension").

This extension provides services available to other extensions, allowing
them to program part of their features in PHP.

Traditionnally, extensions are fully written in C, but there are a lot of cases
where it would be much more interesting, mostly in terms of maintenance, to
code parts in PHP. Today, providing parts of an extension in PHP is possible but
rather complex and almost no extension is using this approach.

PCS makes it much easier as the interaction with the 'client' extension is reduced
to the bare minimum : a client extension just needs to register his PHP script(s)
in PCS once at MINIT time. That's all. Then the classes contained in the registered
PHP code will be automatically autoloaded everytime they are referenced.

Please download releases from the [PECL website](https://pecl.php.net/package/PCS).