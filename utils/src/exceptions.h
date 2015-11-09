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

#ifndef __PECL_UTILS_EXCEPTIONS_H
#define __PECL_UTILS_EXCEPTIONS_H 1

#include "zend_exceptions.h"

/*============================================================================*/

#define THROW_EXCEPTION(_format)	\
	{ \
	DBG_MSG("Throwing exception: " _format); \
	(void)zend_throw_exception_ex(NULL,0 TSRMLS_CC ,_format); \
	}

#define THROW_EXCEPTION_1(_format,_arg1)	\
	{ \
	DBG_MSG1("Throwing exception: " _format , _arg1); \
	(void)zend_throw_exception_ex(NULL,0 TSRMLS_CC ,_format,_arg1); \
	}

#define THROW_EXCEPTION_2(_format,_arg1,_arg2)	\
	{ \
	DBG_MSG2("Throwing exception: " _format , _arg1, _arg2); \
	(void)zend_throw_exception_ex(NULL,0 TSRMLS_CC ,_format,_arg1,_arg2); \
	}

#define THROW_EXCEPTION_3(_format,_arg1,_arg2,_arg3)	\
	{ \
	DBG_MSG3("Throwing exception: " _format , _arg1, _arg2, _arg3); \
	(void)zend_throw_exception_ex(NULL,0 TSRMLS_CC ,_format,_arg1,_arg2,_arg3); \
	}

#define EXCEPTION_ABORT(_format)	\
	{ \
	THROW_EXCEPTION(_format); \
	return; \
	}

#define EXCEPTION_ABORT_1(_format,_arg1)	\
	{ \
	THROW_EXCEPTION_1(_format,_arg1); \
	return; \
	}

#define EXCEPTION_ABORT_2(_format,_arg1,_arg2)	\
	{ \
	THROW_EXCEPTION_2(_format,_arg1,_arg2); \
	return; \
	}

#define EXCEPTION_ABORT_3(_format,_arg1,_arg2,_arg3)	\
	{ \
	THROW_EXCEPTION_3(_format,_arg1,_arg2,_arg3); \
	return; \
	}

#define EXCEPTION_ABORT_RET(_ret,_format)	\
	{ \
	THROW_EXCEPTION(_format); \
	return _ret; \
	}

#define EXCEPTION_ABORT_RET_1(_ret,_format,_arg1)	\
	{ \
	THROW_EXCEPTION_1(_format,_arg1); \
	return _ret; \
	}

#define ON_EXCEPTION	if (EG(exception))
	
#define ON_EXCEPTION_RETURN_VOID()	{ ON_EXCEPTION return; }

#define ON_EXCEPTION_RETURN(_ret)	{ ON_EXCEPTION return (_ret); }

/*============================================================================*/
#endif	/* __PECL_UTILS_EXCEPTIONS_H */
