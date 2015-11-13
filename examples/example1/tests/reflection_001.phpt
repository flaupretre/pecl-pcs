--TEST--
Get URI through ExtensionClass:getFileName()
--EXTENSIONS--
pcs
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
