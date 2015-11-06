--TEST--
Register a non-empty descriptor set with no autoloaded symbol
--EXTENSIONS--
pcs
--INI--
ptest.load_code3=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
var_dump(class_exists('Unregistered'));
?>
===DONE===
--EXPECT--
Loaded code3 set
bool(false)
===DONE===
