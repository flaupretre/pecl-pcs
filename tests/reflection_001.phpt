--TEST--
Get URI through ExtensionClass:getFileName()
--EXTENSIONS--
pcs
--INI--
ptest.load_code1=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
$cname = "PCS_Test\\Dummy2";

$c = new ReflectionClass($cname);
var_dump($c->getFileName());
?>
===DONE===
--EXPECT--
Loaded code1 set
string(26) "pcs://ext/ptest/Dummy2.php"
===DONE===
