--TEST--
Autoload an extension class
--EXTENSIONS--
pcs
--SKIPIF--
<?php if (!extension_loaded("ex1")) print "skip"; ?>
--FILE--
<?php
$obj = new Example1\Dummy1();
$obj->hello();
?>
===DONE===
--EXPECT--
Hello World !
===DONE===
