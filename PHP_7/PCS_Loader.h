/*
  +----------------------------------------------------------------------+
  | PCS extension <http://PCS.tekwire.net>                       |
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

#ifndef __PCS_LOADER_H
#define __PCS_LOADER_H

/*---------------------------------------------------------------*/

/* Symbol types */

#define PCS_T_CLASS		'L'
#define PCS_T_FUNCTION	'F'
#define PCS_T_CONSTANT	'C'

/*---------------------------------------------------------------*/

static int loader_init_done = 0;

static HashTable *symbols;	/* (type + case-sensitive name) => (PCS_Node *) */
StaticMutexDeclare(symbols)

#include "php/php.c/parser_code.php.c"

static PCS_Node *StringParser_node;
static PCS_Node *ParserInterface_node;

static zend_string *spl_ar_func_name;
static zend_string *hook_func_name;
static zend_string *parser_func_name;

/*============================================================================*/

#define PCS_DECLARE_GET_REQUIRE_FUNCTIONS(_name) \
	static PHP_METHOD(PCS, get ## _name); \
	static PHP_METHOD(PCS, require ## _name); \

/*---------------------------------------------------------------*/

static void PCS_Loader_registerHook(TSRMLS_D);
static PHP_METHOD(PCS, autoloadHook);
static int PCS_Loader_loadSymbol(char type, char *symbol, PCS_SIZE_T slen, zend_bool autoload
	, zend_bool exception TSRMLS_DC);
static int PCS_Loader_symbolIsDefined(char type, char *symbol, PCS_SIZE_T slen);
static void PCS_Loader_loadNode(PCS_Node *node);
static char *PCS_Loader_keyTypeString(char c);

PCS_DECLARE_GET_REQUIRE_FUNCTIONS(Function)
PCS_DECLARE_GET_REQUIRE_FUNCTIONS(Constant)
PCS_DECLARE_GET_REQUIRE_FUNCTIONS(Class)

static int PCS_Loader_registerNode(PCS_Node *node);
static int PCS_Loader_registerKey(zend_string *key, PCS_Node *node);
static int PCS_Loader_moduleInit();
static int PCS_Loader_Init();

static int MINIT_PCS_Loader(TSRMLS_D);
static int MSHUTDOWN_PCS_Loader(TSRMLS_D);
static int RINIT_PCS_Loader(TSRMLS_D);
static int RSHUTDOWN_PCS_Loader(TSRMLS_D);

/*============================================================================*/
#endif
