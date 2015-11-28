--TEST--
Display information about virtual files (html)
--EXTENSIONS--
pcs
--INI--
ptest.load_code1=1
ptest.load_code3=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
PCS\Display::showFiles('html');
?>
===DONE===
--EXPECTF--
Loaded code1 set
Loaded code3 set
<table border=1 bordercolor="#BBBBBB" cellpadding=3 cellspacing=0 style="border-collapse: collapse">
<tr><th>Path</th><th>Size</th><th>Load</th></tr>
<tr><td>ext/ptest/Dummy1.php</td><td>163</td><td><center>A</center></td></tr>
<tr><td>ext/ptest/Dummy2.php</td><td>169</td><td><center>A</center></td></tr>
<tr><td>ext/ptest/constclass.txt</td><td>101</td><td><center>R</center></td></tr>
<tr><td>ext/ptest/dir/Dummy4.php</td><td>138</td><td><center>A</center></td></tr>
<tr><td>ext/ptest/dir/Dummy5.php</td><td>166</td><td><center>A</center></td></tr>
<tr><td>ext/ptest/empty.txt</td><td>0</td><td><center>-</center></td></tr>
<tr><td>ext/ptest/func.txt</td><td>54</td><td><center>R</center></td></tr>
<tr><td>ext/ptest/nosymbols.txt</td><td>29</td><td><center>-</center></td></tr>
<tr><td>ext/ptest/test.txt</td><td>11</td><td><center>-</center></td></tr>
<tr><td>ext/ptest/unregistered.txt</td><td>253</td><td><center>-</center></td></tr>
<tr><td>internal/parser/ParserInterface.php</td><td>%d</td><td><center>-</center></td></tr>
<tr><td>internal/parser/StringParser.php</td><td>%d</td><td><center>-</center></td></tr>
<tr><td>internal/tools/Display.php</td><td>%d</td><td><center>A</center></td></tr>
<tr><td>internal/tools/embed.php</td><td>%d</td><td><center>-</center></td></tr>
</table>
===DONE===
