--TEST--
Attempt to register an external file tree from non-existing path
--EXTENSIONS--
pcs
--INI--
ptest.test_case=22
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
===DONE===
--EXPECTF--
PHP Fatal error:  ./unexisting/src/code1: No such file or directory (errno=%d) in Unknown on line 0

Fatal error: ./unexisting/src/code1: No such file or directory (errno=%d) in Unknown on line 0