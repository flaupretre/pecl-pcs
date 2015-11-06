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

/* An example extension using PCS to embed some PHP code */

/* In this file, sections enclosed in '!!!...' lines signal the locations where
   some PCS-related modifications are present. Outside of these blocks, nothing
   differs from an 'usual' PHP extension.
*/
/*============================================================================*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"

/*============================================================================*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* Include the pcs client header file. This file contains the definition of the
   PCS API available to client extensions.
   
   In a real PCS client extension, the 'pcs_client' directory would be
   duplicated from the PCS extension source tree to the client source tree (so,
   there would be no '../..' prefix in the path below).
   Here, we use a single pcs_client directory, without duplicating it in every
   example, to avoid redundancies and synchronization mistakes.
*/

#include "../../pcs_client/client.h"

/*============================================================================*/

#include "php_ex1.h"

#if PHP_MAJOR_VERSION >= 7
#	define PHP_7
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

/*============================================================================*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* Here, we include the PHP code converted to a C-compatible format by
   pcs_process_code.php. The path of the generated file is defined in
   php/Makefile.
*/

#include "php/php.c/code.php.c"

/*============================================================================*/

/*---------------------------------------------------------------*/
/* phpinfo() output                                              */

static PHP_MINFO_FUNCTION(ex1)
{
	char buf[10];

	php_info_print_table_start();

	php_info_print_table_row(2, "EX1", "enabled");
	php_info_print_table_row(2, "Version", PHP_EX1_VERSION);

	/* Let's display the number of PHP scripts we registered in PCS */

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

static PHP_MINIT_FUNCTION(ex1)
{
	ZEND_INIT_MODULE_GLOBALS(ex1, ex1_globals_ctor, NULL);

/*============================================================================*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* Here we register our PHP scripts in PCS.

   That's the only interaction we'll have with PCS as, once it is done, PCS
   will autoload these scripts everytime it is needed.
   
   Note that PCS_registerDescriptors() returns the number of scripts registered.
   You may store this value or discard it, but you must at least test whether
   the return value is equal to FAILURE. If it is the case, you *must* abort
   function execution and return FAILURE too. This will abort PHP execution.
*/

	pcs_file_count = PCS_registerDescriptors(code, 0);
	if (pcs_file_count < 0) return FAILURE;
/*============================================================================*/
	
	return SUCCESS;
}

/*---------------------------------------------------------------*/
/* Dummy function called from PHP */

PHP_FUNCTION(ex1_add)
{
	PCS_LONG_T a, b;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ll", &a, &b) == FAILURE) {
                return;
        }
	
	RETURN_LONG(a + b);
}

/*---------------------------------------------------------------*/
/* Dummy function calling PHP from C */

PHP_FUNCTION(ex1_c_to_php_test)
{
	zval func, *msg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &msg) == FAILURE) {
		return;
	}

#ifdef PHP_7	
	ZVAL_STRINGL(&func, "Example1\\Dummy5::hello", sizeof("Example1\\Dummy5::hello") - 1);
	call_user_function(NULL, NULL, &func, return_value, 1, msg);
	zval_ptr_dtor(&func);
#else
	ZVAL_STRINGL(&func, "Example1\\Dummy5::hello", sizeof("Example1\\Dummy5::hello") - 1, 1);
	call_user_function(NULL, NULL, &func, return_value, 1, &msg TSRMLS_CC);
	zval_dtor(&func);
#endif
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

ZEND_BEGIN_ARG_INFO_EX(UT_1arg_arginfo, 0, 0, 1)
ZEND_ARG_INFO(0, arg1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(UT_2args_arginfo, 0, 0, 2)
ZEND_ARG_INFO(0, arg1)
ZEND_ARG_INFO(0, arg2)
ZEND_END_ARG_INFO()

static zend_function_entry ex1_functions[] = {
	PHP_FE(ex1_c_to_php_test, UT_1arg_arginfo)
	PHP_FE(ex1_add, UT_2args_arginfo)
    PHP_FE_END  /* must be the last line */
};

/*---------------------------------------------------------------*/
/*-- Module definition --*/

/*============================================================================*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* Here, we define a dependency from our extension to PCS

   This dependency ensures that PCS will be started (MINIT) *before* our
   extension. This is important because the call to PCS_registerDescriptors()
   above will fail if the PCS extension was not started yet.

   As we cannot know in advance which extension will be compiled statically
   and dynamically, the only way to ensure they are started in the right order
   is to create a dependency. In a future version, if PCS is included in the
   core distribution and configured to be always compiled statically, this
   constraint will disappear.
*/

static const zend_module_dep ex1_deps[] = {
	ZEND_MOD_REQUIRED("pcs")
	ZEND_MOD_END
};
	
zend_module_entry ex1_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	ex1_deps,
/*============================================================================*/
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
