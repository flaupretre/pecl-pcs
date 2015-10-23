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

/* Uncomment to display debug messages */
/*#define PCS_DEBUG*/

/*============================================================================*/

#ifdef PCS_DEBUG
#define UT_DEBUG
#endif

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif

#ifdef HAVE_STDLIB_H
#	include <stdlib.h>
#endif

#include <stdio.h>
#include <fcntl.h>

#ifdef HAVE_SYS_TYPES_H
#	include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
#	include <sys/stat.h>
#endif

#if HAVE_STRING_H
#	include <string.h>
#endif

#if HAVE_UNISTD_H
#	include <unistd.h>
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "Zend/zend_portability.h"

#if ZEND_EXTENSION_API_NO >= PHP_5_6_X_API_NO
#include "zend_virtual_cwd.h"
#else
#include "TSRM/tsrm_virtual_cwd.h"
#endif

/*---------------------------------------------------------------*/

#include "php_pcs.h"
#include "common/compat.h"
#include "common/utils.h"

#ifdef PHP_7
#	include "PHP_7/PCS_Tree.h"
#	include "PHP_7/PCS_Class.h"
#	include "PHP_7/PCS_Stream.h"
#	include "PHP_7/PCS_Loader.h"
#	include "PHP_7/PCS_API.h"
#else
#endif

/*------------------------*/

#ifdef COMPILE_DL_PCS
#	ifdef PHP_7
#		ifdef ZTS
			ZEND_TSRMLS_CACHE_DEFINE();
#		endif
#	endif
	ZEND_GET_MODULE(pcs)
#endif

/*------------------------*/

ZEND_BEGIN_MODULE_GLOBALS(pcs)

int nb_files; /* Number of registered files (info) */

ZEND_END_MODULE_GLOBALS(pcs)

ZEND_DECLARE_MODULE_GLOBALS(pcs)

#ifdef ZTS
#	ifdef PHP_7
#		define PCS_G(v) ZEND_TSRMG(pcs_globals_id, zend_pcs_globals *, v)
#	else
#		define PCS_G(v) TSRMG(pcs_globals_id, zend_pcs_globals *, v)
#	endif
#else
#	define PCS_G(v) (pcs_globals.v)
#endif

/*------------------------*/
/* Ini parameters */

PHP_INI_BEGIN()

PHP_INI_END()

/*------------------------*/

static int init_done=0;

/*------------------------*/
/* Including C code allows to export a minimal set of symbols */

#include "common/utils.c"

#ifdef PHP_7
#	include "PHP_7/PCS_Tree.c"
#	include "PHP_7/PCS_Class.c"
#	include "PHP_7/PCS_Stream.c"
#	include "PHP_7/PCS_Loader.c"
#	include "PHP_7/PCS_API.c"
#else
#endif

/*---------------------------------------------------------------*/
/* phpinfo() output                                              */

static PHP_MINFO_FUNCTION(pcs)
{
	char buf[10];

	php_info_print_table_start();

	php_info_print_table_row(2, "PHP Code Service", "enabled");
	php_info_print_table_row(2, "Version", PHP_PCS_VERSION);

	sprintf(buf,"%d",PCS_G(nb_files));
	php_info_print_table_row(2, "Registered script count",buf);

	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

/*---------------------------------------------------------------*/
/* Initialize a new zend_pcs_globals struct during thread spin-up */

static void pcs_globals_ctor(zend_pcs_globals * globals TSRMLS_DC)
{
#if defined(PHP_7) && defined(COMPILE_DL_PCS) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	CLEAR_DATA(*globals); /* Init everything to 0/NULL */
}

/*------------------------*/
/* Any resources allocated during initialization may be freed here */

#ifndef ZTS
static void pcs_globals_dtor(zend_pcs_globals * globals TSRMLS_DC)
{
}
#endif

/*---------------------------------------------------------------*/

static PHP_RINIT_FUNCTION(pcs)
{
	DBG_INIT();

	if (RINIT_PCS_Tree(TSRMLS_C) == FAILURE) return FAILURE;
	if (RINIT_PCS_Class(TSRMLS_C) == FAILURE) return FAILURE;
	if (RINIT_PCS_Stream(TSRMLS_C) == FAILURE) return FAILURE;
	if (RINIT_PCS_Loader(TSRMLS_C) == FAILURE) return FAILURE;
	if (RINIT_PCS_API(TSRMLS_C) == FAILURE) return FAILURE;

	return SUCCESS;
}

/*---------------------------------------------------------------*/

static PHP_RSHUTDOWN_FUNCTION(pcs)
{
	if (RSHUTDOWN_PCS_API(TSRMLS_C) == FAILURE) return FAILURE;
	if (RSHUTDOWN_PCS_Loader(TSRMLS_C) == FAILURE) return FAILURE;
	if (RSHUTDOWN_PCS_Stream(TSRMLS_C) == FAILURE) return FAILURE;
	if (RSHUTDOWN_PCS_Class(TSRMLS_C) == FAILURE) return FAILURE;
	if (RSHUTDOWN_PCS_Tree(TSRMLS_C) == FAILURE) return FAILURE;

	return SUCCESS;
}

/*---------------------------------------------------------------*/

static int PCS_init()
{
	if (init_done) return SUCCESS;
	init_done=1;

	ZEND_INIT_MODULE_GLOBALS(pcs, pcs_globals_ctor, NULL);

	if (MINIT_PCS_Tree(TSRMLS_C) == FAILURE) return FAILURE;
	if (MINIT_PCS_Class(TSRMLS_C) == FAILURE) return FAILURE;
	if (MINIT_PCS_Stream(TSRMLS_C) == FAILURE) return FAILURE;
	if (MINIT_PCS_Loader(TSRMLS_C) == FAILURE) return FAILURE;
	if (MINIT_PCS_API(TSRMLS_C) == FAILURE) return FAILURE;

	return SUCCESS;
}
/*---------------------------------------------------------------*/
/* We use a separate init function because client extensions can
*  be initialized before PCS. So, PCS may need to be initialized
*  before its own MINIT().
*/

static PHP_MINIT_FUNCTION(pcs)
{
	REGISTER_INI_ENTRIES();

	if (PCS_init() == FAILURE) {
		return FAILURE;
	}

	return SUCCESS;
}

/*---------------------------------------------------------------*/

static PHP_MSHUTDOWN_FUNCTION(pcs)
{
#ifndef ZTS
		pcs_globals_dtor(&pcs_globals TSRMLS_CC);
#endif

	if (MSHUTDOWN_PCS_API(TSRMLS_C) == FAILURE) return FAILURE;
	if (MSHUTDOWN_PCS_Loader(TSRMLS_C) == FAILURE) return FAILURE;
	if (MSHUTDOWN_PCS_Stream(TSRMLS_C) == FAILURE) return FAILURE;
	if (MSHUTDOWN_PCS_Class(TSRMLS_C) == FAILURE) return FAILURE;
	if (MSHUTDOWN_PCS_Tree(TSRMLS_C) == FAILURE) return FAILURE;

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}

/*---------------------------------------------------------------*/
/*-- Functions --*/

static zend_function_entry pcs_functions[] = {
    {NULL, NULL, NULL}  /* must be the last line */
};

/*---------------------------------------------------------------*/
/*-- Module definition --*/

zend_module_entry pcs_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	PHP_PCS_EXTNAME,
	pcs_functions,
	PHP_MINIT(pcs),
	PHP_MSHUTDOWN(pcs),
	PHP_RINIT(pcs),
	PHP_RSHUTDOWN(pcs),
	PHP_MINFO(pcs),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_PCS_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};

/*---------------------------------------------------------------*/
