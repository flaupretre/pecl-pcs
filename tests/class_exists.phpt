--TEST--
Autoload extension class through class_exists()
--EXTENSIONS--
pcs
--INI--
ptest.load_code1=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
var_dump(class_exists("PCS_Test\\Dummy2", false));
var_dump(class_exists("PCS_Test\\Dummy2"));
?>
===DONE===
--EXPECT--
Loaded code1 set
bool(false)
bool(true)
===DONE===
