--TEST--
Display information about symbols (html)
--EXTENSIONS--
pcs
--INI--
ptest.load_code1=1
ptest.load_code3=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php
PCS\Display::showSymbols('html');
?>
===DONE===
--EXPECTF--
Loaded code1 set
Loaded code3 set
<table border=1 bordercolor="#BBBBBB" cellpadding=3 cellspacing=0 style="border-collapse: collapse">
<tr><th>Type</th><th>Name</th><th>Virtual path</th><th>Load</th></tr>
<tr><td><center>Constant</center></td><td>MYCONST1</td><td>ext/ptest/constclass.txt</td><td><center>R</center></td></tr>
<tr><td><center>Constant</center></td><td>MYCONST2</td><td>ext/ptest/constclass.txt</td><td><center>R</center></td></tr>
<tr><td><center>Class</center></td><td>MyNS\class1</td><td>ext/ptest/constclass.txt</td><td><center>R</center></td></tr>
<tr><td><center>Class</center></td><td>PCS\Display</td><td>internal/tools/Display.php</td><td><center>A</center></td></tr>
<tr><td><center>Class</center></td><td>PCS_Test\Dummy1</td><td>ext/ptest/Dummy1.php</td><td><center>A</center></td></tr>
<tr><td><center>Class</center></td><td>PCS_Test\Dummy2</td><td>ext/ptest/Dummy2.php</td><td><center>A</center></td></tr>
<tr><td><center>Class</center></td><td>PCS_Test\Dummy4</td><td>ext/ptest/dir/Dummy4.php</td><td><center>A</center></td></tr>
<tr><td><center>Class</center></td><td>PCS_Test\Dummy5</td><td>ext/ptest/dir/Dummy5.php</td><td><center>A</center></td></tr>
<tr><td><center>Function</center></td><td>greet</td><td>ext/ptest/func.txt</td><td><center>R</center></td></tr>
</table>
===DONE===
