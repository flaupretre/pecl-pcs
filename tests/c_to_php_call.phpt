--TEST--
Autoload and call PHP code from C
--EXTENSIONS--
pcs
--INI--
ptest.load_code1=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php

var_dump(class_exists("PCS_Test\\Dummy5", false));

var_dump(ptest_c_to_php_test("Dummy message"));
?>
===DONE===
--EXPECT--
Loaded code1 set
bool(false)
Hello from Dummy5 - Dummy message
string(32) "57d156c6dbe54efde3f4ee44dca87ad0"
===DONE===
