/*
  +----------------------------------------------------------------------+
  | PCS extension <http://pcs.tekwire.net>								 |
  +----------------------------------------------------------------------+
  | Copyright (c) 2015 The PHP Group									 |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,		 |
  | that is bundled with this package in the file LICENSE, and is		 |
  | available through the world-wide-web at the following url:			 |
  | http://www.php.net/license/3_01.txt.								 |
  | If you did not receive a copy of the PHP license and are unable to	 |
  | obtain it through the world-wide-web, please send a note to			 |
  | license@php.net so we can mail you a copy immediately.				 |
  +----------------------------------------------------------------------+
  | Author: Francois Laupretre <francois@tekwire.net>					 |
  +----------------------------------------------------------------------+
*/

/*---------------------------------------------------------------*/

static php_stream_ops pcs_ops = {
	NULL,
	PCS_Stream_read,
	PCS_Stream_close,
	NULL,
	"pcs",
	PCS_Stream_seek,
	NULL,				/* cast */
	PCS_Stream_fstat,
	NULL				/* set_option */
};

static php_stream_ops pcs_dirops = {
	NULL,				/* write */
	PCS_Stream_readdir,
	PCS_Stream_close,
	NULL,				/* flush */
	"pcs",
	PCS_Stream_seekdir,
	NULL,				/* cast */
	NULL,				/* stat */
	NULL				/* set_option */
};

static php_stream_wrapper_ops pcs_stream_wops = {
	PCS_Stream_openfile,		/* open */
	NULL,						/* close */
	NULL,						/* stat, */
	PCS_Stream_url_stat,		/* stat_url */
	PCS_Stream_opendir,			/* opendir */
	"pcs",
	NULL,						/* unlink */
	NULL,						/* rename */
	NULL,						/* mkdir */
	NULL						/* rmdir */
};

static php_stream_wrapper php_stream_pcs_wrapper = {
	&pcs_stream_wops,
	NULL,
	0							/* is_url */
};

/*============================================================================*/
/* Allocate and intialize abstract data */

static PCS_STREAM_DATA *new_dp(int show_errors, int persistent)
{
	PCS_STREAM_DATA *dp = NULL;

	PEALLOCATE(dp, sizeof(PCS_STREAM_DATA), persistent);
	CLEAR_DATA(*dp);
	dp->persistent = persistent;

	dp->show_errors = show_errors;

	return dp;
}

/*--------------------*/
/* Free abstract data */

static void free_dp(PCS_STREAM_DATA **dpp)
{
	PCS_STREAM_DATA *dp;

	if ((!dpp) || (!(dp = *dpp))) {
		return;
	}

	PEFREE(dp, dp->persistent);
}

/*--------------------*/
/* File read */

static size_t PCS_Stream_read(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	size_t max;
	PCS_STREAM_DATA *dp = stream->abstract;

	ZEND_ASSERT(PCS_NODE_IS_FILE(dp->node));

	max = PCS_FILE_LEN(dp->node) - dp->offset;
	if (max < 0) max = 0;	/* Should not happen */
	if (count > max) count = max;

	if (count) {
		memmove(buf, PCS_FILE_DATA(dp->node) + dp->offset, count);
	}

	dp->offset += count;
	stream->eof = (PCS_FILE_LEN(dp->node) == dp->offset);

	return count;
}

/*--------------------*/
/* Close a file or a directory */

static int PCS_Stream_close(php_stream *stream, int close_handle TSRMLS_DC)
{
	PCS_STREAM_DATA *dp = stream->abstract;

	free_dp(&dp);

	return 0;
}

/*--------------------*/
/* File seek */
/* Warning: zend_off_t is signed but size_t is not. (zend_off_t) casts below are
   mandatory for signed operations. */

static int PCS_Stream_seek(php_stream *stream, zend_off_t offset, int whence
						   , zend_off_t *newoffset TSRMLS_DC)
{
	PCS_STREAM_DATA *dp = stream->abstract;

	ZEND_ASSERT(PCS_NODE_IS_FILE(dp->node));
	/*DBG_MSG3("start: dp->offset=%d, offset=%d, whence=%d\n",dp->offset,offset,whence);*/

	switch (whence) {
		case SEEK_SET:
			dp->offset = offset;
			break;

		case SEEK_CUR:
			dp->offset += offset;
			break;


		case SEEK_END:
			dp->offset = (zend_off_t)PCS_FILE_LEN(dp->node) + offset;
			break;
	}

	if (dp->offset > (zend_off_t)PCS_FILE_LEN(dp->node)) dp->offset = PCS_FILE_LEN(dp->node);
	if (dp->offset < 0) dp->offset = (zend_off_t)0;

	if (newoffset) (*newoffset) = dp->offset;
	stream->eof = (dp->offset == (zend_off_t)PCS_FILE_LEN(dp->node));

	/*DBG_MSG2("end: offset=%d, stream->eof=%d\n",dp->offset,stream->eof);*/
	return 0;
}

/*--------------------*/

static PCS_Node *PCS_Stream_getNodeFromURI(const char *uri, size_t len)
{
	PCS_Node *node;

	ZEND_ASSERT(uri);
	DBG_MSG1("-> PCS_Stream_getNodeFromURI(%s)", uri);

	if (len < 6) return NULL;
	node = PCS_Tree_getNodeFromPath(uri + 6, len - 6);
	if (! node) {
		DBG_MSG1("*** PCS_Stream_getNodeFromURI(%s) failed", uri);
	} else {
		DBG_MSG2("<- PCS_Stream_getNodeFromURI(%s) => %s", uri, ZSTR_VAL(node->path));
	}

	return node;
}

/*--------------------*/
/* Stat on a file or dir (opened or not) */

static int do_stat(php_stream_wrapper *wrapper, const char *uri
	, PCS_STREAM_DATA *dp, php_stream_statbuf *ssb TSRMLS_DC)
{
	DBG_MSG1("-> do_stat(%s)", uri);

	/*-- Get node */

	if (!(dp->node)) {
		dp->node = PCS_Stream_getNodeFromURI(uri, (size_t)strlen(uri));
		if (!(dp->node)) {
			php_stream_wrapper_log_error(wrapper, dp->show_errors TSRMLS_CC
										 , "%s: File not found", uri);
			return -1;
		}
	}

	memset(ssb, 0, sizeof(*ssb));

	ssb->sb.st_size = (off_t)(PCS_NODE_IS_DIR(dp->node)
		? zend_hash_num_elements(PCS_DIR_HT(dp->node)) : PCS_FILE_LEN(dp->node));
	ssb->sb.st_mode = (mode_t) (PCS_NODE_IS_DIR(dp->node) ? S_IFDIR|0555 : S_IFREG|0444);
	ssb->sb.st_nlink = 1;
#ifdef HAVE_ST_RDEV
	ssb->sb.st_rdev = -1;
#endif
#ifdef HAVE_ST_BLKSIZE
	ssb->sb.st_blksize = -1;
#endif
#ifdef HAVE_ST_BLOCKS
	ssb->sb.st_blocks = -1;
#endif

	DBG_MSG("<- do_stat()");
	return 0;
}

/*--------------------*/
/* Stat on an open file/dir */

static int PCS_Stream_fstat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC)
{
	PCS_STREAM_DATA *dp = stream->abstract;

	if (!ssb) return -1;

	return do_stat(stream->wrapper, stream->orig_path, dp, ssb TSRMLS_CC);
}

/*---------------------------------------------------------------*/
/* readdir */

static size_t PCS_Stream_readdir(php_stream *stream, char *buf, size_t count TSRMLS_DC)
{
	php_stream_dirent *ent = (php_stream_dirent *) buf;
	PCS_STREAM_DATA *dp = stream->abstract;
	HashTable *ht;
	char *name;
	size_t nlen;

	ZEND_ASSERT(PCS_NODE_IS_DIR(dp->node));

	ht = PCS_DIR_HT(dp->node);

	if (compat_zend_hash_str_get_current_key_ex(ht, &name, &nlen, NULL, 0
		, &(dp->pos)) == HASH_KEY_NON_EXISTENT) {
		stream->eof = 1;
		return 0;
	}

	count =	MIN((size_t)sizeof(ent->d_name) - 1, nlen);
	memmove(ent->d_name, name, count);
	ent->d_name[count] = '\0';

	zend_hash_move_forward_ex(ht, &(dp->pos));
	stream->eof = (zend_hash_has_more_elements_ex(ht, &(dp->pos)) == FAILURE);

	return sizeof(php_stream_dirent);
}

/*--------------------*/
/* rewinddir */

static int PCS_Stream_seekdir(php_stream *stream, zend_off_t offset
							  , int whence, zend_off_t *newoffset TSRMLS_DC)
{
	PCS_STREAM_DATA *dp = stream->abstract;
	HashTable *ht;

	ZEND_ASSERT(PCS_NODE_IS_DIR(dp->node));

	ht = PCS_DIR_HT(dp->node);
	if ((whence == SEEK_SET) && (!offset)) {	/* rewinddir */
		zend_hash_internal_pointer_reset_ex(ht, &(dp->pos));
		stream->eof = (zend_hash_has_more_elements_ex(ht, &(dp->pos)) == SUCCESS);
		if (newoffset) (*newoffset) = (zend_off_t)0;
		return 0;
	}

	return -1;
}

/*---------------------------------------------------------------*/
/* Used to open a file or a directory */
/* Support persistent streams */

#define ABORT_PCS_STREAM_OPEN() \
	{ \
	DBG_MSG("<** Aborting generic_open()"); \
	free_dp(&dp); \
	EFREE(persistent_id); \
	return NULL; \
	}

static php_stream *PCS_Stream_generic_open(int dir, php_stream_wrapper *wrapper
	, const char *uri, const char *mode, int options, OPENED_PATH_PTR *opened_path
	, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	PCS_STREAM_DATA *dp = NULL;
	size_t uri_len;
	char open_flags;
	php_stream *ret;
	int persistent = options & STREAM_OPEN_PERSISTENT;
	char *persistent_id = NULL;

	DBG_MSG2("-> generic_open(%s %s)", (dir ? "dir" : "file"), uri);

	uri_len = (size_t)strlen(uri);

	/*-- For files, support read mode only ('r' or 'rb') */

	if ((!dir) && ((mode[0] != 'r') || (mode[1] && mode[1] != 'b'))) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC
			, "'%s' mode not supported (read-only access)", mode);
		ABORT_PCS_STREAM_OPEN();
	}
	open_flags = (((!dir) && (mode[1] == 'b')) ? 'b' : 't');

	/*-- Allocate the private ('abstract') data */

	dp = new_dp(options & REPORT_ERRORS, persistent);

	/*-- Get node */

	dp->node = PCS_Stream_getNodeFromURI(uri, uri_len);
	if (!(dp->node))
		{
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC
									 , "%s: File not found", uri);
		ABORT_PCS_STREAM_OPEN();
	}

	/* Check node type */

	if (dir && (!PCS_NODE_IS_DIR(dp->node))) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC
									 , "%s: Node is not a directory", uri);
		ABORT_PCS_STREAM_OPEN();
	}
	if ((!dir) && (!PCS_NODE_IS_FILE(dp->node))) {
		php_stream_wrapper_log_error(wrapper, options TSRMLS_CC
									 , "%s: Node is not a regular file", uri);
		ABORT_PCS_STREAM_OPEN();
	}

	/*-- Persitent open */

	if (persistent) {
		spprintf(&persistent_id, 0, "streams_pcs_%c_%s", open_flags
			, ZSTR_VAL(dp->node->path));
		switch (php_stream_from_persistent_id(persistent_id, &ret TSRMLS_CC)) {
			case PHP_STREAM_PERSISTENT_SUCCESS:
				if (opened_path) {
#ifdef PHP_7
					zend_string_addref(dp->node->uri);
					(*opened_path) = dp->node->uri;
#else
					(*opened_path) = ut_eduplicate(ZSTR_VAL(dp->node->uri)
						, ZSTR_LEN(dp->node->uri) + 1);
#endif
				}
			/* fall through */
			case PHP_STREAM_PERSISTENT_FAILURE:
				EFREE(persistent_id);;
				return ret;
			/* Default: PHP_STREAM_PERSISTENT_NOT_EXIST => Continue */
		}
	}

	/* Init dp data */

	if (dir) {
		zend_hash_internal_pointer_reset_ex(PCS_DIR_HT(dp->node), &(dp->pos));
	} else { /* File */
		dp->offset = 0;	/*-- Initialize offset */
	}

	if (opened_path) { /* Return canonical path as opened path */
#ifdef PHP_7
		zend_string_addref(dp->node->uri);
		(*opened_path) = dp->node->uri;
#else
		(*opened_path) = ut_eduplicate(ZSTR_VAL(dp->node->uri)
			, ZSTR_LEN(dp->node->uri) + 1);
#endif
	}

	DBG_MSG("<- generic_open()");
	ret = php_stream_alloc((dir ? &pcs_dirops : &pcs_ops), dp, persistent_id, mode);

	EFREE(persistent_id);
	return ret;
}

/*--------------------*/

static php_stream *PCS_Stream_openfile(php_stream_wrapper *wrapper
	, COMPAT_STREAM_CONST_DECL char *uri, COMPAT_STREAM_CONST_DECL char *mode, int options
	, OPENED_PATH_PTR *opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	return PCS_Stream_generic_open(0, wrapper, uri, mode, options
		, opened_path, context STREAMS_CC TSRMLS_CC);
}

/*---------------------------------------------------------------*/

static int PCS_Stream_url_stat(php_stream_wrapper *wrapper, COMPAT_STREAM_CONST_DECL char *uri
	, int flags, php_stream_statbuf *ssb, php_stream_context *context TSRMLS_DC)
{
	PCS_STREAM_DATA *dp;
	int retval;

	dp = new_dp((flags & PHP_STREAM_URL_STAT_QUIET) ? 0 : REPORT_ERRORS, 0);
	retval = do_stat(wrapper, uri, dp, ssb TSRMLS_CC);
	free_dp(&dp);

	return retval;
}

/*--------------------*/

static php_stream *PCS_Stream_opendir(php_stream_wrapper * wrapper
	, COMPAT_STREAM_CONST_DECL char *uri, COMPAT_STREAM_CONST_DECL char *mode, int options
	, OPENED_PATH_PTR *opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC)
{
	return PCS_Stream_generic_open(1, wrapper, uri, mode, options
		, opened_path, context STREAMS_CC TSRMLS_CC);
}

/*---------------------------------------------------------------*/
/* Module initialization										 */

static zend_always_inline int MINIT_PCS_Stream(TSRMLS_D)
{
	php_register_url_stream_wrapper("pcs", &php_stream_pcs_wrapper TSRMLS_CC);

	return SUCCESS;
}

/*---------------------------------------------------------------*/
/* Module shutdown												 */

static zend_always_inline int MSHUTDOWN_PCS_Stream(TSRMLS_D)
{
	php_unregister_url_stream_wrapper("pcs" TSRMLS_CC);

	return SUCCESS;
}

/*---------------------------------------------------------------*/

static zend_always_inline int RINIT_PCS_Stream(TSRMLS_D)
{
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static zend_always_inline int RSHUTDOWN_PCS_Stream(TSRMLS_D)
{
	return SUCCESS;
}

/*---------------------------------------------------------------*/
