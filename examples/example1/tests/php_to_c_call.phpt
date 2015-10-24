--TEST--
Call C code from PHP
--EXTENSIONS--
pcs
--SKIPIF--
<?php if (!extension_loaded("ex1")) print "skip"; ?>
--FILE--
<?php

$obj = new Example1\Dummy4();
var_dump($obj->compute(10, 20));
?>
===DONE===
--EXPECT--
int(30)
===DONE===
