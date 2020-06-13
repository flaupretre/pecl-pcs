--TEST--
spl_autoload() is functional
--FILE--
<?php

function my_autoload($sym)
{
var_dump($sym);
}
spl_autoload_register('my_autoload');
class_exists('undefined');

?>
===DONE===
--EXPECTF--
string(9) "undefined"
===DONE===
