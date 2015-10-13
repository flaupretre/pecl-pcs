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

#include <errno.h>

#include "php_streams.h"
#include "ext/standard/php_filestat.h"
#include "main/spprintf.h"
#include "main/php.h"

/*===============================================================*/
/* Public API */

/*--------------------*/
/* Register a set of preprocessed PHP scripts */
/* Return the number of registered descriptors or FAILURE on error */

PHPAPI int PCS_registerDescriptors(PCS_DESCRIPTOR *list, zend_long flags)
{
	PCS_Node *node;
	int count;

	count = 0;
	while (list->data) {
		node = PCS_Tree_addFile(list->path, list->path_len, list->data
			, list->data_len, 0, flags);
		if (! node) return FAILURE;
		list++;
		count++;
	}

	return count;
}

/*--------------------*/
/* Register a script contained in memory.
   The memory is duplicated. So, it doesn't need to be stored in persistent
   storage by the caller.
   Returns a PCS_NODE_ID which can be used as an argument to PCS_loadScriptByID().
   Returns NULL on error
*/

PHPAPI PCS_NODE_ID PCS_registerData(char *data, size_t data_len
	, const char *path, size_t path_len, zend_long flags)
{
	return PCS_Tree_addFile(path, path_len, data, data_len, 0, flags);
}

/*--------------------*/
/* Register a file or a file tree from an external path */
/* The source path must be an absolute path (don't assume anything about the
   current working directory when this function is executed).
   Stream-wrapped paths are accepted.
   Returns the address of the node corresponding to the filename argument . If
   this is a regular file, this value can be sent later to PCS_loadScriptByID().
*/

#define CLEANUP_PCS_registerPath() { \
	if (type) zend_string_release(type); \
	if (children_count && namelist) { \
		for (i = 0; i < children_count ; i++) { \
			zend_string_release(namelist[i]); \
			namelist[i] = NULL; /* Security */ \
		} \
	} \
	EFREE(namelist); \
	}

#define ABORT_PCS_registerPath() { \
	EFREE(data); \
	CLEANUP_PCS_registerPath(); \
	return NULL; \
	}

PHPAPI PCS_NODE_ID PCS_registerPath(const char *filename, size_t filename_len
	, const char *virtual_path, size_t virtual_path_len, zend_long flags)
{
	php_stream *stream;
	zend_string *contents;
	char *data = NULL, *sub_fname, *sub_vpath;
	size_t datalen, sub_fname_len, sub_vpath_len;
	PCS_Node *node, *subnode;
	zval zv;
	zend_string **namelist = NULL, *type = NULL, *zsp;
	int i, children_count = 0;

	php_stat(filename, filename_len, FS_TYPE, &zv);
	if (Z_TYPE(zv) != IS_STRING) ABORT_PCS_registerPath();
	type = Z_STR(zv);

	/* If path is a directory */

	if (!strcmp(ZSTR_VAL(type), "dir")) {
		node = PCS_Tree_addDir(virtual_path, virtual_path_len, flags);
		
		/* Recurse on dir entries */
	
		children_count = php_stream_scandir(filename, &namelist, NULL, NULL);
		if (children_count < 0) {
			php_error(E_CORE_ERROR, "%s: %s (errno %d)", filename, strerror(errno), errno);
			ABORT_PCS_registerPath();
		}
		if (children_count) {
			for (i = 0; i < children_count ; i++) {
				zsp = namelist[i];
				if (((ZSTR_LEN(zsp) == 1) && (ZSTR_VAL(zsp)[0] == '.')) ||
					((ZSTR_LEN(zsp) == 2) && (ZSTR_VAL(zsp)[0] == '.')
						&& (ZSTR_VAL(zsp)[1] == '.'))) {
					/* Ignore '.' and '..' */
					continue;
				}
				spprintf(&sub_fname, 0, "%s%s%s", filename, PHP_DIR_SEPARATOR, ZSTR_VAL(namelist[i]));
				sub_fname_len = filename_len + 1 + ZSTR_LEN(namelist[i]);
				spprintf(&sub_vpath, 0, "%s/%s", virtual_path, ZSTR_VAL(namelist[i]));
				sub_vpath_len = virtual_path_len + 1 + ZSTR_LEN(namelist[i]);
				subnode = PCS_registerPath(sub_fname, sub_fname_len, sub_vpath
									, sub_vpath_len, flags);
				EFREE(sub_fname);
				EFREE(sub_vpath);
				if (!subnode) {
					ABORT_PCS_registerPath();
				}
			}
		}
	
	/* If path is a regular file */

	} else if (!strcmp(ZSTR_VAL(type), "file")) {
		stream = php_stream_open_wrapper(filename, "rb", REPORT_ERRORS, NULL);
		if (!stream) {
			php_error(E_CORE_ERROR, "%s: File not found", filename);
			ABORT_PCS_registerPath();
		}
		contents = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL, 1);
		if (!contents) {
			php_error(E_CORE_ERROR, "%s: Cannot read file", filename);
			php_stream_close(stream);
			ABORT_PCS_registerPath();
		}
		datalen = ZSTR_LEN(contents);
		data = ut_duplicate(ZSTR_VAL(contents), datalen+1, 1);
		zend_string_release(contents);
		node = PCS_Tree_addFile(virtual_path, virtual_path_len, data, datalen, 1, flags);
	
	/* Other file types are unsupported */

	} else {
		php_error(E_CORE_ERROR, "%s: PCS unsupported file type (%s)", filename, Z_STRVAL(zv));
		ABORT_PCS_registerPath();
	}

	CLEANUP_PCS_registerPath();
	return node;
}

/*--------------------*/
/* On errors, throws exception */

PHPAPI void PCS_loadScript(PCS_NODE_ID id)
{
	PCS_Loader_loadNode((PCS_Node *)id);
}

/*--------------------*/
/* Returns SUCCESS|FAILURE */

PHPAPI PCS_NODE_ID PCS_Loader_getNodeID(const char *path, size_t pathlen)
{
	return PCS_Tree_getNodeFromPath(path, pathlen);
}

/*===============================================================*/

static int MINIT_PCS_API(TSRMLS_D)
{
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static int MSHUTDOWN_PCS_API(TSRMLS_D)
{
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static int RINIT_PCS_API(TSRMLS_D)
{
	return SUCCESS;
}
/*---------------------------------------------------------------*/

static int RSHUTDOWN_PCS_API(TSRMLS_D)
{
	return SUCCESS;
}

/*===============================================================*/
