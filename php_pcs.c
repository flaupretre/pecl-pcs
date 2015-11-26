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
/* Uncomment to prefix debug messages with timestamps */
/*#define UT_DBG_TIMESTAMPS*/

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
#include "zend_extensions.h"
#include "ext/standard/info.h"

#include "pecl-compat/compat.h"
#include "pecl-utils/utils.h"

#ifdef PHP_7
#include "Zend/zend_portability.h"
#endif

#if ZEND_EXTENSION_API_NO >= PHP_5_6_X_API_NO
#include "zend_virtual_cwd.h"
#else
#include "TSRM/tsrm_virtual_cwd.h"
#endif

/*---------------------------------------------------------------*/

#define MODULE_NAME PHP_PCS_EXTNAME
#define MODULE_VERSION PHP_PCS_VERSION

/*---------------------------------------------------------------*/

#include "php_pcs.h"

#include "./client.h"

#include "src/PCS_Utils.h"
#include "src/PCS_Tree.h"
#include "src/PCS_Class.h"
#include "src/PCS_Stream.h"
#include "src/PCS_Loader.h"
#include "src/PCS_API.h"

/*------------------------*/

#include "php/phpc/tools_code.phpc"

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

int dummy; /* Struct cannot be empty (error on WIndows) */

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
/* Including C code allows to export a minimal set of symbols */

#include "pecl-utils/utils.c"

#include "src/PCS_Utils.c"
#include "src/PCS_Tree.c"
#include "src/PCS_Class.c"
#include "src/PCS_Stream.c"
#include "src/PCS_Loader.c"
#include "src/PCS_API.c"

/*---------------------------------------------------------------*/
/* phpinfo() output                                              */

static PHP_MINFO_FUNCTION(pcs)
{
	char buf[10];

	php_info_print_table_start();

	php_info_print_table_row(2, "PHP Code Service", "enabled");
	php_info_print_table_row(2, "Version", PHP_PCS_VERSION);

	sprintf(buf, "%d", (int)zend_hash_num_elements(fileList));
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
	DBG_MSG("-> PCS RINIT");
	
	in_startup = 0;

	if (RINIT_PCS_Utils(TSRMLS_C) == FAILURE) return FAILURE;
	if (RINIT_PCS_Tree(TSRMLS_C) == FAILURE) return FAILURE;
	if (RINIT_PCS_Class(TSRMLS_C) == FAILURE) return FAILURE;
	if (RINIT_PCS_Stream(TSRMLS_C) == FAILURE) return FAILURE;
	if (RINIT_PCS_Loader(TSRMLS_C) == FAILURE) return FAILURE;
	if (RINIT_PCS_API(TSRMLS_C) == FAILURE) return FAILURE;

	DBG_MSG("<- PCS RINIT");
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
	if (RSHUTDOWN_PCS_Utils(TSRMLS_C) == FAILURE) return FAILURE;

	return SUCCESS;
}

/*---------------------------------------------------------------*/

static PHP_MINIT_FUNCTION(pcs)
{
#ifndef PHP_7
	/* In some PHP versions, valgrind reports a conditional jump on
	uninitialized data when calling php_error() during MINIT because
	EG(exception is not initialized yet. */

	EG(exception) = NULL;
#endif

	DBG_INIT();
	DBG_MSG("-> PCS MINIT");

	REGISTER_INI_ENTRIES();

	ZEND_INIT_MODULE_GLOBALS(pcs, pcs_globals_ctor, NULL);

	
	if (MINIT_PCS_Utils(TSRMLS_C) == FAILURE) return FAILURE;
	if (MINIT_PCS_Tree(TSRMLS_C) == FAILURE) return FAILURE;
	if (MINIT_PCS_Class(TSRMLS_C) == FAILURE) return FAILURE;
	if (MINIT_PCS_Stream(TSRMLS_C) == FAILURE) return FAILURE;
	if (MINIT_PCS_Loader(TSRMLS_C) == FAILURE) return FAILURE;
	if (MINIT_PCS_API(TSRMLS_C) == FAILURE) return FAILURE;

	/* Register the PHP tools */

	if (PCS_registerEmbedded(tools_code, IMM_STRL("internal/tools")
		, PCS_LOAD_NONE) == FAILURE) {
		return FAILURE;
	}

	DBG_MSG("<- PCS MINIT");
	return SUCCESS;
}

/*---------------------------------------------------------------*/

/* ARGSUSED*/
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
	if (MSHUTDOWN_PCS_Utils(TSRMLS_C) == FAILURE) return FAILURE;

	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}

/*---------------------------------------------------------------*/
/*-- Functions --*/

static zend_function_entry module_functions[] = {
    {NULL, NULL, NULL}  /* must be the last line */
};

/*---------------------------------------------------------------*/
/*-- Module definition --*/

zend_module_entry pcs_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	MODULE_NAME,
	module_functions,
	PHP_MINIT(pcs),
	PHP_MSHUTDOWN(pcs),
	PHP_RINIT(pcs),
	PHP_RSHUTDOWN(pcs),
	PHP_MINFO(pcs),
#if ZEND_MODULE_API_NO >= 20010901
	MODULE_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};

/*---------------------------------------------------------------*/
