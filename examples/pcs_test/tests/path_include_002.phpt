--TEST--
Include pcs virtual files using non-canonical paths
--EXTENSIONS--
pcs
--INI--
ptest.load_code1=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
function load($path)
{
	echo("Loading $path\n");
	require($path);
}

var_dump(class_exists("PCS_Test\\Dummy1", false));
var_dump(class_exists("PCS_Test\\Dummy2", false));
var_dump(class_exists("PCS_Test\\Dummy3", false));
var_dump(class_exists("PCS_Test\\Dummy4", false));

load('pcs://ext/../ext/ptest/./../ptest/Dummy1.php');
load('pcs://ext/../../../ext/ptest/Dummy2.php');
load('pcs://../../ext/../../../ext/ptest/Dummy3.php');
load('pcs://./ext/.././../../ext/../../../ext/./ptest/Dummy4.php');

var_dump(class_exists("PCS_Test\\Dummy1", false));
var_dump(class_exists("PCS_Test\\Dummy2", false));
var_dump(class_exists("PCS_Test\\Dummy3", false));
var_dump(class_exists("PCS_Test\\Dummy4", false));
?>
===DONE===
--EXPECT--
Loaded code1 set
bool(false)
bool(false)
bool(false)
bool(false)
Loading pcs://ext/../ext/ptest/./../ptest/Dummy1.php
Loading pcs://ext/../../../ext/ptest/Dummy2.php
Loading pcs://../../ext/../../../ext/ptest/Dummy3.php
Loading pcs://./ext/.././../../ext/../../../ext/./ptest/Dummy4.php
bool(true)
bool(true)
bool(true)
bool(true)
===DONE===
