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

#ifndef __PECL_UTILS_MUTEX_H
#define __PECL_UTILS_MUTEX_H 1

/*============================================================================*/

#include "TSRM/TSRM.h"

/*============================================================================*/

/*-- Thread-safe stuff ------*/

#ifdef ZTS
#define MutexDeclare(x)		MUTEX_T x ## _mutex;
#define StaticMutexDeclare(x)	static MUTEX_T x ## _mutex;
#define MutexSetup(x)		{ x ## _mutex = tsrm_mutex_alloc(); }
#define MutexShutdown(x)	{ tsrm_mutex_free(x ## _mutex); x ## _mutex = NULL; }
#define MutexLock(x)		{ tsrm_mutex_lock(x ## _mutex); }
#define MutexUnlock(x)		{ tsrm_mutex_unlock(x ## _mutex); }
#else
#define MutexDeclare(x)
#define StaticMutexDeclare(x)
#define MutexSetup(x)
#define MutexShutdown(x)
#define MutexLock(x)
#define MutexUnlock(x)
#endif

/*============================================================================*/
#endif	/* __PECL_UTILS_MUTEX_H */
