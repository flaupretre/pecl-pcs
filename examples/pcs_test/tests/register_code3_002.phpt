--TEST--
Register a non-empty descriptor set forcing autoload
--EXTENSIONS--
pcs
--INI--
ptest.test_case=30
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
var_dump(class_exists('Unregistered'));
?>
===DONE===
--EXPECT--
Loaded code3 set (autoload force)

bool(true)
===DONE===
