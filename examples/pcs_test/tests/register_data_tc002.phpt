--TEST--
Register a data script (suffix=txt, autoload=auto)
--EXTENSIONS--
pcs
--INI--
ptest.test_case=2
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
var_dump(class_exists("PCS_Test\\data1"));

require('pcs://ext/ptest/data1.txt');

var_dump(class_exists("PCS_Test\\data1"));
?>
===DONE===
--EXPECT--
Registered data1 as ext/ptest/data1.txt => ext/ptest/data1.txt
bool(false)
bool(true)
===DONE===
