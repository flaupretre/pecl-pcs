/*
  +----------------------------------------------------------------------+
  | Compatibility macros for different PHP versions                      |
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

#ifndef _COMPAT_H
#define _COMPAT_H

#include "php.h"
#include "zend.h"
#include "zend_extensions.h"

#if PHP_MAJOR_VERSION >= 7
#define PHP_7
#endif

#include "src/misc.h"
#include "src/zend_string.h"
#include "src/zend_hash.h"

#endif /* _COMPAT_H */
