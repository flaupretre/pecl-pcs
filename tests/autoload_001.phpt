--TEST--
__autoload() is functional
--SKIPIF--
<?php if (PHP_VERSION_ID >= 70200) die("skip PHP < 7.2 only"); ?>
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
