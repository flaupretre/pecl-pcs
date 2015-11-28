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

#include <string.h>
#include "php_streams.h"
#include "ext/standard/php_filestat.h"

/*---------------------------------------------------------------*/
/* Add a subnode to an existing directory node

Note:
   - File data is not initialized.
   - Adding an already-existing subdir is OK.
   - name arg doesn't have to be null-terminated.
   - '.' and '..' are forbidden as node name
   - Null 'parent' means we are creating the root node
   - On error, returns NULL
*/

static PCS_Node *PCS_Tree_addSubNode(PCS_Node *parent, const char *name
	, size_t len, int type, zend_ulong flags)
{
	PCS_Node *node;
	char *p;

	ZEND_ASSERT(!(parent && (len <= 0))); /* Accept empty name for root only */

	if ((len <= 2) && (name[0]=='.') && ((name[1]=='.') || (len == 1))) {
		php_error(E_CORE_ERROR, "Cannot create node named '.' or '..'");
		return NULL;
	}
	
	/* Check if entry already exists */

	if (parent) {
		node = (PCS_Node *)zend_hash_str_find_ptr(PCS_DIR_HT(parent), name, len);
		if (node) {
			if (PCS_NODE_IS_DIR(node)) {
				if (type == PCS_TYPE_DIR) {
					return node; /* Dir exists already, return it */
				} else {
					php_error(E_CORE_ERROR
						,"%s: Cannot create file, a directory with that name does already exist"
						, ZSTR_VAL(node->path)
						);
					return NULL;
				}
			} else { /* Existing node is a file -> error */
					php_error(E_CORE_ERROR
						,"%s: Cannot create %s, a file with that name does already exist"
						, ZSTR_VAL(node->path)
						, ((type == PCS_TYPE_DIR) ? "dir" : "file"));
					return NULL;
			}
		}
	}

	/* Create node (type-common) */

	node = ut_pallocate(NULL, sizeof(PCS_Node));
	CLEAR_DATA(*node);

	node->parent = parent;
	node->type = type;
	node->flags = flags;

	if (parent) {
		node->path = zend_string_alloc(ZSTR_LEN(parent->path) + 1 + len, 1);
		p = ZSTR_VAL(node->path);
		if (PCS_NODE_IS_ROOT(parent)) {
			ZSTR_LEN(node->path) = len;
		} else {
			memcpy(p, ZSTR_VAL(parent->path), ZSTR_LEN(parent->path));
			p += ZSTR_LEN(parent->path);
			(*(p++)) = '/';
		}
		memcpy(p, name, len);
		p[len] = '\0';
	} else { /* Root node */
		node->path = zend_string_init("", 0, 1);
	}
	zend_string_hash_val(node->path);

	node->uri = zend_string_alloc(ZSTR_LEN(node->path) + 6, 1);
	sprintf(ZSTR_VAL(node->uri), "pcs://%s", ZSTR_VAL(node->path));
	zend_string_hash_val(node->uri);

	/* Record path in pathList (increments zend_string refcount) */

	MutexLock(pathList);
	zend_hash_add_new_ptr(pathList, node->path, node);
	MutexUnlock(pathList);

	/* Add node to parent's subnode list */

	if (parent) {
		zend_hash_str_add_new_ptr(PCS_DIR_HT(parent), name, len, node);
	}

	/* Type-specific init */

	switch (type) {
		case PCS_TYPE_DIR:
			zend_hash_init(PCS_DIR_HT(node), 8, 0, (dtor_func_t)PCS_Tree_destroyNode, 1);
			break;

		case PCS_TYPE_FILE:
			node->u.f.id = zend_hash_next_free_element(fileList);
			zend_hash_next_index_insert_ptr(fileList, node);
			break;
	}

	return node;
}

/*--------------------*/
/* On error, returns NULL */

static PCS_Node *PCS_Tree_addNode(const char *path, size_t path_len
	, int type, zend_ulong flags)
{
	PCS_Node *node;
	size_t remaining, len;
	const char *start, *found;
	zend_string *cpath;

	cpath = PCS_Tree_cleanPath(path, path_len);
	start = ZSTR_VAL(cpath);
	node = root;
	while (1) {
		remaining = ZSTR_LEN(cpath) - (size_t)(start - ZSTR_VAL(cpath));
		found = memchr(start, '/', remaining);
		len = (found ? (size_t)(found - start) : remaining);
		node = PCS_Tree_addSubNode(node, start, len, (found ?  PCS_TYPE_DIR : type)
			, flags);
		if (! node) return NULL;
		if (! found) break;
		start = found + 1;
	}

	zend_string_release(cpath);
	return node;
}

/*--------------------*/

static PCS_Node *PCS_Tree_addDir(const char *path, size_t path_len, zend_ulong flags)
{
	return PCS_Tree_addNode(path, path_len, PCS_TYPE_DIR, flags);
}

/*--------------------*/

static PCS_Node *PCS_Tree_addFile(const char *path, size_t path_len
	, char *data, size_t datalen, int alloc, zend_ulong flags)
{
	PCS_Node *node;

	node = PCS_Tree_addNode(path, path_len, PCS_TYPE_FILE, flags);
	if (! node) return NULL;

	node->u.f.data = data;
	node->u.f.len = datalen;
	node->u.f.alloc = alloc;

	return node;
}

/*--------------------*/

static void PCS_Tree_destroyNode(zval *zp)
{
	PCS_Node **nodep, *node;

	nodep = (PCS_Node **)COMPAT_HASH_PTR(zp);
	node = *nodep;

#if ZEND_DEBUG
	node->flags |= PCS_FLAG_NOCHECK; /* Checks will fail during tree destruction */
#endif

	zend_string_release(node->path);
	zend_string_release(node->uri);

	if (PCS_NODE_IS_DIR(node)) {
		zend_hash_destroy(PCS_DIR_HT(node));
	} else {
		if (PCS_FILE_ALLOC(node)) {
			ut_pallocate(PCS_FILE_DATA(node), 0);
		}
	}

	PFREE(*nodep); /* Security: free and set null ptr */
}

/*--------------------*/
/* Returns a newly-allocated string (must be released by caller)
   Replace '\' with '/'
   Remove leading and trailing '/', multiple '/'
   Note : 'name' input arg does not have to be null-terminated.
*/

static zend_string *PCS_Tree_cleanPath(const char *path, size_t len)
{
	size_t dlen, remain;
	int ignore_slash;
	zend_string *ret;
	char *target, c;

	ret = zend_string_alloc(len, 0);
	target = ZSTR_VAL(ret);
	for(ignore_slash = 1, remain = len ; remain ; path++, remain--) {
		c = (*path);
		if (c == '\\') c = '/';
		if (c == '/') {
			if (ignore_slash) {
				continue;
			} else {
				ignore_slash = 1;
			}
		} else {
			ignore_slash = 0;
		}
		*(target++) = c;
	}
	dlen = (size_t)(target - ZSTR_VAL(ret));
	target--;
	while (dlen && ((*target) == '/')) {
		dlen--;
		target--;
	}
	*(target + 1) = '\0';

	ZSTR_LEN(ret) = dlen;
	return ret;
}

/*--------------------*/
/* Resolve a path not existing in pathList
   On entry, path is 'clean', ie produced by PCS_Tree_cleanPath()
   Handles '.' and '..' special dir entries
   Returns NULL if node does not exist
*/

static PCS_Node *PCS_Tree_resolvePath(zend_string *path)
{
	PCS_Node *node;
	size_t remaining, len;
	char *start, *end;

	DBG_MSG1("-> PCS_Tree_resolvePath(%s)", ZSTR_VAL(path));

	node = root;
	start = ZSTR_VAL(path);
	while (1) {
		remaining = ZSTR_LEN(path) - (size_t)(start - ZSTR_VAL(path));
		end = memchr(start, '/', remaining);
		len = (end ? (size_t)(end - start) : remaining);
		if ((len == 2) && (start[0] == '.') && (start[1] == '.')) {
			if (node->parent) {
				node = node->parent;
			}
		} else if ((len != 1) || (start[0] != '.')) {
			/* '.' remains on the same node */
			node = zend_hash_str_find_ptr(PCS_DIR_HT(node), start
										  , (end ? len : remaining));
		}
		if ((!end) || (!node)) break;
		if (! PCS_NODE_IS_DIR(node)) return NULL;
		start = end + 1;
	}
	DBG_MSG("<- PCS_Tree_resolvePath()");
	return node;
}

/*--------------------*/
/* If path is valid but not in pathList, add it using the mutex */

static PCS_Node *PCS_Tree_getNodeFromPath(const char *path, size_t len)
{
	zend_string *cpath, *pcpath;
	PCS_Node *node;

	DBG_MSG1("-> PCS_Tree_getNodeFromPath(%s)", path);

	cpath = PCS_Tree_cleanPath(path, len);

	if (UNEXPECTED(!ZSTR_LEN(cpath))) {
		node = root;
	} else {
		node = zend_hash_find_ptr(pathList, cpath);
		if (!node) {
			node = PCS_Tree_resolvePath(cpath);
			if (node) { /* Register unknown path in path list */
				DBG_MSG1("%s: Registering path in pathList", ZSTR_VAL(cpath));
				MutexLock(pathList);
				pcpath = zend_string_dup(cpath, 1); /* Make persistent */
				zend_hash_add_new_ptr(pathList, pcpath, node);
				MutexUnlock(pathList);
			}
		}
	}

	zend_string_release(cpath);
	return node;
}

/*--------------------*/

static PCS_Node *PCS_Tree_getNodeFromID(PCS_ID id)
{
	return zend_hash_index_find_ptr(fileList, (zend_ulong)id);
}

/*--------------------*/

static char PCS_Tree_LoadModeToDisplay(const PCS_Node *node)
{
	char *modes="!AR-";

	return modes[node->load_mode & PCS_LOAD_MASK];
}

/*===============================================================*/

static zend_always_inline int MINIT_PCS_Tree(TSRMLS_D)
{
	/* Init path list */
	/* No destructor because PCS_Node structs are destroyed with the tree */

	MutexSetup(pathList);
	pathList = ut_pallocate(NULL, sizeof(*pathList));
	zend_hash_init(pathList, 32, 0, NULL, 1);

	/* Init file list */

	fileList = ut_pallocate(NULL, sizeof(*fileList));
	zend_hash_init(fileList, 32, 0, NULL, 1);

	/* Create root node */

	root = PCS_Tree_addSubNode(NULL, "", 0, PCS_TYPE_DIR, 0);

	return SUCCESS;
}

/*---------------------------------------------------------------*/

static zend_always_inline int MSHUTDOWN_PCS_Tree(TSRMLS_D)
{

	/* Free path list */

	zend_hash_destroy(pathList);
	PFREE(pathList);
	MutexShutdown(pathList);

	/* Free file list */

	zend_hash_destroy(fileList);
	PFREE(fileList);

	/* Destroy tree */

#ifdef PHP_7
	{
	zval zv;
	
	ZVAL_PTR(&zv, root);
	PCS_Tree_destroyNode(&zv);
	root = NULL;
	}
#else
	PCS_Tree_destroyNode((zval *)(&root));
#endif
	
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static zend_always_inline int RINIT_PCS_Tree(TSRMLS_D)
{
	return SUCCESS;
}
/*---------------------------------------------------------------*/

static zend_always_inline int RSHUTDOWN_PCS_Tree(TSRMLS_D)
{
	return SUCCESS;
}

/*===============================================================*/
