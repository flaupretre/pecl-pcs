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

interface ParserInterface
{
//---------------------------------
/**
* Extracts symbols from input and returns them as an
* array (first char is symbol type).
*
* @param string $buf The script to parse
* @return array of symbols
* @throws \Exception on parse error
*/

public static function parse($buf);

//---
} // End of class
//===========================================================================
} // End of namespace
//===========================================================================
