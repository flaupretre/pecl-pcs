--TEST--
__autoload() is functional
--FILE--
<?php

function __autoload($sym)
{
var_dump($sym);
}

class_exists('undefined');

?>
===DONE===
--EXPECTF--
string(9) "undefined"
===DONE===
