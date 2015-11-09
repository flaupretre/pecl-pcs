This is a set of C include files, whose objective is to ease the pain of writing C code compatible with PHP 5 and 7.

The reason behind this is that, without such a library, and with the exception of pure 'bridge' extensions, it is almost impossible to write code compatible between PHP 5 & 7. So, the only possibility, for extension maintainers, is to duplicate code in two subdirectories, which is a maintenance headache. The goal, here, is to keep a single source tree without 'ifdef' nightmare.

Scope : PHP versions 5.3 and above

Main features :

- A set of miscellaneous macros.

- A zend_string library

- A compatibility library making most new zend_hash features available in PHP 5.
