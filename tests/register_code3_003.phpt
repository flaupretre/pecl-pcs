--TEST--
Register a script containing a function (load=auto)
--EXTENSIONS--
pcs
--INI--
ptest.load_code3=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
greet();
?>
===DONE===
--EXPECT--
Loaded code3 set
Hello World !
===DONE===
