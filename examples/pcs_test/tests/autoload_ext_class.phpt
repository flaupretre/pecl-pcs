--TEST--
Autoload an extension class
--EXTENSIONS--
pcs
--INI--
ptest.load_code1=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
$obj = new PCS_Test\Dummy1();
$obj->hello();
?>
===DONE===
--EXPECT--
Loaded code1 set
Hello World !
===DONE===
