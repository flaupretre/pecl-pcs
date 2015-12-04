--TEST--
Register a data script using an invalid path ( contains '.')
--EXTENSIONS--
pcs
--INI--
ptest.test_case=6
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
===DONE===
--EXPECT--
PHP Fatal error:  Cannot create node named '.' or '..' in Unknown on line 0

Fatal error: Cannot create node named '.' or '..' in Unknown on line 0
