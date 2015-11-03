--TEST--
Attempt to register a script at an already-registered path
--EXTENSIONS--
pcs
--INI--
ptest.load_code1=1
ptest.load_code2=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
===DONE===
--EXPECT--
PHP Fatal error:  ext/ptest/Dummy1.php: Cannot create file, a file with that name does already exist in Unknown on line 0
Loaded code1 set

Fatal error: ext/ptest/Dummy1.php: Cannot create file, a file with that name does already exist in Unknown on line 0