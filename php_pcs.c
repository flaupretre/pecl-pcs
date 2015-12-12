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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "zend_extensions.h"
#include "ext/standard/info.h"

#include "pecl-compat/compat.h"
#include "pecl-utils/utils.h"

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
#include "src/PCS_Info.h"

/*------------------------*/
/* Include embedded PHP code */

#include "php/phpc/tools_code.phpc"

/*------------------------*/

#ifdef COMPILE_DL_PCS
	ZEND_TSRMLS_CACHE_DEFINE();
	ZEND_GET_MODULE(pcs)
#endif

/*------------------------*/

ZEND_BEGIN_MODULE_GLOBALS(pcs)

zend_function *autoload_func; /* Original content of EG(autoload_func) */

ZEND_END_MODULE_GLOBALS(pcs)

ZEND_DECLARE_MODULE_GLOBALS(pcs)

#define PCS_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(pcs, v)

/*------------------------*/
/* Including C code allows to export only the symbols we want to make public */

#include "pecl-utils/utils.c"

#include "src/PCS_Utils.c"
#include "src/PCS_Tree.c"
#include "src/PCS_Class.c"
#include "src/PCS_Stream.c"
#include "src/PCS_Loader.c"
#include "src/PCS_API.c"
#include "src/PCS_Info.c"

/*---------------------------------------------------------------*/
/* phpinfo() output                                              */

static PHP_MINFO_FUNCTION(pcs)
{
	char buf[10];
	zend_ulong modes[3];
	PCS_Node *node;

	php_info_print_table_start();

	php_info_print_table_row(2, "PHP Code Service", "enabled");
	php_info_print_table_row(2, "Version", PHP_PCS_VERSION);

	sprintf(buf, "%d", (int)zend_hash_num_elements(fileList));
	php_info_print_table_row(2, "File count",buf);
	php_info_print_table_end();

	modes[0] = modes[1] = modes[2] = 0;
	ZEND_HASH_FOREACH_PTR(fileList, node) {
		modes [node->load_mode -1 ]++;
	} ZEND_HASH_FOREACH_END();

	php_info_print_table_start();

	php_info_print_table_colspan_header(2, "Load mode");

	sprintf(buf, "%lu", modes[PCS_LOAD_AUTOLOAD -1]);
	php_info_print_table_row(2, "Autoloaded",buf);

	sprintf(buf, "%lu", modes[PCS_LOAD_RINIT -1]);
	php_info_print_table_row(2, "Loaded at RINIT",buf);

	sprintf(buf, "%lu", modes[PCS_LOAD_NONE -1]);
	php_info_print_table_row(2, "Not loaded",buf);

	php_info_print_table_end();
}

/*---------------------------------------------------------------*/
/* Initialize a new zend_pcs_globals struct during thread spin-up */

static void pcs_globals_ctor(zend_pcs_globals * globals TSRMLS_DC)
{
#ifdef COMPILE_DL_PCS
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	CLEAR_DATA(*globals); /* Init everything to 0/NULL */
}

/*------------------------*/
/* Resources allocated during initialization are freed here */

static void pcs_globals_dtor(zend_pcs_globals * globals TSRMLS_DC)
{
}

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
	if (RINIT_PCS_Info(TSRMLS_C) == FAILURE) return FAILURE;

	DBG_MSG("<- PCS RINIT");
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static PHP_RSHUTDOWN_FUNCTION(pcs)
{
	if (RSHUTDOWN_PCS_Info(TSRMLS_C) == FAILURE) return FAILURE;
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

	ZEND_INIT_MODULE_GLOBALS(pcs, pcs_globals_ctor, NULL);

	
	if (MINIT_PCS_Utils(TSRMLS_C) == FAILURE) return FAILURE;
	if (MINIT_PCS_Tree(TSRMLS_C) == FAILURE) return FAILURE;
	if (MINIT_PCS_Class(TSRMLS_C) == FAILURE) return FAILURE;
	if (MINIT_PCS_Stream(TSRMLS_C) == FAILURE) return FAILURE;
	if (MINIT_PCS_Loader(TSRMLS_C) == FAILURE) return FAILURE;
	if (MINIT_PCS_API(TSRMLS_C) == FAILURE) return FAILURE;
	if (MINIT_PCS_Info(TSRMLS_C) == FAILURE) return FAILURE;

	/* Register embedded PHP code (tools) */

	if (PCS_registerEmbedded(tools_code, IMM_STRL("internal/tools"), 0) == FAILURE) {
		return FAILURE;
	}

	DBG_MSG("<- PCS MINIT");
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static PHP_MSHUTDOWN_FUNCTION(pcs)
{
	pcs_globals_dtor(ZEND_MODULE_GLOBALS_BULK(pcs) TSRMLS_CC);

	if (MSHUTDOWN_PCS_Info(TSRMLS_C) == FAILURE) return FAILURE;
	if (MSHUTDOWN_PCS_API(TSRMLS_C) == FAILURE) return FAILURE;
	if (MSHUTDOWN_PCS_Loader(TSRMLS_C) == FAILURE) return FAILURE;
	if (MSHUTDOWN_PCS_Stream(TSRMLS_C) == FAILURE) return FAILURE;
	if (MSHUTDOWN_PCS_Class(TSRMLS_C) == FAILURE) return FAILURE;
	if (MSHUTDOWN_PCS_Tree(TSRMLS_C) == FAILURE) return FAILURE;
	if (MSHUTDOWN_PCS_Utils(TSRMLS_C) == FAILURE) return FAILURE;

	return SUCCESS;
}

/*---------------------------------------------------------------*/
/*-- Module definition --*/

ZEND_BEGIN_ARG_INFO_EX(_pcs_autoload_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, symbol)
	ZEND_ARG_INFO(0, type)
	ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(_pcs_autoload_register_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, autoload_function)
	ZEND_ARG_INFO(0, throw)
	ZEND_ARG_INFO(0, prepend)
ZEND_END_ARG_INFO()

static zend_function_entry pcs_functions[] = {
	PHP_FE(_pcs_autoload, _pcs_autoload_arginfo)
	PHP_FE(_pcs_autoload_register, _pcs_autoload_register_arginfo)
	PHP_FE_END
};

static const zend_module_dep pcs_deps[] = {
	ZEND_MOD_REQUIRED("tokenizer")
	ZEND_MOD_REQUIRED("pcre")
	ZEND_MOD_REQUIRED("SPL")
	ZEND_MOD_END
};

zend_module_entry pcs_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	pcs_deps,
	MODULE_NAME,
	pcs_functions,
	PHP_MINIT(pcs),
	PHP_MSHUTDOWN(pcs),
	PHP_RINIT(pcs),
	PHP_RSHUTDOWN(pcs),
	PHP_MINFO(pcs),
	MODULE_VERSION,
	STANDARD_MODULE_PROPERTIES
};

/*---------------------------------------------------------------*/
