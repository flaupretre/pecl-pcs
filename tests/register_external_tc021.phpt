--TEST--
Register a single external file
--EXTENSIONS--
pcs
--INI--
ptest.test_case=21
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
var_dump(class_exists("PCS_Test\\Dummy4"));
var_dump(class_exists("PCS_Test\\Dummy5"));
?>
===DONE===
--EXPECT--
Loaded 1 file
bool(true)
bool(false)
===DONE===
