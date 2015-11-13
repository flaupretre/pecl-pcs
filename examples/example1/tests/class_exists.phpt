--TEST--
Autoload extension class through class_exists()
--EXTENSIONS--
pcs
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
