--TEST--
Display information about symbols (text)
--FILE--
<?php
PCS\Display::showSymbols('text');
?>
===DONE===
--EXPECTF--
--------------------------------------------------------
| Type  |    Name     |        Virtual path        | L |
|-------+-------------+----------------------------+---|
| Class | PCS\Display | internal/tools/Display.php | A |
===DONE===
