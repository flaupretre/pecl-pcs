--TEST--
Register a data script (suffix=php, load=none)
--EXTENSIONS--
pcs
--INI--
ptest.test_case=3
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
var_dump(class_exists("PCS_Test\\data1"));

require('pcs://ext/ptest/data1.php');

var_dump(class_exists("PCS_Test\\data1"));
?>
===DONE===
--EXPECT--
Registered data1 as ext/ptest/data1.php => ext/ptest/data1.php
bool(false)
bool(true)
===DONE===
