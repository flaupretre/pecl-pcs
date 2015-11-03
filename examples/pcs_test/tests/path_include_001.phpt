--TEST--
Include a pcs stream-wrapped path
--EXTENSIONS--
pcs
--INI--
ptest.load_code1=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php

var_dump(class_exists("PCS_Test\\Dummy1", false));

require('pcs://ext/ptest/Dummy1.php');

var_dump(class_exists("PCS_Test\\Dummy1", false));
?>
===DONE===
--EXPECT--
Loaded code1 set
bool(false)
bool(true)
===DONE===
