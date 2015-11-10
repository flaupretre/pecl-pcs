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

#ifndef __PECL_UTILS_MISC_H
#define __PECL_UTILS_MISC_H 1

#include <fcntl.h>

#include "main/SAPI.h"
#include "zend_modules.h"

/*============================================================================*/

#define CLEAR_DATA(_v)	memset(&(_v),'\0',sizeof(_v)); \

#define ENSURE_LONG(zp) { if (Z_TYPE_P((zp))!=IS_LONG) convert_to_long((zp)); }
#define ENSURE_BOOL(zp) { if (Z_TYPE_P((zp))!=IS_BOOL) convert_to_boolean((zp)); }
#define ENSURE_STRING(zp) { if (Z_TYPE_P((zp))!=IS_STRING) convert_to_string((zp)); }

#ifndef S_ISDIR
#	define S_ISDIR(mode)	(((mode)&S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISREG
#	define S_ISREG(mode)	(((mode)&S_IFMT) == S_IFREG)
#endif

#define IMM_STRL(_str)	_str, sizeof(_str) - 1

/*---------------------------------------------------------------*/

static zend_always_inline int ut_is_web()
{
	return strcmp(sapi_module.name, "cli");
}

/*------------------*/

static zend_always_inline int ut_extension_loaded(char *name, int len TSRMLS_DC)
{
	int status;

	status = compat_zend_hash_str_exists(&module_registry, name, len);

	DBG_MSG2("Checking if extension %s is loaded: %s",name,(status ? "yes" : "no"));
	return status;
}

/*-----------------------------------------------------*/

static zend_always_inline zend_module_entry *ut_getModuleData(char *name, size_t len)
{
	zend_module_entry *module;

#ifdef PHP_7
		zend_string *zs;
		
		zs = zend_string_init(name, len, 0);
		module = zend_hash_find_ptr(&module_registry, zs);
		zend_string_release(zs);
#else
		int status;

		status = zend_hash_find(&module_registry, name, len + 1, (void **)(&module));
		if (status != SUCCESS) module = NULL;
#endif

	if (! module) {
		php_error(E_CORE_ERROR, "%s: Cannot retrieve module data", name);
	}

	return module;
}
	
/*-----------------------------------------------------*/

static zend_always_inline int ut_moduleIsStarted(char *name, size_t len)
{
	zend_module_entry *module;

	module = ut_getModuleData(name, len);
	if ((! module) || (! module->module_started)) {
		return 0;
	}

	return 1;
}

/*============================================================================*/
#endif	/* __PECL_UTILS_MISC_H */
