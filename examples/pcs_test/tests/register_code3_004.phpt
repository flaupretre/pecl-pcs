--TEST--
Register a script containing constants and class (load=auto)
--EXTENSIONS--
pcs
--INI--
ptest.load_code3=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
var_dump(class_exists("MyNS\\class1", false));

var_dump(MYCONST1);
var_dump(MYCONST2);

var_dump(class_exists("MyNS\\class1", false));
?>
===DONE===
--EXPECT--
Loaded code3 set
bool(true)
string(6) "value1"
string(6) "value2"
bool(true)
===DONE===
