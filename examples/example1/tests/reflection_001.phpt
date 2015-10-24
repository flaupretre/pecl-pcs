--TEST--
Get URI through ExtensionClass:getFileName()
--EXTENSIONS--
pcs
--SKIPIF--
<?php if (!extension_loaded("ex1")) print "skip"; ?>
--FILE--
<?php
$cname = "Example1\\Dummy3";

var_dump(class_exists($cname));

$c = new ReflectionClass($cname);
var_dump($c->getFileName());
?>
===DONE===
--EXPECT--
bool(true)
string(24) "pcs://ext/ex1/Dummy3.php"
===DONE===
