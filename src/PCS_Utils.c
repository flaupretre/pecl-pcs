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

int PCS_Utils_assertModuleIsStarted()
{
	if (! ut_moduleIsStarted(MODULE_NAME, sizeof(MODULE_NAME) - 1)) {
		php_error(E_CORE_ERROR,"Cannot call PCS before it is started. Please use a module dependency");
		return FAILURE;
	}
	
	return SUCCESS;
}

/*===============================================================*/

static zend_always_inline int MINIT_PCS_Utils(TSRMLS_D)
{
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static zend_always_inline int MSHUTDOWN_PCS_Utils(TSRMLS_D)
{
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static zend_always_inline int RINIT_PCS_Utils(TSRMLS_D)
{
	return SUCCESS;
}
/*---------------------------------------------------------------*/

static zend_always_inline int RSHUTDOWN_PCS_Utils(TSRMLS_D)
{
	return SUCCESS;
}

/*===============================================================*/
