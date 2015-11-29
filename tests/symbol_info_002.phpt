--TEST--
Display information about symbols (html)
--FILE--
<?php
PCS\Display::showSymbols('html');
?>
===DONE===
--EXPECTF--
<table border=1 bordercolor="#BBBBBB" cellpadding=3 cellspacing=0 style="border-collapse: collapse">
<tr><th>Type</th><th>Name</th><th>Virtual path</th><th>Load</th></tr>
<tr><td><center>Class</center></td><td>PCS\Display</td><td>internal/tools/Display.php</td><td><center>A</center></td></tr>
</table>
===DONE===
