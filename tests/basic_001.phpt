--TEST--
Load extension and quit (no registration)
--EXTENSIONS--
pcs
--INI--
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
===DONE===
--EXPECT--
===DONE===
