--TEST--
Attempt to register a script at an already-registered path
--EXTENSIONS--
pcs
--INI--
ptest.load_code1=1
ptest.load_code2=1
ptest.load_messages=0
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
# Need ptest.load_messages=0 because lines are not displayed in the same
# order in PHP 5 & 7.
?>
===DONE===
--EXPECT--
PHP Fatal error:  ext/ptest/Dummy1.php: Cannot create file, a file with that name does already exist in Unknown on line 0

Fatal error: ext/ptest/Dummy1.php: Cannot create file, a file with that name does already exist in Unknown on line 0