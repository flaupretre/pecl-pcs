--TEST--
Display information about virtual files (text)
--FILE--
<?php
PCS\Display::showFiles('text');
?>
===DONE===
--EXPECTF--
--------------------------------------------------
|            Virtual path             | Size | L |
|-------------------------------------+------+---|
| internal/parser/ParserInterface.php | %s | - |
| internal/parser/StringParser.php    | %s | - |
| internal/tools/Display.php          | %s | A |
| internal/tools/embed.php            | %s | - |
===DONE===
