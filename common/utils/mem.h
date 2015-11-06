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

#ifndef __PECL_UTILS_MEM_H
#define __PECL_UTILS_MEM_H 1

#include "zend.h"
#include "zend_alloc.h"

/*============================================================================*/

#define ut_eallocate(ptr, size) ut_allocate(ptr, size, 0)
#define ut_pallocate(ptr, size) ut_allocate(ptr, size, 1)

#define PEALLOCATE(ptr, size, persistent)	ptr=ut_allocate(ptr, size, persistent)
#define EALLOCATE(ptr, size)	PEALLOCATE(ptr, size, 0)
#define PALLOCATE(ptr, size)	PEALLOCATE(ptr, size, 1)
#define PEFREE(ptr, persistent)	PEALLOCATE(ptr, 0, persistent)
#define EFREE(ptr)				PEFREE(ptr, 0)
#define PFREE(ptr)				PEFREE(ptr, 1)

#define ut_eduplicate(ptr, size) ut_duplicate(ptr, size, 0)
#define ut_pduplicate(ptr, size) ut_duplicate(ptr, size, 1)

/*============================================================================*/

static zend_always_inline void *ut_allocate(void *ptr, size_t size, int persistent)
{
	if (ptr) {
		if (size) ptr=perealloc(ptr, size, persistent);
		else {
			pefree(ptr, persistent);
			ptr=NULL;
		}
	} else {
		if (size) ptr=pemalloc(size, persistent);
	}
return ptr;
}
	
/*---------------------------------------------------------------*/

static zend_always_inline void *ut_duplicate(void *ptr, size_t size, int persistent)
{
	char *p;

	if (!ptr) return NULL;
	if (size==0) return ut_allocate(NULL,1,persistent);

	p=ut_allocate(NULL,size,persistent);
	memmove(p,ptr,size);
	return p;
}

/*============================================================================*/
#endif	/* __PECL_UTILS_MEM_H */
