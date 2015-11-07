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

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>

#ifdef HAVE_SYS_DIR_H
# include <sys/dir.h>
#endif

#ifdef PHP_WIN32
# include "config.w32.h"
# include "win32/readdir.h"
#else
# include <php_config.h>
#endif

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_DIRENT_H
# include <dirent.h>
#endif

#include "ext/standard/file.h"
#include "ext/standard/php_filestat.h"
#include "main/spprintf.h"
#include "main/php.h"
#include "zend_list.h"

/*===============================================================*/
/* Public API */

/*--------------------*/

PHPAPI int PCS_registerDescriptors(PCS_DESCRIPTOR *list, PCS_LONG_T flags)
{
	PCS_Node *node;
	int count;

	if (! in_startup) {
		php_error(E_CORE_ERROR, "PCS_registerDescriptors() can be called during MINIT only");
		return FAILURE;
	}

	if (PCS_Utils_assertModuleIsStarted() == FAILURE) {
		return FAILURE;
	}

	count = 0;
	while (list->data) {
		/* Only descriptor version 0 can be handled */
		if (list->version != 0) {
			php_error(E_CORE_ERROR,"Cannot handle descriptor version (%d)", list->version);
			return FAILURE;
		}
		node = PCS_Tree_addFile(list->path, list->path_len, list->data
			, list->data_len, 0, flags);
		if (! node) return FAILURE;
		list++;
		count++;
	}

	return count;
}

/*--------------------*/

PHPAPI PCS_ID PCS_registerData(char *data, PCS_SIZE_T data_len
	, const char *path, PCS_SIZE_T path_len, PCS_LONG_T flags)
{
	PCS_Node *node;

	if (! in_startup) {
		php_error(E_CORE_ERROR, "PCS_registerData() can be called during MINIT only");
		return FAILURE;
	}

	if (PCS_Utils_assertModuleIsStarted() == FAILURE) {
		return FAILURE;
	}

	node = PCS_Tree_addFile(path, path_len, data, data_len, 0, flags);
	return (node ? PCS_FILE_ID(node) : FAILURE);
}

/*--------------------*/
/* Note: Stream wrapper open/opendir functions cannot be used during MINIT */
/* So, we need to revert to stdio functions. */

#define CLEANUP_PCS_registerPath() { \
	if (dir) closedir(dir); \
	if (fp) fclose(fp); \
	PFREE(data); \
	}

#define ABORT_PCS_registerPath() { \
	CLEANUP_PCS_registerPath(); \
	return FAILURE; \
	}

PHPAPI int PCS_registerPath(const char *filename, PCS_SIZE_T filename_len
	, const char *virtual_path, PCS_SIZE_T virtual_path_len, PCS_LONG_T flags)
{
	char *data = NULL, *sub_fname, *sub_vpath, *dname;
	PCS_SIZE_T datalen, sub_fname_len, sub_vpath_len;
	int status, fcount = 0;
	DIR *dir = NULL;
	struct dirent *entry;
	FILE *fp = NULL;
	struct stat st;

	if (! in_startup) {
		php_error(E_CORE_ERROR, "PCS_registerPath() can be called during MINIT only");
		ABORT_PCS_registerPath();
	}

	if (PCS_Utils_assertModuleIsStarted() == FAILURE) {
		ABORT_PCS_registerPath();
	}


	if (stat(filename, &st)) {
		php_error(E_CORE_ERROR, "%s: %s (errno=%d)", filename, strerror(errno), errno);
		ABORT_PCS_registerPath();
	}

	/* If path is a directory */

	if (S_ISDIR(st.st_mode)) {
		if (! PCS_Tree_addDir(virtual_path, virtual_path_len, flags)) {
			ABORT_PCS_registerPath();
		}
		
		/* Recurse on dir entries */

		dir = opendir(filename);
#ifdef PHP_WIN32
		if (!dir) {
			php_win32_docref2_from_error(GetLastError(), path, path);
			ABORT_PCS_registerPath();
			}

		if (dir->finished) {
			ABORT_PCS_registerPath();
			}
#endif
		if (! dir) {
			php_error(E_CORE_ERROR, "%s: %s (errno=%d)", filename, strerror(errno), errno);
			ABORT_PCS_registerPath();
		}

		/* No need to use readdir_r() as this is run during MINIT only */
		while (1) {
			entry = readdir(dir);
			if (! entry) {
				break;
			}
			dname = entry->d_name;
			if ((dname[0] == '.') && ((dname[1] == '\0') || ((dname[1] == '.') || (dname[2] == '\0')))) {
				continue;
			}
			spprintf(&sub_fname, 0, "%s%c%s", filename, PHP_DIR_SEPARATOR, dname);
			sub_fname_len = strlen(sub_fname);
			spprintf(&sub_vpath, 0, "%s/%s", virtual_path, dname);
			sub_vpath_len = strlen(sub_vpath);
			status = PCS_registerPath(sub_fname, sub_fname_len, sub_vpath, sub_vpath_len, flags);
			EFREE(sub_fname);
			EFREE(sub_vpath);
			if (status == FAILURE) {
				ABORT_PCS_registerPath();
			}
			fcount += status;
		}
	
	/* If path is a regular file */

	} else if (S_ISREG(st.st_mode)) {
		
		fp=fopen(filename, "rb");
		if (!fp) {
			php_error(E_CORE_ERROR, "%s: %s (errno=%d)", filename, strerror(errno), errno);
			ABORT_PCS_registerPath();
		}
		fstat(fileno(fp),&st);
		datalen = (PCS_SIZE_T)(st.st_size);
		PALLOCATE(data, datalen + 1);
		if (datalen) {
			while (!fread(data, datalen, 1, fp)) {}
		}
		data[datalen]='\0';
		if (! PCS_Tree_addFile(virtual_path, virtual_path_len, data, datalen, 1, flags)) {
			ABORT_PCS_registerPath();
		}
		data = NULL;
		fcount = 1;
	
	/* Other file types are silently ignored */
	}

	CLEANUP_PCS_registerPath();
	return fcount;
}

/*--------------------*/

PHPAPI int PCS_loadScript(PCS_ID id, int throw TSRMLS_DC)
{
	PCS_Node *node;

	if (in_startup) {
		if (throw) {
			THROW_EXCEPTION("PCS_loadScript() cannot be called during MINIT");
		}
		return FAILURE;
	}

	if (PCS_Utils_assertModuleIsStarted() == FAILURE) {
		if (throw) {
			THROW_EXCEPTION("PCS module is not active");
		}
		return FAILURE;
	}

	node = PCS_Tree_getNodeFromID(id);
	if (! node) {
		if (throw) {
			THROW_EXCEPTION_1("Cannot get PCS node from ID (%ld)", id);
		}
		return FAILURE;
	}

	return PCS_Loader_loadNode(node, throw TSRMLS_CC);
}

/*--------------------*/

PHPAPI char *PCS_getPath(PCS_ID id)
{
	PCS_Node *node;

	if (PCS_Utils_assertModuleIsStarted() == FAILURE) {
		return NULL;
	}

	node = PCS_Tree_getNodeFromID(id);
	if (! node) {
		return NULL;
	}

	return ZSTR_VAL(node->path);
}

/*--------------------*/

PHPAPI PCS_ID PCS_getID(const char *path, PCS_SIZE_T pathlen)
{
	PCS_Node *node;

	if (PCS_Utils_assertModuleIsStarted() == FAILURE) {
		return FAILURE;
	}

	node = PCS_Tree_getNodeFromPath(path, pathlen);
	if (! node) {
		return FAILURE;
	}

	return PCS_FILE_ID(node);
}

/*===============================================================*/

static zend_always_inline int MINIT_PCS_API(TSRMLS_D)
{
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static zend_always_inline int MSHUTDOWN_PCS_API(TSRMLS_D)
{
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static zend_always_inline int RINIT_PCS_API(TSRMLS_D)
{
	return SUCCESS;
}
/*---------------------------------------------------------------*/

static zend_always_inline int RSHUTDOWN_PCS_API(TSRMLS_D)
{
	return SUCCESS;
}

/*===============================================================*/
