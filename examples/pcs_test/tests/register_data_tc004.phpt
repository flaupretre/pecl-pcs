--TEST--
Register a data script (suffix=txt, autoload=force)
--EXTENSIONS--
pcs
--INI--
ptest.test_case=4
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
var_dump(class_exists("PCS_Test\\data1"));
?>
===DONE===
--EXPECT--
Registered data1 as ext/ptest/data1.txt => ext/ptest/data1.txt
bool(true)
===DONE===
