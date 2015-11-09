/*
  +----------------------------------------------------------------------+
  | Common utility function for PHP extensions                           |
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

#ifndef __PECL_UTILS_DEBUG_H
#define __PECL_UTILS_DEBUG_H 1

#ifdef PHP_WIN32
#include "win32/time.h"
#elif defined(NETWARE)
#include <sys/timeval.h>
#include <sys/time.h>
#else
#include <sys/time.h>
#endif

#ifdef PHP_7
#include "Zend/zend_portability.h"
#endif

/*---------------------------------------------------------------*/

#ifdef UT_DEBUG
#define DBG_INIT() ut_dbg_init_time()
#define DBG_MSG(_format) { ut_dbg_print_time(); php_printf(_format "\n"); }
#define DBG_MSG1(_format,_var1) { ut_dbg_print_time(); php_printf(_format "\n",_var1); }
#define DBG_MSG2(_format,_var1,_var2) { ut_dbg_print_time(); php_printf(_format "\n",_var1,_var2); }
#define DBG_MSG3(_format,_var1,_var2,_var3) { ut_dbg_print_time(); php_printf(_format "\n",_var1,_var2,_var3); }
#define CHECK_MEM()	full_mem_check(1)

static void ut_dbg_init_time();
static void ut_dbg_print_time();

#else
#define DBG_INIT()
#define DBG_MSG(_format)
#define DBG_MSG1(_format,_var1)
#define DBG_MSG2(_format,_var1,_var2)
#define DBG_MSG3(_format,_var1,_var2,_var3)
#define CHECK_MEM()
#endif	/* UT_DEBUG */

/*---------------------------------------------------------------*/

static zend_always_inline void CHECK_ZSTRING(zend_string *zsp)
{
	ZEND_ASSERT(zsp);
	ZEND_ASSERT(ZSTR_VAL(zsp)[ZSTR_LEN(zsp)] == '\0');
}

/*============================================================================*/
#endif	/* __PECL_UTILS_DEBUG_H */
