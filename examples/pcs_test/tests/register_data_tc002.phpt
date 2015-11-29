--TEST--
Register a data script (suffix=txt, load=rinit)
--EXTENSIONS--
pcs
--INI--
ptest.test_case=2
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
var_dump(class_exists("PCS_Test\\data1", false));
?>
===DONE===
--EXPECT--
Registered data1 as ext/ptest/data1.php => ext/ptest/data1.php
bool(true)
===DONE===
