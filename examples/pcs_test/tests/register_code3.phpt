--TEST--
Register a non-empty descriptor set with no autoloaded symbol
--EXTENSIONS--
pcs
--INI--
ptest.load_code3=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
===DONE===
--EXPECT--
Loaded code3 set
===DONE===
