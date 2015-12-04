--TEST--
Call C code from PHP
--EXTENSIONS--
pcs
--INI--
ptest.load_code1=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php

$obj = new PCS_Test\Dummy4();
var_dump($obj->compute(10, 20));
?>
===DONE===
--EXPECT--
Loaded code1 set
int(30)
===DONE===
