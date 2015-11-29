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

/*---------------------------------------------------------------*/

static PHP_METHOD(PCS, fileCount)
{
	RETURN_LONG(zend_hash_num_elements(fileList));
}

/*---------------------------------------------------------------*/
/* Return array of file info
   Each file info is an array(flags/load/size/path) */

static PHP_METHOD(PCS, fileInfos)
{
	PCS_Node *node;
	char mode[2];

	array_init_size(return_value, zend_hash_num_elements(fileList));
	
	mode[1] = '\0';
	ZEND_HASH_FOREACH_PTR(fileList, node) {
		mode[0] = PCS_Tree_LoadModeToDisplay(node);

		{
#ifdef PHP_7
		zval zv, elt;
		HashTable *ht;

		array_init_size(&elt, 4);
		ht = Z_ARRVAL(elt);

		ZVAL_LONG(&zv, (long)(node->flags));
		zend_hash_str_update(ht, "flags", 5, &zv);

		ZVAL_STRINGL(&zv, mode, 1);
		zend_hash_str_update(ht, "load", 4, &zv);

		ZVAL_LONG(&zv, (long)(PCS_FILE_LEN(node)));
		zend_hash_str_update(ht, "size", 4, &zv);

		zend_string_addref(node->path);
		ZVAL_STR(&zv, node->path);
		zend_hash_str_update(ht, "path", 4, &zv);

		zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &elt);
#else
		zval *elt;

		MAKE_STD_ZVAL(elt);
		array_init_size(elt, 4);
		
		add_assoc_long(elt, "flags", (long)(node->flags));
		add_assoc_stringl(elt, "load", mode, 1, 1);
		add_assoc_long(elt, "size", (long)(PCS_FILE_LEN(node)));
		add_assoc_stringl(elt, "path", ZSTR_VAL(node->path), ZSTR_LEN(node->path), 1);

		add_next_index_zval(return_value, elt);
#endif
		}
	} ZEND_HASH_FOREACH_END();
}

/*---------------------------------------------------------------*/
/* Return array of symbol info
   Each symbol info is an array(type/name/path/load) */

static PHP_METHOD(PCS, symbolInfos)
{
	PCS_Node *node;
	char mode[2];

	array_init_size(return_value, zend_hash_num_elements(symbols));
	
	mode[1] = '\0';
	{
#ifdef PHP_7
	zval zv, elt;
	HashTable *ht;
	zend_string *key, *zs;
	char *type;

	ZEND_HASH_FOREACH_STR_KEY_PTR(symbols, key, node) {
		mode[0] = PCS_Tree_LoadModeToDisplay(node);

		array_init_size(&elt, 4);
		ht = Z_ARRVAL(elt);

		type = PCS_Loader_keyTypeString(ZSTR_VAL(key)[0]);
		zs = zend_string_init(type, strlen(type), 0);
		ZVAL_STR(&zv, zs);
		zend_hash_str_update(ht, "type", 4, &zv);

		zs = zend_string_init(ZSTR_VAL(key) + 1, ZSTR_LEN(key) - 1, 0);
		ZVAL_STR(&zv, zs);
		zend_hash_str_update(ht, "name", 4, &zv);

		ZVAL_STRINGL(&zv, mode, 1);
		zend_hash_str_update(ht, "load", 4, &zv);

		zend_string_addref(node->path);
		ZVAL_STR(&zv, node->path);
		zend_hash_str_update(ht, "path", 4, &zv);

		zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &elt);
	} ZEND_HASH_FOREACH_END();
#else
	zval *elt;
	HashPosition pos; \
	char *key;
	size_t keylen;
	char *type;

	for (zend_hash_internal_pointer_reset_ex(symbols, &pos) \
		;;zend_hash_move_forward_ex(symbols, &pos)) { \
		if (zend_hash_has_more_elements_ex(symbols, &pos) != SUCCESS) break;
		node = compat_zend_hash_get_current_data_ptr_ex(symbols, &pos);
		compat_zend_hash_str_get_current_key_ex(symbols, &key, &keylen, NULL, 0, &pos);
		
		MAKE_STD_ZVAL(elt);
		array_init_size(elt, 4);
		
		type = PCS_Loader_keyTypeString(key[0]);
		add_assoc_string(elt, "type", type, 1);
		add_assoc_stringl(elt, "name", key + 1, keylen - 1, 1);
		
		mode[0] = PCS_Tree_LoadModeToDisplay(node);
		add_assoc_stringl(elt, "load", mode, 1, 1);
		add_assoc_stringl(elt, "path", ZSTR_VAL(node->path), ZSTR_LEN(node->path), 1);

		add_next_index_zval(return_value, elt);
	}
#endif
	}
}

/*===============================================================*/

static zend_always_inline int MINIT_PCS_Info(TSRMLS_D)
{
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static zend_always_inline int MSHUTDOWN_PCS_Info(TSRMLS_D)
{
	return SUCCESS;
}

/*---------------------------------------------------------------*/

static zend_always_inline int RINIT_PCS_Info(TSRMLS_D)
{
	return SUCCESS;
}
/*---------------------------------------------------------------*/

static zend_always_inline int RSHUTDOWN_PCS_Info(TSRMLS_D)
{
	return SUCCESS;
}

/*===============================================================*/
