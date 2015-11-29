--TEST--
Register a data script (suffix=php, load=auto), fix path
--EXTENSIONS--
pcs
--INI--
ptest.test_case=9
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
var_dump(class_exists("PCS_Test\\data1"));	// autoloaded
?>
===DONE===
--EXPECT--
Registered data1 as /ext/ptest///data1.php/ => ext/ptest/data1.php
bool(true)
===DONE===
