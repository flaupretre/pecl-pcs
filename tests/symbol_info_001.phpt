--TEST--
Display information about symbols (text)
--EXTENSIONS--
pcs
--INI--
ptest.load_code1=1
ptest.load_code3=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
PCS\Display::showSymbols('text');
?>
===DONE===
--EXPECTF--
Loaded code1 set
Loaded code3 set
---------------------------------------------------------------
|   Type   |      Name       |        Virtual path        | L |
|----------+-----------------+----------------------------+---|
| Constant | MYCONST1        | ext/ptest/constclass.txt   | R |
| Constant | MYCONST2        | ext/ptest/constclass.txt   | R |
| Class    | MyNS\class1     | ext/ptest/constclass.txt   | R |
| Class    | PCS\Display     | internal/tools/Display.php | A |
| Class    | PCS_Test\Dummy1 | ext/ptest/Dummy1.php       | A |
| Class    | PCS_Test\Dummy2 | ext/ptest/Dummy2.php       | A |
| Class    | PCS_Test\Dummy4 | ext/ptest/dir/Dummy4.php   | A |
| Class    | PCS_Test\Dummy5 | ext/ptest/dir/Dummy5.php   | A |
| Function | greet           | ext/ptest/func.txt         | R |
===DONE===
