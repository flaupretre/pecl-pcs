--TEST--
Autoload and call PHP code from C
--EXTENSIONS--
pcs
--FILE--
<?php

var_dump(class_exists("Example1\\Dummy5", false));

var_dump(ex1_c_to_php_test("Dummy message"));
?>
===DONE===
--EXPECT--
bool(false)
Hello from Dummy5 - Dummy message
string(32) "57d156c6dbe54efde3f4ee44dca87ad0"
===DONE===
