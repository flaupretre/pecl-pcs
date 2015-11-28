--TEST--
Display information about virtual files (text)
--EXTENSIONS--
pcs
--INI--
ptest.load_code1=1
ptest.load_code3=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
PCS\Display::showFiles('text');
?>
===DONE===
--EXPECTF--
Loaded code1 set
Loaded code3 set
--------------------------------------------------
|            Virtual path             | Size | L |
|-------------------------------------+------+---|
| ext/ptest/Dummy1.php                | 163  | A |
| ext/ptest/Dummy2.php                | 169  | A |
| ext/ptest/constclass.txt            | 101  | R |
| ext/ptest/dir/Dummy4.php            | 138  | A |
| ext/ptest/dir/Dummy5.php            | 166  | A |
| ext/ptest/empty.txt                 | 0    | - |
| ext/ptest/func.txt                  | 54   | R |
| ext/ptest/nosymbols.txt             | 29   | - |
| ext/ptest/test.txt                  | 11   | - |
| ext/ptest/unregistered.txt          | 253  | - |
| internal/parser/ParserInterface.php | %s | - |
| internal/parser/StringParser.php    | %s | - |
| internal/tools/Display.php          | %s | A |
| internal/tools/embed.php            | %s | - |
===DONE===
