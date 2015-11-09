--TEST--
Check the PCS stream wrapper
--FILE--
<?php

var_dump(filetype("pcs://internal/Parser"));

var_dump(filetype("pcs://internal/Parser/ParserInterface.php"));

var_dump(file_exists("pcs://internal/Parser/ParserInterface.php"));
var_dump(file_exists("pcs://internal/../internal/Parser/ParserInterface.php"));
var_dump(file_exists("pcs://internal/Parser/ParserInterface.php/."));
var_dump(file_exists("pcs://./internal/Parser/../Parser"));
var_dump(file_exists("pcs://./internal/Parser/../Parser/."));
var_dump(file_exists("pcs://*/Parser/ParserInterface.php"));

var_dump(file_exists("pcs://internal/none/none"));

var_dump(file_exists("pcs://internal/Parser/ParserInterface.php/unknown"));

var_dump(scandir("pcs://internal/Parser"));

//-----
echo "--- fopen/fread/fseek\n";

$path = "pcs://internal/Parser/StringParser.php";
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
string(3) "dir"
string(4) "file"
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
array(2) {
  [0]=>
  string(19) "ParserInterface.php"
  [1]=>
  string(16) "StringParser.php"
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
