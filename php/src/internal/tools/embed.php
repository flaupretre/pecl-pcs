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
// <Parser>:ignore-file

//=============================================================================

namespace PCS\Parser {

class File
{
const DESCRIPTOR_VERSION = 0;

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

public static function sortByVpath($f1,$f2)
{
	return strcmp($f1->vpath, $f2->vpath);
}

/*------------------------------------------------------------------*/

private function is_php_source()
{
	return ((strlen($this->data) >= 5) && (substr($this->data, 0, 5)=='<?php'));
}

/*------------------------------------------------------------------*/

private function strip_string($str)
{
	return str_repeat("\n", substr_count($str, "\n"));
}
	
/*------------------------------------------------------------------*/
/**
* Removes whitespaces from file contents while preserving line numbers and
* parser directives.
*
* Adapted from composer
*/

public function strip()
{
	if (! $this->is_php_source()) return;

	$res = '';

	foreach (token_get_all($this->data) as $token) {
		if (is_string($token)) {
				$res .= $token;
		} else {
			switch($token[0]) {
				case T_DOC_COMMENT:
					$res .= self::strip_string($token[1]);
					break;
				case T_COMMENT:
					// Keep parser directives only
					$keep = false;
					if (strlen($token[1]) > 3) {
						$s = ltrim(substr($token[1], 2));
						if ((strlen($s) > 9) && (substr($s, 0, 9) == '<Parser>:')) {
							$keep = true;
						}
					}
					if ($keep) {
						$res .= $token[1];
					} else {
						$res .= self::strip_string($token[1]);
					}
					break;
				case T_WHITESPACE:
					// reduce wide spaces
					$whitespace = preg_replace('{[ \t]+}', ' ', $token[1]);
					// normalize newlines to \n
					$whitespace = preg_replace('{(?:\r\n|\r|\n)}', "\n", $whitespace);
					// trim leading spaces
					$whitespace = preg_replace('{\n +}', "\n", $whitespace);
					$res .= $whitespace;
					break;
				default:
					$res .= $token[1];
			}
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
/* Return the descriptor struct definition */

function dump_descriptor($prefix, $index)
{
	return "  { ".self::DESCRIPTOR_VERSION.", ".$prefix.'_string'.$index
		.", ".strlen($this->data).", \"".$this->vpath."\", "
		.strlen($this->vpath)." },\n";
}

/*------------------------------------------------------------------*/
} // End of class
//=============================================================================

function dump_files($prefix, $table)
{
	usort($table, array("PCS\\Parser\\File", 'sortByVpath'));

	$ret = '';
	foreach($table as $index => $file) {
		$file->strip();
		$ret .= $file->dump_data($prefix, $index);
	}

	$ret .= "\nstatic PCS_DESCRIPTOR ".$prefix."[".(count($table) + 1)."] = {\n";
	foreach($table as $index => $file) {
		$ret .= $file->dump_descriptor($prefix, $index);
	}
	$ret .= "  { ".File::DESCRIPTOR_VERSION.", NULL }\n};\n";

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
				$evpath = (($vpath==='') ? $entry : ($vpath.'/'.$entry));
				register_path($epath, $evpath, $table);
			}
			break;
	
		case 'file':
			$table[] = new File($path, $vpath);
			break;
	
		default:
			echo $path.": Unsupported file type ($type) - ignoring\n";
	}
}

/*------------------------------------------------------------------*/
// MAIN

$prog = basename($argv[0]);

if ($argc < 4) {
	throw new Exception("Usage: ".$argv[0]." <input-path> <var> <output-file>\n");
}

if ($argv[1] == '-s') { /* Ignore obsolete option */
	array_shift($argv);
}

$ipath = $argv[1];
$prefix = $argv[2];
$opath = $argv[3];

$table = array();

register_path($ipath, '', $table);

$output = 
"/* This file was generated by the PCS embed utility */
/*-------------------- DO NOT MODIFY --------------------*/\n\n"
	. dump_files($prefix, $table);

file_put_contents($opath, $output);

//=============================================================================
}
?>
