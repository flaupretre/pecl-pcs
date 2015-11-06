--TEST--
Register an external file tree (basic)
--EXTENSIONS--
pcs
--INI--
ptest.test_case=20
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
var_dump(class_exists("PCS_Test\\Dummy4"));
?>
===DONE===
--EXPECT--
Loaded code1 set from external file tree (4 files)
bool(true)
===DONE===
