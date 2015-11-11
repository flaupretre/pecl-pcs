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

#ifndef __PCS_STREAM_H
#define __PCS_STREAM_H

#include "Zend/zend_hash.h"

/*============================================================================*/

typedef struct {
	int persistent;
	zend_off_t offset;
	int show_errors;
	PCS_Node *node;
	HashPosition pos;	/* readdir() position */
} PCS_STREAM_DATA;

/*----------------------------------------*/

#define PCS_DP_FILE_DATA(_dp)	(_dp)->node.u.f.data
#define PCS_DP_FILE_LEN(_dp)	(_dp)->node.u.f.len
#define PCS_DP_DIR_HT(_dp)		(_dp)->node.u.d.items

/*============================================================================*/

static PCS_STREAM_DATA *new_dp(int show_errors, int persistent);
static void free_dp(PCS_STREAM_DATA **dpp);
static size_t PCS_Stream_read(php_stream *stream, char *buf, size_t count TSRMLS_DC);
static int PCS_Stream_close(php_stream *stream, int close_handle TSRMLS_DC);
static int PCS_Stream_seek(php_stream *stream, zend_off_t offset, int whence, zend_off_t *newoffset TSRMLS_DC);
static PCS_Node *PCS_Stream_getNodeFromURI(const char *uri, size_t len);
static int do_stat(php_stream_wrapper *wrapper, const char *uri, PCS_STREAM_DATA *dp, php_stream_statbuf *ssb TSRMLS_DC);
static int PCS_Stream_fstat(php_stream *stream, php_stream_statbuf *ssb TSRMLS_DC);
static size_t PCS_Stream_readdir(php_stream *stream, char *buf, size_t count TSRMLS_DC);
static int PCS_Stream_seekdir(php_stream *stream, zend_off_t offset, int whence, zend_off_t *newoffset TSRMLS_DC);
static php_stream *PCS_Stream_generic_open(int dir, php_stream_wrapper *wrapper, const char *uri, const char *mode, int options, OPENED_PATH_PTR *opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);
static php_stream *PCS_Stream_openfile(php_stream_wrapper * wrapper, COMPAT_STREAM_CONST_DECL char *uri, COMPAT_STREAM_CONST_DECL char *mode, int options, OPENED_PATH_PTR *opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);
static int PCS_Stream_url_stat(php_stream_wrapper *wrapper, COMPAT_STREAM_CONST_DECL char *uri, int flags, php_stream_statbuf *ssb, php_stream_context *context TSRMLS_DC);
static php_stream *PCS_Stream_opendir(php_stream_wrapper * wrapper, COMPAT_STREAM_CONST_DECL char *uri, COMPAT_STREAM_CONST_DECL char *mode, int options, OPENED_PATH_PTR *opened_path, php_stream_context *context STREAMS_DC TSRMLS_DC);

/*============================================================================*/
#endif
