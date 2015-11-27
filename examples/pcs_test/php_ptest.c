/*
  +----------------------------------------------------------------------+
  | PCS test extension <http://ptest.tekwire.net>                        |
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
/* This extension is used to test the PCS extension */

/*============================================================================*/

#include "../../pecl-compat/compat.h"

#include "ext/standard/info.h"

#include "ext/pcs/client.h"

#include "php_ptest.h"

/*---------------------------------------------------------------*/

#define MODULE_NAME PHP_PTEST_EXTNAME
#define MODULE_VERSION PHP_PTEST_VERSION

/*---------------------------------------------------------------*/

#ifdef COMPILE_DL_PTEST
	ZEND_TSRMLS_CACHE_DEFINE();
	ZEND_GET_MODULE(ptest)
#endif

#define IMM_STRL(_str)	_str, sizeof(_str) - 1

/*------------------------*/

ZEND_BEGIN_MODULE_GLOBALS(ptest)

zend_long test_case;
zend_bool load_code1;
zend_bool load_code2;
zend_bool load_code3;
zend_bool load_empty;
zend_bool load_messages;

ZEND_END_MODULE_GLOBALS(ptest)

ZEND_DECLARE_MODULE_GLOBALS(ptest)

#define PTEST_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(ptest, v)

long pcs_file_count;

char data1[] = "<?php\n\
namespace PCS_Test {\n\
class data1\n\
{\n\
public function hello()\n\
{\n\
	echo \"Hello from \".__CLASS__.\"\\n\";\n\
}\n\
} // End of class\n\
} // End of namespace\n\
";

/*============================================================================*/

#include "php/phpc/code1.phpc"

#include "php/phpc/code2.phpc"

#include "php/phpc/code3.phpc"

#include "php/phpc/empty.phpc"

/*============================================================================*/
/*---------------------------------------------------------------*/
/* Dummy function called from PHP */

PHP_FUNCTION(ptest_add)
{
	COMPAT_ARG_LONG_T a, b;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &a, &b) == FAILURE) {
		return;
	}
	
	RETURN_LONG(a + b);
}

/*---------------------------------------------------------------*/
/* Dummy function calling PHP from C */

PHP_FUNCTION(ptest_c_to_php_test)
{
	zval func, *msg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &msg) == FAILURE) {
		return;
	}

#ifdef PHP_7	
	ZVAL_STRINGL(&func, "PCS_Test\\Dummy5::hello", sizeof("PCS_Test\\Dummy5::hello") - 1);
	call_user_function(NULL, NULL, &func, return_value, 1, msg);
	zval_ptr_dtor(&func);
#else
	ZVAL_STRINGL(&func, "PCS_Test\\Dummy5::hello", sizeof("PCS_Test\\Dummy5::hello") - 1, 1);
	call_user_function(NULL, NULL, &func, return_value, 1, &msg TSRMLS_CC);
	zval_dtor(&func);
#endif
}

/*============================================================================*/
/*---------------------------------------------------------------*/
/* phpinfo() output                                              */

static PHP_MINFO_FUNCTION(ptest)
{
	char buf[10];

	php_info_print_table_start();

	php_info_print_table_row(2, "PCS test extension", "enabled");
	php_info_print_table_row(2, "Version", PHP_PTEST_VERSION);

	/* Let's display the number of PHP scripts we registered in PCS */

	sprintf(buf,"%ld", pcs_file_count);
	php_info_print_table_row(2, "Scripts registered in PCS", buf);

	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

/*---------------------------------------------------------------*/
/* Initialize a new zend_ptest_globals struct during thread spin-up */

static void ptest_globals_ctor(zend_ptest_globals * globals TSRMLS_DC)
{
#ifdef COMPILE_DL_PTEST
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	memset(globals, 0, sizeof(*globals)); /* Init everything to 0/NULL */
}

/*------------------------*/
/* Any resources allocated during initialization may be freed here */

static void ptest_globals_dtor(zend_ptest_globals * globals TSRMLS_DC)
{
}

/*---------------------------------------------------------------*/
/* Ini settings */

PHP_INI_BEGIN()

STD_PHP_INI_ENTRY("ptest.test_case", "0", PHP_INI_ALL, OnUpdateLong, test_case,
	zend_ptest_globals, ptest_globals)
STD_PHP_INI_BOOLEAN("ptest.load_code1", "0", PHP_INI_ALL, OnUpdateBool, load_code1,
	zend_ptest_globals, ptest_globals)
STD_PHP_INI_BOOLEAN("ptest.load_code2", "0", PHP_INI_ALL, OnUpdateBool, load_code2,
	zend_ptest_globals, ptest_globals)
STD_PHP_INI_BOOLEAN("ptest.load_code3", "0", PHP_INI_ALL, OnUpdateBool, load_code3,
	zend_ptest_globals, ptest_globals)
STD_PHP_INI_BOOLEAN("ptest.load_empty", "0", PHP_INI_ALL, OnUpdateBool, load_empty,
	zend_ptest_globals, ptest_globals)
STD_PHP_INI_BOOLEAN("ptest.load_messages", "1", PHP_INI_ALL, OnUpdateBool, load_messages,
	zend_ptest_globals, ptest_globals)

PHP_INI_END()

/*---------------------------------------------------------------*/

static PCS_ID register_data1(char *path, zend_ulong flags TSRMLS_DC)
{
	PCS_ID id;
	char *rpath;

	id = PCS_registerData(data1, strlen(data1), path, strlen(path), flags);
	if (id == FAILURE) {
		php_error(E_CORE_ERROR, "Error registering data1 as %s: PCS_registerData() failed", path);
		return FAILURE;
	}
	if (id == FAILURE) return FAILURE;
	rpath = PCS_getPath(id);
	if (! rpath) {
		php_error(E_CORE_ERROR, "Error registering data1 as %s: Cannot retrieve path", path);
		return FAILURE;
	}
	if (PTEST_G(load_messages)) php_printf("Registered data1 as %s => %s\n", path, rpath);
	
	return id;
}

/*---------------------------------------------------------------*/

static PHP_RINIT_FUNCTION(ptest)
{
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static PHP_RSHUTDOWN_FUNCTION(ptest)
{
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static PHP_MINIT_FUNCTION(ptest)
{
	long count;

	ZEND_INIT_MODULE_GLOBALS(ptest, ptest_globals_ctor, ptest_globals_dtor);
	REGISTER_INI_ENTRIES();

	pcs_file_count = 0;

	if (PTEST_G(load_code1)) {
		if ((count = PCS_registerEmbedded(code1, "ext/ptest", sizeof("ext/ptest") -1, 0)) == FAILURE) return FAILURE;
		if (PTEST_G(load_messages)) php_printf("Loaded code1 set\n");
		pcs_file_count += count;
	}

	if (PTEST_G(load_code2)) {
		if ((count = PCS_registerEmbedded(code2, "ext/ptest", sizeof("ext/ptest") -1, 0)) == FAILURE) return FAILURE;
		if (PTEST_G(load_messages)) php_printf("Loaded code2 set\n");
		pcs_file_count += count;
	}

	if (PTEST_G(load_code3)) {
		if ((count = PCS_registerEmbedded(code3, "ext/ptest", sizeof("ext/ptest") -1, 0)) == FAILURE) return FAILURE;
		if (PTEST_G(load_messages)) php_printf("Loaded code3 set\n");
		pcs_file_count += count;
	}

	if (PTEST_G(load_empty)) {
		if ((count = PCS_registerEmbedded(empty, "ext/ptest", sizeof("ext/ptest") -1, 0)) == FAILURE) return FAILURE;
		if (PTEST_G(load_messages)) php_printf("Loaded empty set\n");
		pcs_file_count += count;
	}

		switch(PTEST_G(test_case)) {
			case 0:
				/* Default */
				break;
			/* Register data */
			case 1:
				register_data1("ext/ptest/data1.php", 0 TSRMLS_CC);
				break;
			case 2:
				register_data1("ext/ptest/data1.php", PCS_LOAD_RINIT TSRMLS_CC);
				break;
			case 3:
				register_data1("ext/ptest/data1.php", PCS_LOAD_NONE TSRMLS_CC);
				break;
			case 4:
				register_data1("ext/ptest/data1.txt", PCS_LOAD_AUTOLOAD TSRMLS_CC);
				break;
			case 5:
				register_data1("ext/ptest/../data1.php", 0 TSRMLS_CC);
				break;
			case 6:
				register_data1("ext/ptest/./data1.php", 0 TSRMLS_CC);
				break;
			case 7:
				register_data1("/ext/ptest/data1.php", 0 TSRMLS_CC);
				break;
			case 8:
				register_data1("ext/ptest/data1.php/", 0 TSRMLS_CC);
				break;
			case 9:
				register_data1("/ext/ptest///data1.php/", 0 TSRMLS_CC);
				break;
			/* Register external path*/
			case 20:
				count = PCS_registerPath(IMM_STRL("./php/src/code1")
					, IMM_STRL("ext/ptest/external/code1"), 0);
				if (count == FAILURE) return FAILURE;
				if (PTEST_G(load_messages)) php_printf("Loaded code1 set from external file tree (%ld files)\n", count);
				pcs_file_count += count;
				break;
			case 21: /* Register a single file */
				count = PCS_registerPath(IMM_STRL("./php/src/code1/dir/Dummy4.php")
					, IMM_STRL("ext/ptest/d.php"), 0);
				if (count == FAILURE) return FAILURE;
				if (PTEST_G(load_messages)) php_printf("Loaded %ld file\n", count);
				pcs_file_count += count;
				break;
			case 22: /* Error on non-existing source */
				count = PCS_registerPath(IMM_STRL("./unexisting/src/code1")
					, IMM_STRL("ext/ptest/external/code1"), 0);
				if (count == FAILURE) return FAILURE;
				if (PTEST_G(load_messages)) php_printf("Loaded unknown set from external file tree (%ld files)\n", count);
				pcs_file_count += count;
				break;
			case 23: /* Register a single file (disabling autoload) */
				count = PCS_registerPath(IMM_STRL("./php/src/code1/dir/Dummy4.php")
					, IMM_STRL("ext/ptest/d.php"), PCS_LOAD_NONE);
				if (count == FAILURE) return FAILURE;
				if (PTEST_G(load_messages)) php_printf("Loaded %ld file\n", count);
				pcs_file_count += count;
				break;
			case 24: /* Register a single file (forcing autoload) */
				count = PCS_registerPath(IMM_STRL("./php/src/code1/dir/Dummy4.php")
					, IMM_STRL("ext/ptest/d"), PCS_LOAD_AUTOLOAD);
				if (count == FAILURE) return FAILURE;
				if (PTEST_G(load_messages)) php_printf("Loaded %ld file\n", count);
				pcs_file_count += count;
				break;
			/* Register descriptor set forcing autoload */
			case 30:
				if ((count = PCS_registerEmbedded(code3, "ext/ptest", sizeof("ext/ptest") -1, PCS_LOAD_AUTOLOAD)) == FAILURE) return FAILURE;
				if (PTEST_G(load_messages)) php_printf("Loaded code3 set (autoload force)\n");
				pcs_file_count += count;
				break;

			default:
				php_error(E_CORE_ERROR, "Invalid test case (%ld)", PTEST_G(test_case));
		}

	return SUCCESS;
}

/*---------------------------------------------------------------*/

static PHP_MSHUTDOWN_FUNCTION(ptest)
{
	UNREGISTER_INI_ENTRIES();

#ifndef ZTS
		ptest_globals_dtor(&ptest_globals TSRMLS_CC);
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

static zend_function_entry ptest_functions[] = {
	PHP_FE(ptest_c_to_php_test, UT_1arg_arginfo)
	PHP_FE(ptest_add, UT_2args_arginfo)
    PHP_FE_END  /* must be the last line */
};

/*---------------------------------------------------------------*/
/*-- Module definition --*/

static const zend_module_dep ptest_deps[] = {
	ZEND_MOD_REQUIRED("pcs")
	ZEND_MOD_END
};
	
zend_module_entry ptest_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	ptest_deps,
/*============================================================================*/
	MODULE_NAME,
	ptest_functions,
	PHP_MINIT(ptest),
	PHP_MSHUTDOWN(ptest),
	PHP_RINIT(ptest),
	PHP_RSHUTDOWN(ptest),
	PHP_MINFO(ptest),
	MODULE_VERSION,
	STANDARD_MODULE_PROPERTIES
};

/*---------------------------------------------------------------*/
