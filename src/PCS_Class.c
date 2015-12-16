/*
  +----------------------------------------------------------------------+
  | PCS extension <http://PCS.tekwire.net>                       |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005-2015 The PHP Group                                |
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

/*---------------------------------------------------------------*/
/* {{{ proto string \PCS\Mgr::__construct() */
/* This private method is never called. It exists only to prevent instanciation
* from user code using a 'new' directive */

static PHP_METHOD(PCS, __construct)
{}

/*---------------------------------------------------------------*/

static zend_function_entry PCS_methods[] = {
	PHP_ME(PCS, __construct, UT_noarg_arginfo, ZEND_ACC_PRIVATE)
	PHP_ME(PCS, getFunction, UT_1arg_arginfo,
		   ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME(PCS, getConstant, UT_1arg_arginfo,
		   ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME(PCS, getClass, UT_1arg_arginfo,
		   ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME(PCS, requireFunction, UT_1arg_arginfo,
		   ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME(PCS, requireConstant, UT_1arg_arginfo,
		   ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME(PCS, requireClass, UT_1arg_arginfo,
		   ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME(PCS, fileCount, UT_noarg_arginfo,
		   ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME(PCS, fileInfos, UT_noarg_arginfo,
		   ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME(PCS, symbolInfos, UT_noarg_arginfo,
		   ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL, 0, 0}
};

/*===============================================================*/

static zend_always_inline int MINIT_PCS_Class(TSRMLS_D)
{
	zend_class_entry ce;
	
	INIT_CLASS_ENTRY(ce, "PCS\\Mgr", PCS_methods);
	zend_register_internal_class(&ce TSRMLS_CC);

	return SUCCESS;
}

/*---------------------------------------------------------------*/

static zend_always_inline int MSHUTDOWN_PCS_Class(TSRMLS_D)
{
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static zend_always_inline int RINIT_PCS_Class(TSRMLS_D)
{
	return SUCCESS;
}
/*---------------------------------------------------------------*/

static zend_always_inline int RSHUTDOWN_PCS_Class(TSRMLS_D)
{
	return SUCCESS;
}

/*===============================================================*/
