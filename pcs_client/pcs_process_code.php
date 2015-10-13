<?php
/*
  +----------------------------------------------------------------------+
  | PCS extension <http://pcs.tekwire.net>                               |
  +----------------------------------------------------------------------+
  | Copyright (c) 2015 The PHP Group                                     |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Francois Laupretre <francois@tekwire.net>                    |
  +----------------------------------------------------------------------+
*/

//=============================================================================

class VFile
{

private $data; /* File contents */
private $path;
private $vpath;

/*------------------------------------------------------------------*/

public function __construct($path, $vpath)
{
	$this->path = $path;
	$this->vpath = $vpath;

	$this->data = file_get_contents($path);
	if ($this->data === FALSE) {
		throw new Exception($path.": Cannot read file");
	}
}

/*------------------------------------------------------------------*/

private function is_php_source()
{
	return ((strlen($this->path) > 3) && (strtolower(substr($this->path, -4)) == '.php'));
}

/*------------------------------------------------------------------*/
/**
* Removes whitespaces from file contents while preserving line numbers.
*
* Adapted from composer
*/

public function strip()
{
	if (! $this->is_php_source()) return;

	$res = '';

	foreach (token_get_all($this->data) as $token)
			{
			if (is_string($token))
					{
					$res .= $token;
					}
			elseif (in_array($token[0], array(T_COMMENT, T_DOC_COMMENT)))
					{
					$res .= str_repeat("\n", substr_count($token[1], "\n"));
					}
			elseif (T_WHITESPACE === $token[0])
					{
					// reduce wide spaces
					$whitespace = preg_replace('{[ \t]+}', ' ', $token[1]);
					// normalize newlines to \n
					$whitespace = preg_replace('{(?:\r\n|\r|\n)}', "\n", $whitespace);
					// trim leading spaces
					$whitespace = preg_replace('{\n +}', "\n", $whitespace);
					$res .= $whitespace;
					}
			else
					{
					$res .= $token[1];
					}
			}

	$this->data = $res;
}

/*------------------------------------------------------------------*/
/* Returns the definition of the string containing the data */

function dump_data($prefix, $index)
{
	$ret="static char ".$prefix.'_string'.$index."[]= { /* ".$this->vpath." */\n  ";
	$len = strlen($this->data);
	if ($len) {
		for ($i = 0; $i < $len; $i++) {
			$ret .= ord($this->data[$i]).', ';
			if ($i % 16 == 15) {
				$ret .= "\n  ";
			}
		}
	}
	$ret .= "0 };\n\n";
	
	return $ret;
}

/*------------------------------------------------------------------*/
/* Return the PCS_DESCRIPTOR struct definition */

function dump_descriptor($prefix, $index)
{
	return "  { ".$prefix.'_string'.$index.", ".strlen($this->data)
		.", \"".$this->vpath."\", ".strlen($this->vpath)." },\n";
}

/*------------------------------------------------------------------*/
} // End of class
//=============================================================================

function dump_files($prefix, $table)
{
	$ret = '';
	foreach($table as $index => $file) {
		$file->strip();
		$ret .= $file->dump_data($prefix, $index);
	}

	$ret .= "\n";
	if ($GLOBALS['static']) {
		$ret .= 'static ';
	}
	$ret .= "PCS_DESCRIPTOR ".$prefix."[".(count($table) + 1)."] = {\n";
	foreach($table as $index => $file) {
		$ret .= $file->dump_descriptor($prefix, $index);
	}
	$ret .= "  { NULL }\n};\n";

	return $ret;
}

/*------------------------------------------------------------------*/

function register_path($path, $vpath, &$table)
{
	$type = filetype($path);
	if ($type === false) {
		throw new Exception($path.": Cannot get file type");
	}

	switch($type) {
		case 'dir':
			foreach(scandir($path) as $entry) {
				if (($entry === '.') || ($entry === '..')) {
					continue;
				}
				$epath = $path.DIRECTORY_SEPARATOR.$entry;
				$evpath = $vpath.'/'.$entry;
				register_path($epath, $evpath, $table);
			}
			break;
	
		case 'file':
			$table[] = new VFile($path, $vpath);
			break;
	
		default:
			echo $path.": Unsupported file type ($type) - ignoring\n";
	}
}

/*------------------------------------------------------------------*/
// MAIN

$GLOBALS['static'] = false;

$prog = basename($argv[0]);

if ($argc < 5) {
	throw new Exception("Usage: ".$argv[0]." [-s] <input-path> <virtual-path> <var> <output-file>\n");
}

if ($argv[1] == '-s') {
	$GLOBALS['static'] = true;
	array_shift($argv);
}

$ipath = $argv[1];
$vpath = $argv[2];
$prefix = $argv[3];
$opath = $argv[4];

if ($vpath == '') {
	throw new Exception("Virtual path cannot be root path (empty)");
}

$table = array();

register_path($ipath, $vpath, $table);

$output = 
"/* This file was generated by the $prog utility. */
/*-------------------- DO NOT MODIFY --------------------*/\n\n"
	. dump_files($prefix, $table);

file_put_contents($opath, $output);

//=============================================================================
?>
