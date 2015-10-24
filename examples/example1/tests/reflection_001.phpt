--TEST--
Get URI through ExtensionClass:getFileName()
--EXTENSIONS--
pcs
--SKIPIF--
<?php if (!extension_loaded("ex1")) print "skip"; ?>
--FILE--
<?php
$cname = "Example1\\Dummy2";

$c = new ReflectionClass($cname);
var_dump($c->getFileName());
?>
===DONE===
--EXPECT--
string(24) "pcs://ext/ex1/Dummy2.php"
===DONE===
