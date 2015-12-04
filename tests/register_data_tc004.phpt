--TEST--
Register a data script (suffix=txt, load=autoload)
--EXTENSIONS--
pcs
--INI--
ptest.test_case=4
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
var_dump(class_exists("PCS_Test\\data1", false));
var_dump(class_exists("PCS_Test\\data1", true));
?>
===DONE===
--EXPECT--
Registered data1 as ext/ptest/data1.txt => ext/ptest/data1.txt
bool(false)
bool(true)
===DONE===
