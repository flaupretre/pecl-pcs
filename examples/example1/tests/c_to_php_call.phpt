--TEST--
Autoload and call PHP code from C
--EXTENSIONS--
pcs
--SKIPIF--
<?php if (!extension_loaded("ex1")) print "skip"; ?>
--FILE--
<?php

var_dump(class_exists("Example1\\Dummy5", false));

var_dump(ex1_c_to_php_test("Dummy message"));
?>
===DONE===
--EXPECT--
bool(false)
Hello from Dummy5 - Dummy message
int(-11016323)
===DONE===
