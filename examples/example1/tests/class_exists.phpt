--TEST--
Autoload extension class through class_exists()
--EXTENSIONS--
pcs
--SKIPIF--
<?php if (!extension_loaded("ex1")) print "skip"; ?>
--FILE--
<?php
var_dump(class_exists("Example1\\Dummy2", false));
var_dump(class_exists("Example1\\Dummy2"));
?>
===DONE===
--EXPECT--
bool(false)
bool(true)
===DONE===
