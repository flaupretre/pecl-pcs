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

#ifndef __PCS_CLIENT_H
#define __PCS_CLIENT_H

#include <stdlib.h>

#include "zend_types.h"

/*============================================================================*/

/*-- Flags --*/

/* Whether to parse and autoload the file */
/* Default (0): Only compile filenames with php|PHP suffix */

#define PCS_AUTOLOAD_FORCE		0x01
#define PCS_AUTOLOAD_DISABLE	0x02

/*-----------------*/

typedef zend_long PCS_ID;

/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* Registers a descriptor list, created by the pcs_process_code.php script.
   Returns the number of registered scripts, or FAILURE on error.
   Can be called during MINIT only.
*/

PHPAPI int PCS_registerDescriptors(void *list, zend_long flags);

/*----------------------------------------------------------------------------*/
/* Registers a script contained in memory
   Data is not duplicated. So, it must be persistent and never overwritten
   Returns the ID of the registered file, or FAILURE on error.
   Can be called during MINIT only.
*/

PHPAPI PCS_ID PCS_registerData(char *data, size_t data_len
	, const char *path, size_t pathlen, zend_long flags);

/*----------------------------------------------------------------------------*/
/* Registers an external file/tree. filename is a path to an existing
   file or directory.
   If it is a directory, the subtree is recursively crawled and registered.
   The source path must be absolute (don't assume anything about the
   current working directory when this function is executed).
   Stream-wrapped paths are accepted.
   Returns the number of registered scripts, or FAILURE on error.
   Can be called during MINIT only.
*/

PHPAPI int PCS_registerPath(const char *filename, size_t filename_len
	, const char *virtual_path, size_t virtual_path_len, zend_long flags);

/*----------------------------------------------------------------------------*/
/*	Execute a registered PHP script.
	The input arg is a PCS_ID.
	Use only when script cannot be autoloaded.
	Cannot be called during MINIT.
	Throws exception on error.
*/

PHPAPI void PCS_loadScript(PCS_ID id);

/*----------------------------------------------------------------------------*/
/*	Returns the node ID of a registered script, knowing its virtual path
	Returns FAILURE on error.
	If the 'throw' arg is non-null, also throws an exception on error.
*/

PHPAPI PCS_ID PCS_getID(const char *path, size_t pathlen, int throw);

/*============================================================================*/
#endif /* __PCS_CLIENT_H */
