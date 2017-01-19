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

#include "php/phpc/parser_code.phpc"

static PCS_Node *StringParser_node;
static PCS_Node *ParserInterface_node;

static zend_string *parser_func_name;

static void (*spl_register_handler)(INTERNAL_FUNCTION_PARAMETERS);
static void (*spl_unregister_handler)(INTERNAL_FUNCTION_PARAMETERS);
static void (*spl_functions_handler)(INTERNAL_FUNCTION_PARAMETERS);
static zend_function *pcs_autoload_func;
static zend_function *spl_autoload_call_func;

/*============================================================================*/

#define PCS_DECLARE_GET_REQUIRE_FUNCTIONS(_name) \
	static PHP_METHOD(PCS, get ## _name); \
	static PHP_METHOD(PCS, require ## _name); \

/*---------------------------------------------------------------*/

extern PHP_FUNCTION(spl_autoload_register);

static PHP_FUNCTION(_pcs_autoload_register);
static void PCS_Loader_insertAutoloadHook(TSRMLS_D);
static PHP_FUNCTION(_pcs_autoload);
static int PCS_Loader_loadSymbol(char type, char *symbol, size_t slen, zend_bool autoload
	, zend_bool exception TSRMLS_DC);
static int PCS_Loader_symbolIsDefined(char type, char *symbol, size_t slen TSRMLS_DC);
static int PCS_Loader_loadNode(PCS_Node *node, int throw TSRMLS_DC);
static char *PCS_Loader_keyTypeString(char c);

PCS_DECLARE_GET_REQUIRE_FUNCTIONS(Function)
PCS_DECLARE_GET_REQUIRE_FUNCTIONS(Constant)
PCS_DECLARE_GET_REQUIRE_FUNCTIONS(Class)

static int PCS_Loader_registerNode(PCS_Node *node TSRMLS_DC);
static int PCS_Loader_registerKey(zend_string *key, PCS_Node *node);
static zend_function *PCS_Loader_get_function(HashTable *h, char *fname, int err);
static int PCS_Loader_moduleInit(TSRMLS_D);
static int PCS_Loader_Init(TSRMLS_D);

/*============================================================================*/
#endif
