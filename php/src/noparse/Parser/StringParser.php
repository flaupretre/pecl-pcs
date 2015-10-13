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

namespace PCS\Parser {

include(__DIR__.'/ParserInterface.php');

//=============================================================================
//-- For PHP version < 5.3.0

// <Parser>:ignore constant T_NAMESPACE
if (!defined('T_NAMESPACE')) define('T_NAMESPACE',-2);
// <Parser>:ignore constant T_NS_SEPARATOR
if (!defined('T_NS_SEPARATOR'))	define('T_NS_SEPARATOR',-3);
// <Parser>:ignore constant T_CONST
if (!defined('T_CONST'))	define('T_CONST',-4);
// <Parser>:ignore constant T_TRAIT
if (!defined('T_TRAIT'))	define('T_TRAIT',-5);

//===========================================================================
/**
* This parser extracts symbols from a PHP script received in a string
*
*///==========================================================================

class StringParser implements ParserInterface
{
//-- Symbol types

const T_CLASS='L';
const T_FUNCTION='F';
const T_CONSTANT='C';

//-- Parser states :

const ST_OUT=1;						// Upper level
const ST_FUNCTION_FOUND=self::T_FUNCTION; // Found 'function'. Looking for name
const ST_SKIPPING_BLOCK_NOSTRING=3; // In block, outside of string
const ST_SKIPPING_BLOCK_STRING=4;	// In block, in string
const ST_CLASS_FOUND=self::T_CLASS;	// Found 'class'. Looking for name
const ST_DEFINE_FOUND=6;			// Found 'define'. Looking for '('
const ST_DEFINE_2=7;				// Found '('. Looking for constant name
const ST_SKIPPING_TO_EOL=8;			// Got constant. Looking for EOL (';')
const ST_NAMESPACE_FOUND=9;			// Found 'namespace'. Looking for <whitespace>
const ST_NAMESPACE_2=10;			// Found 'namespace' and <whitespace>. Looking for name
const ST_CONST_FOUND=11;			// Found 'const'. Looking for name

//---------

/** @var array(keys (<1-char symbol type>+<case-sensitive symbol name>)) */

private static $symbols = array();

/** @var array(<key> => null) A list of symbols to exclude */

private static $exclude_list = array();

//---------------------------------------------------------------------------

private static function key($type,$name)
{
	return $type.$name;
}

//---------------------------------

private static function stringToType($str)
{
	switch($str) {
		case 'class':		return self::T_CLASS;
		case 'function':	return self::T_FUNCTION;
		case 'constant':	return self::T_CONSTANT;
	}
}

//---------------------------------

private static function cleanup()
{
	// Filter out excluded symbols
	if (count(self::$exclude_list)) {
		foreach(self::$symbols as $n => $s) {
			if (array_key_exists($s, self::$exclude_list))
				unset(self::$symbols[$n]);
		}
	}

	$a=self::$symbols;
	self::$symbols=array();
	self::$exclude_list=array();

	return $a;
}

//---------------------------------
/**
* Mark a symbol as excluded
*
* @param string $type one of the T_xx constants
* @param string $name The symbol name
* @return null
*/

private static function exclude($type,$name)
{
	self::$exclude_list[self::key($type,$name)] = null;
}

//---------------------------------
/**
* Add a symbol into the table
*
* Filter out the symbol from the exclude list
*
* @param string $type one of the T_xx constants
* @param string $name The symbol name
* @return null
*/

private static function addSymbol($type,$name)
{
	self::$symbols[] = self::key($type, $name);
}

//---------------------------------
/**
* Combine a namespace with a symbol
*
* The leading and trailing backslashes are first suppressed from the namespace.
* Then, if the namespace is not empty it is prepended to the symbol using a
* backslash.
*
* @param string $ns Namespace (can be empty)
* @param string $symbol Symbol name (cannot be empty)
* @return string Fully qualified name without leading backslash
*/

private static function combineNSSymbol($ns,$symbol)
{
	$ns=trim($ns,'\\');
	return $ns.(($ns==='') ? '' : '\\').$symbol;
}

//---------------------------------
/**
* Register explicit declarations
*
* A file may disable indexing and explicitely declare symbols.
*
* Format:
*	<double-slash> <Parser>:declare <type> <value>
*	<double-slash> <Parser>:ignore <type> <value>
*	<double-slash> <Parser>:ignore-file
*	<double-slash> <Parser>:skip-blocks
*
* @return bool false if indexing is disabled on this file
*/

private static function parseDirectives($buf,&$skip_blocks)
{
	$ret=true;
	$a=null;
	if (preg_match_all('{^//\s+\<Parser\>:(\S+)(.*)$}m',$buf,$a,PREG_SET_ORDER)!=0) {
		foreach($a as $match) {
			switch ($cmd=$match[1]) {
				case 'ignore-file':
					$ret=false;

				case 'skip-blocks':
					$skip_blocks=true;
					break;

				case 'declare':
				case 'ignore':
					$type_string=strtolower(strtok($match[2],' '));
					$name=strtok(' ');
					if ($type_string===false || $name===false)
						throw new \Exception($cmd.': Directive needs 2 args');
					$type=self::stringToType($type_string);
					if ($cmd=='declare')
						self::addSymbol($type,$name);
					else
						self::exclude($type,$name);
					break;

				default:
					throw new \Exception($cmd.': Invalid directive');
			}
		}
	}
	return $ret;
}

//---------------------------------
/**
* Extracts symbols from a PHP script contained in a string
*
* @param string $buf The script to parse
* @return array of symbols
* @throws \Exception on parse error
*/

public static function parse($buf)
{
	$buf=str_replace("\r",'',$buf);

	$skip_blocks=false;

	if (self::parseDirectives($buf, $skip_blocks)) {
		self::parseTokens($buf, $skip_blocks);
	}

	return self::cleanup();
}

//---------------------------------
/**
* Extract symbols from script tokens
*/

private static function parseTokens($buf,$skip_blocks)
{
	$block_level=0;
	$state=self::ST_OUT;
	$name='';
	$ns='';

	// Note: Using php_strip_whitespace() before token_get_all does not improve
	// performance.

	foreach(token_get_all($buf) as $token) {
		if (is_string($token)) {
			$tvalue=$token;
			$tnum=-1;
			$tname='String';
		} else {
			list($tnum,$tvalue)=$token;
			$tname=token_name($tnum);
		}

		if (($tnum==T_COMMENT)||($tnum==T_DOC_COMMENT)) continue;
		if (($tnum==T_WHITESPACE)&&($state!=self::ST_NAMESPACE_FOUND)) continue;

		//echo "$tname <$tvalue>\n";//TRACE
		switch($state) {
			case self::ST_OUT:
				switch($tnum)
					{
					case T_FUNCTION:
						$state=self::ST_FUNCTION_FOUND;
						break;
					case T_CLASS:
					case T_INTERFACE:
					case T_TRAIT:
						$state=self::ST_CLASS_FOUND;
						break;
					case T_NAMESPACE:
						$state=self::ST_NAMESPACE_FOUND;
						$name='';
						break;
					case T_CONST:
						$state=self::ST_CONST_FOUND;
						break;
					case T_STRING:
						if ($tvalue=='define') $state=self::ST_DEFINE_FOUND;
						$name='';
						break;
					// If this flag is set, we skip anything enclosed
					// between {} chars, ignoring any conditional block.
					case -1:
						if ($tvalue=='{' && $skip_blocks) {
							$state=self::ST_SKIPPING_BLOCK_NOSTRING;
							$block_level=1;
						}
						break;
				}
				break;

			case self::ST_NAMESPACE_FOUND:
				$state=($tnum==T_WHITESPACE) ? self::ST_NAMESPACE_2 : self::ST_OUT;
				break;

			case self::ST_NAMESPACE_2:
				switch($tnum) {
					case T_STRING:
						$name .=$tvalue;
						break;
					case T_NS_SEPARATOR:
						$name .= '\\';
						break;
					default:
						$ns=$name;
						$state=self::ST_OUT;
				}
				break;


			case self::ST_FUNCTION_FOUND:
				if (($tnum==-1)&&($tvalue=='(')) {
					// Closure : Ignore (no function name to get here)
					$state=self::ST_OUT;
					break;
				}
				 //-- Function returning ref: keep looking for name
				 if ($tnum==-1 && $tvalue=='&') break;
				// No break here !
			case self::ST_CLASS_FOUND:
				if ($tnum==T_STRING) {
					self::addSymbol($state,self::combineNSSymbol($ns,$tvalue));
				} else {
					throw new \Exception('Unrecognized token for class/function definition'
						."(type=$tnum ($tname);value='$tvalue'). String expected");
				}
				$state=self::ST_SKIPPING_BLOCK_NOSTRING;
				$block_level=0;
				break;

			case self::ST_CONST_FOUND:
				if ($tnum==T_STRING) {
					self::addSymbol(self::T_CONSTANT,self::combineNSSymbol($ns,$tvalue));
				}
				else {
					throw new \Exception('Unrecognized token for constant definition'
						."(type=$tnum ($tname);value='$tvalue'). String expected");
				}
				$state=self::ST_OUT;
				break;

			case self::ST_SKIPPING_BLOCK_STRING:
				if ($tnum==-1 && $tvalue=='"')
					$state=self::ST_SKIPPING_BLOCK_NOSTRING;
				break;

			case self::ST_SKIPPING_BLOCK_NOSTRING:
				if ($tnum==-1 || $tnum==T_CURLY_OPEN) {
					switch($tvalue) {
						case '"':
							$state=self::ST_SKIPPING_BLOCK_STRING;
							break;
						case '{':
							$block_level++;
							//TRACE echo "block_level=$block_level\n";
							break;
						case '}':
							$block_level--;
							if ($block_level==0) $state=self::ST_OUT;
							//TRACE echo "block_level=$block_level\n";
							break;
					}
				}
				break;

			case self::ST_DEFINE_FOUND:
				if ($tnum==-1 && $tvalue=='(') {
					$state=self::ST_DEFINE_2;
				} else {
					throw new \Exception('Unrecognized token for constant definition'
						."(type=$tnum ($tname);value='$tvalue'). Expected '('");
				}
				break;

			case self::ST_DEFINE_2:
				// Remember: T_STRING is incorrect in 'define' as constant name.
				// Current namespace is ignored in 'define' statement.
				if ($tnum==T_CONSTANT_ENCAPSED_STRING) {
					$schar=$tvalue{0};
					if ($schar=="'" || $schar=='"') $tvalue=trim($tvalue,$schar);
					self::addSymbol(self::T_CONSTANT,$tvalue);
				} else {
					throw new \Exception('Unrecognized token for constant definition'
						."(type=$tnum ($tname);value='$tvalue'). Expected quoted string constant");
				}
				$state=self::ST_SKIPPING_TO_EOL;
				break;

			case self::ST_SKIPPING_TO_EOL:
				if ($tnum==-1 && $tvalue==';') $state=self::ST_OUT;
				break;
			}
		}
}

//---
} // End of class
//===========================================================================
} // End of namespace
//===========================================================================
