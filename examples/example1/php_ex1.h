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

#ifndef __PHP_EX1_H
#define __PHP_EX1_H

#include "pcs_client/client.h"

/*============================================================================*/

#define PHP_EX1_VERSION "0.0.1" /* The extension version */

#define PHP_EX1_EXTNAME "ex1"

extern zend_module_entry ex1_module_entry;

#define phpext_ex1_ptr &ex1_module_entry

/*============================================================================*/

/*============================================================================*/
#endif /* __PHP_EX1_H */
