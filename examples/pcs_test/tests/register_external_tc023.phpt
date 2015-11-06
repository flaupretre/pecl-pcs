--TEST--
Register a single external file (disabling autoload)
--EXTENSIONS--
pcs
--INI--
ptest.test_case=23
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
var_dump(class_exists("PCS_Test\\Dummy4"));
var_dump(class_exists("PCS_Test\\Dummy5"));

require("pcs://ext/ptest/d.php");

var_dump(class_exists("PCS_Test\\Dummy4"));
?>
===DONE===
--EXPECT--
Loaded 1 file
bool(false)
bool(false)
bool(true)
===DONE===
