--TEST--
Check existence of Mgr class
--FILE--
<?php

var_dump(class_exists("PCS\\Mgr", false));

?>
===DONE===
--EXPECT--
bool(true)
===DONE===
