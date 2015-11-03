--TEST--
Register an empty descriptor set and exit
--EXTENSIONS--
pcs
--INI--
ptest.load_empty=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
===DONE===
--EXPECT--
Loaded empty set
===DONE===
