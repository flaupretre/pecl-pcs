/*
  +----------------------------------------------------------------------+
  | EX1 extension <http://ex1.tekwire.net>                               |
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

/* Example extension */

/*============================================================================*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"

#include "php_ex1.h"

#if PHP_MAJOR_VERSION >= 7
#define PHP_7
#endif

/*------------------------*/

#ifdef COMPILE_DL_EX1
#	ifdef PHP_7
#		ifdef ZTS
			ZEND_TSRMLS_CACHE_DEFINE();
#		endif
#	endif
	ZEND_GET_MODULE(ex1)
#endif

/*------------------------*/

ZEND_BEGIN_MODULE_GLOBALS(ex1)

ZEND_END_MODULE_GLOBALS(ex1)

ZEND_DECLARE_MODULE_GLOBALS(ex1)

#ifdef ZTS
#	ifdef PHP_7
#		define EX1_G(v) ZEND_TSRMG(ex1_globals_id, zend_ex1_globals *, v)
#	else
#		define EX1_G(v) TSRMG(ex1_globals_id, zend_ex1_globals *, v)
#	endif
#else
#	define EX1_G(v) (ex1_globals.v)
#endif

int pcs_file_count;

#include "php/php.c/code.php.c"

/*---------------------------------------------------------------*/
/* phpinfo() output                                              */

static PHP_MINFO_FUNCTION(ex1)
{
	char buf[10];

	php_info_print_table_start();

	php_info_print_table_row(2, "EX1", "enabled");
	php_info_print_table_row(2, "Version", PHP_EX1_VERSION);

	sprintf(buf,"%d", pcs_file_count);
	php_info_print_table_row(2, "Scripts registered in PCS", buf);

	php_info_print_table_end();
}

/*---------------------------------------------------------------*/
/* Initialize a new zend_ex1_globals struct during thread spin-up */

static void ex1_globals_ctor(zend_ex1_globals * globals TSRMLS_DC)
{
#if defined(PHP_7) && defined(COMPILE_DL_EX1) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	memset(globals, 0, sizeof(*globals)); /* Init everything to 0/NULL */
}

/*------------------------*/
/* Any resources allocated during initialization may be freed here */

#ifndef ZTS
static void ex1_globals_dtor(zend_ex1_globals * globals TSRMLS_DC)
{
}
#endif

/*---------------------------------------------------------------*/

static PHP_RINIT_FUNCTION(ex1)
{
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static PHP_RSHUTDOWN_FUNCTION(ex1)
{
	return SUCCESS;
}

/*---------------------------------------------------------------*/
/* We use a separate init function because client extensions can
*  be initialized before EX1. So, EX1 may need to be initialized
*  before its own MINIT().
*/

static PHP_MINIT_FUNCTION(ex1)
{
	ZEND_INIT_MODULE_GLOBALS(ex1, ex1_globals_ctor, NULL);

	/* Register PHP code in PCS */

	pcs_file_count = PCS_registerDescriptors(code, 0);
	if (pcs_file_count < 0) return FAILURE;
	
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static PHP_MSHUTDOWN_FUNCTION(ex1)
{
#ifndef ZTS
		ex1_globals_dtor(&ex1_globals TSRMLS_CC);
#endif

	return SUCCESS;
}

/*---------------------------------------------------------------*/
/*-- Functions --*/

static zend_function_entry ex1_functions[] = {
    {NULL, NULL, NULL}  /* must be the last line */
};

/*---------------------------------------------------------------*/
/*-- Module definition --*/

/* This module depends on the PCS extension, which must be activated first */

static const zend_module_dep ex1_deps[] = {
	ZEND_MOD_REQUIRED("pcs")
	ZEND_MOD_END
};
	
zend_module_entry ex1_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	ex1_deps,
	PHP_EX1_EXTNAME,
	ex1_functions,
	PHP_MINIT(ex1),
	PHP_MSHUTDOWN(ex1),
	PHP_RINIT(ex1),
	PHP_RSHUTDOWN(ex1),
	PHP_MINFO(ex1),
	PHP_EX1_VERSION,
	STANDARD_MODULE_PROPERTIES
};

/*---------------------------------------------------------------*/
