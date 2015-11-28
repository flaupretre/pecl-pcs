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

namespace PCS {

class Display // Static only
{
//---------

private static function displayFormat($format=null)
{
	if (is_null($format)||($format==='auto')) {
		$format=((php_sapi_name()=='cli') ? 'text' : 'html');
	}

	$format=strtolower($format);
	if (($format !== 'text') && ($format !== 'html')) {
		throw new \Exception("$format: Unknown display format");
	}

	return $format;
}

//---------

public static function showFiles($format=null, $path_to_url_function=null)
{
	$format = self::displayFormat($format);
	$method = (($format == 'text') ? 'showFilesAsText' : 'showFilesAsHtml');
	self::$method($path_to_url_function);
}

//---------

private static function sortByPath($s1,$s2)
{
	return strcmp($s1['path'],$s2['path']);
}

//---------

private static function showFilesAsText($path_to_url_function=null)
{
	$infos = Mgr::fileInfos();
	usort($infos, array(__CLASS__,'sortByPath'));
	
	$size_len=4;
	$path_len=12;
	foreach($infos as $info) {
		$size_len=max($size_len, strlen(strval($info['size']))+2);
		$path_len=max($path_len,strlen($info['path'])+2);
	}

	echo
		 str_repeat('-',$size_len+$path_len+7)."\n"
		,'|'.str_pad('Virtual path',$path_len,' ',STR_PAD_BOTH)
		,'|'.str_pad('Size',$size_len,' ',STR_PAD_BOTH)
		,"| L |\n"
		,'|'.str_repeat('-',$path_len)
		,'+'.str_repeat('-',$size_len)
		,"+---|\n";

	foreach($infos as $info) {
		echo
			 '| '.str_pad($info['path'],$path_len-1,' ',STR_PAD_RIGHT)
			,'| '.str_pad(strval($info['size']),$size_len-1,' ',STR_PAD_RIGHT)
			,'| '.$info['load']
			," |\n";
	}
}

//---
// The same in HTML

private static function showFilesAsHtml($path_to_url_function=null)
{
	$infos = Mgr::fileInfos();
	usort($infos, array(__CLASS__,'sortByPath'));

	echo '<table border=1 bordercolor="#BBBBBB" cellpadding=3 '
		,'cellspacing=0 style="border-collapse: collapse">'."\n";
	echo '<tr><th>Path</th><th>Size</th><th>Load</th></tr>'."\n";
	foreach($infos as $info) {
		echo '<tr><td>';
		if (!is_null($path_to_url_function)) 
			echo '<a href="'.call_user_func($path_to_url_function,$info['path']).'">';
		echo htmlspecialchars($info['path']);
		if (!is_null($path_to_url_function)) echo '</a>';
		echo '</td><td>'.$info['size'].'</td><td><center>'
			.$info['load'].'</center></td>';
		echo "</tr>\n";
	}
	echo "</table>\n";
}

//---
} // End of class
//===========================================================================
} // End of namespace
//===========================================================================
?>
