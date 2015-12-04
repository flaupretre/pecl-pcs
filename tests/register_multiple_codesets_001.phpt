--TEST--
Register two non-empty descriptor sets
--EXTENSIONS--
pcs
--INI--
ptest.load_code1=1
ptest.load_code3=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
===DONE===
--EXPECT--
Loaded code1 set
Loaded code3 set
===DONE===
