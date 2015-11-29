--TEST--
Display information about virtual files (html)
--FILE--
<?php
PCS\Display::showFiles('html');
?>
===DONE===
--EXPECTF--
<table border=1 bordercolor="#BBBBBB" cellpadding=3 cellspacing=0 style="border-collapse: collapse">
<tr><th>Virtual path</th><th>Size</th><th>Load</th></tr>
<tr><td>internal/parser/ParserInterface.php</td><td>%d</td><td><center>-</center></td></tr>
<tr><td>internal/parser/StringParser.php</td><td>%d</td><td><center>-</center></td></tr>
<tr><td>internal/tools/Display.php</td><td>%d</td><td><center>A</center></td></tr>
<tr><td>internal/tools/embed.php</td><td>%d</td><td><center>-</center></td></tr>
</table>
===DONE===
