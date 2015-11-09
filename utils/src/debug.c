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

#ifdef UT_DEBUG
/*---------------------------------------------------------------*/

#include "php.h"

/*---------------------------------------------------------------*/

#ifdef HAVE_GETTIMEOFDAY
static struct timeval _ut_base_tp;
#endif

/*------------*/

static void ut_dbg_init_time()
{
#ifdef HAVE_GETTIMEOFDAY
	struct timezone tz;

	(void)gettimeofday(&_ut_base_tp,&tz);
#endif
}

/*------------*/

static void ut_dbg_print_time()
{
#ifdef UT_DBG_TIMESTAMPS
#ifdef HAVE_GETTIMEOFDAY
	struct timeval tp;
	struct timezone tz;
	time_t sec;

	(void)gettimeofday(&tp,&tz);
	sec=tp.tv_sec-_ut_base_tp.tv_sec;
	if (ut_is_web()) php_printf("<br>");
	php_printf("<");
	if (sec) php_printf("(%ld s) ",sec);
	else php_printf("(%ld µs) ",(tp.tv_usec-_ut_base_tp.tv_usec));
	memmove(&_ut_base_tp,&tp,sizeof(tp));
#endif
#endif
}

#endif	/* UT_DEBUG */

/*============================================================================*/
