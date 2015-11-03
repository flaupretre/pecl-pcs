--TEST--
Register a non-empty descriptor set and exit
--EXTENSIONS--
pcs
--INI--
ptest.load_code1=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
===DONE===
--EXPECT--
Loaded code1 set
===DONE===
