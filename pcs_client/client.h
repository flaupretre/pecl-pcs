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
/* Note: please keep this API compatible with PHP 5 and 7 */

#ifndef __PCS_CLIENT_H
#define __PCS_CLIENT_H

#include <stdlib.h>

#include "zend_types.h"

#if PHP_MAJOR_VERSION >= 7
	typedef size_t PCS_SIZE_T;
	typedef zend_long PCS_LONG_T;
#else
	typedef int PCS_SIZE_T;
	typedef long PCS_LONG_T;
#endif

typedef PCS_LONG_T PCS_ID;

/*============================================================================*/

/*-- Flags --*/

/* Whether to parse and autoload the file */
/* Default (0): Only compile filenames with php|PHP suffix */

#define PCS_AUTOLOAD_FORCE		0x01
#define PCS_AUTOLOAD_DISABLE	0x02

/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* Registers a descriptor list, created by the pcs_process_code.php script.
   Returns the number of registered scripts, or FAILURE on error.
   Can be called during MINIT only.
*/

PHPAPI int PCS_registerDescriptors(void *list, PCS_LONG_T flags);

/*----------------------------------------------------------------------------*/
/* Registers a file already present in memory
   Data is not duplicated. So, it is the caller's responsibility to ensure the
   data is persistent, never overwritten, and freed at MSHUTDOWN if needed.
   Returns the ID of the registered file, or FAILURE on error.
   Can be called during MINIT only.
*/

PHPAPI PCS_ID PCS_registerData(char *data, PCS_SIZE_T data_len
	, const char *path, PCS_SIZE_T pathlen, PCS_LONG_T flags);

/*----------------------------------------------------------------------------*/
/* Registers an external file/tree. filename is a path to an existing
   file or directory.
   If it is a directory, the subtree is recursively crawled and registered.
   The source path must be absolute (don't assume anything about the
   current working directory when this function is executed).
   Stream-wrapped paths are not accepted.
   Returns the number of registered scripts, or FAILURE on error.
   Can be called during MINIT only.
*/

PHPAPI int PCS_registerPath(const char *filename, PCS_SIZE_T filename_len
	, const char *virtual_path, PCS_SIZE_T virtual_path_len, PCS_LONG_T flags);

/*----------------------------------------------------------------------------*/
/*	Execute a registered PHP script.
	The input arg is a PCS_ID.
	Use only when script cannot be autoloaded.
	Cannot be called during MINIT.
	Throws exception on error.
*/

PHPAPI void PCS_loadScript(PCS_ID id);

/*----------------------------------------------------------------------------*/
/*	Returns the path corresponding to a given ID as a null-terminated string.
	The input arg is a PCS_ID.
	Returns NULL on error.
	If the 'throw' arg is non-null, also throws an exception on error.
	Returned string must NOT be modified nor freed.
	Can be called at any time, even during MINIT.
*/

PHPAPI char *PCS_getPath(PCS_ID id, int throw);

/*----------------------------------------------------------------------------*/
/*	Returns the ID of a registered script, knowing its virtual path
	Returns FAILURE on error.
	If the 'throw' arg is non-null, also throws an exception on error.
	Can be called at any time, even during MINIT.
*/

PHPAPI PCS_ID PCS_getID(const char *path, PCS_SIZE_T pathlen, int throw);

/*============================================================================*/
#endif /* __PCS_CLIENT_H */
