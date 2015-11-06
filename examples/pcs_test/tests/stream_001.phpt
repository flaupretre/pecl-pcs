--TEST--
Test stream wrapper
--EXTENSIONS--
pcs
--INI--
ptest.load_code1=1
ptest.load_code3=1
--SKIPIF--
<?php if (!extension_loaded("pcs")) print "skip"; ?>
--FILE--
<?php

var_dump(filetype("pcs://ext/ptest/dir"));

var_dump(filetype("pcs://ext/ptest/dir/Dummy4.php"));

var_dump(file_exists("pcs://ext/ptest/dir/none"));

var_dump(file_exists("pcs://ext/ptest/none/none"));

var_dump(file_exists("pcs://ext/ptest/dir/Dummy4.php/unknown"));

var_dump(file_get_contents("pcs://ext/ptest/test.txt"));

var_dump(file_get_contents("pcs://ext/ptest/empty.txt"));

var_dump(scandir("pcs://ext/ptest/dir"));

//-----
echo "--- fopen/fread/fseek\n";

$path = "pcs://ext/ptest/dir/Dummy4.php";
$contents = file_get_contents($path);
var_dump(strlen($contents) === filesize($path));

$fp = fopen($path, "rb");

echo "--- read part1\n";
$part1 = fread($fp,10);
echo "--- fseek(-5)\n";
var_dump(fseek($fp, -5, SEEK_CUR));
var_dump(feof($fp));
echo "--- fseek(-50)\n";
var_dump(fseek($fp, -50, SEEK_CUR));
var_dump(feof($fp));
echo "--- seek to end\n";
var_dump(fseek($fp, 0, SEEK_END));
var_dump(feof($fp));
echo "--- check ftell\n";
var_dump(ftell($fp) == filesize($path));
var_dump(fseek($fp, -5, SEEK_END));
var_dump(feof($fp));
echo "--- read part2\n";
var_dump(fseek($fp, 10, SEEK_SET));
$part2 = fread($fp, filesize($path));
var_dump(feof($fp));
echo "--- check concat\n";
$concat = $part1 . $part2;
var_dump($concat === $contents);
echo "--- fclose\n";
var_dump(fclose($fp));

//-----
?>
===DONE===
--EXPECT--
Loaded code1 set
Loaded code3 set
string(3) "dir"
string(4) "file"
bool(false)
bool(false)
bool(false)
string(11) "Code3 test
"
string(0) ""
array(2) {
  [0]=>
  string(10) "Dummy4.php"
  [1]=>
  string(10) "Dummy5.php"
}
--- fopen/fread/fseek
bool(true)
--- read part1
--- fseek(-5)
int(0)
bool(false)
--- fseek(-50)
int(0)
bool(false)
--- seek to end
int(0)
bool(false)
--- check ftell
bool(true)
int(0)
bool(false)
--- read part2
int(0)
bool(true)
--- check concat
bool(true)
--- fclose
bool(true)
===DONE===
