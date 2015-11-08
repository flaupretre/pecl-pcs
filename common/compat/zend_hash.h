/*
  +----------------------------------------------------------------------+
  | zend_hash compatibility layer for PHP 5 & 7							 |
  +----------------------------------------------------------------------+
  | Copyright (c) 2005-2007 The PHP Group								 |
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

#ifndef _COMPAT_ZEND_HASH_H
#define _COMPAT_ZEND_HASH_H

#include "zend_hash.h"

#ifdef PHP_7
/*============================================================================*/

#define compat_zend_hash_exists(ht, key) zend_hash_exists(ht, key)

#define compat_zend_hash_str_exists(ht, str, len) zend_hash_str_exists(ht, str, len)

#else
/*============================================================================*/

#define  HASH_KEY_NON_EXISTENT HASH_KEY_NON_EXISTANT

/*------------------------------------------------*/

static zend_always_inline void *zend_hash_add_ptr(HashTable *ht
	, zend_string *key, void *pData)
{
	int status;

	status = zend_hash_quick_add(ht, ZSTR_VAL(key), (uint)ZSTR_LEN(key), (ulong)ZSTR_HASH(key), &pData, sizeof(void *), NULL);
	return (status == SUCCESS ? pData : NULL);
}

#define zend_hash_add_new_ptr(ht, key, pData) zend_hash_add_ptr(ht, key, pData)

/*---------*/

static zend_always_inline void *zend_hash_add_mem(HashTable *ht
	, zend_string *key, void *pData, size_t size)
{
	int status;
	void *p;

	p = pemalloc(size, ht->persistent);
	status = zend_hash_quick_add(ht, ZSTR_VAL(key), (uint)ZSTR_LEN(key), (ulong)ZSTR_HASH(key), &p, sizeof(void *), NULL);
	return (status == SUCCESS ? p : NULL);
}

/*---------*/

static zend_always_inline void *zend_hash_str_add_ptr(HashTable *ht
	, const char *str, size_t len, void *pData)
{
	int status;

	status = zend_hash_quick_add(ht, str, len
		, zend_get_hash_value(str, len + 1), &pData, sizeof(void *), NULL);
	return (status == SUCCESS ? pData : NULL);
}

#define zend_hash_str_add_new_ptr(ht, str, len, pData) zend_hash_str_add_ptr(ht, str, len, pData)

/*---------*/

static zend_always_inline void *zend_hash_str_add_mem(HashTable *ht
	, const char *str, size_t len, void *pData, size_t size)
{
	int status;
	void *p;

	p = pemalloc(size, ht->persistent);
	status = zend_hash_quick_add(ht, str, len
		, zend_get_hash_value(str, len + 1), &p, sizeof(void *), NULL);
	return (status == SUCCESS ? p : NULL);
}

/*---------*/

static zend_always_inline void *zend_hash_index_add_ptr(HashTable *ht
	, zend_ulong h, void *pData)
{
	int status;

	status = zend_hash_index_update(ht, (ulong)h, &pData, sizeof(void *), NULL);
	return (status == SUCCESS ? pData : NULL);
}

#define zend_hash_index_add_new_ptr(ht, h, pData) \
	zend_hash_index_add_ptr(ht, h, pData)

/*---------*/

static zend_always_inline void *zend_hash_index_add_mem(HashTable *ht
	, zend_ulong h, void *pData, size_t size)
{
	int status;
	void *p;

	p = pemalloc(size, ht->persistent);
	status = zend_hash_index_update(ht, (ulong)h, &p, sizeof(void *), NULL);
	return (status == SUCCESS ? p : NULL);
}

/*---------*/

static zend_always_inline void *zend_hash_next_index_insert_ptr(HashTable *ht
	, void *pData)
{
	int status;

	status = zend_hash_next_index_insert(ht, &pData, sizeof(void *), NULL);
	return (status == SUCCESS ? pData : NULL);
}

/*---------*/

static zend_always_inline void *zend_hash_next_index_insert_mem(HashTable *ht
	, void *pData, size_t size)
{
	int status;
	void *p;

	p = pemalloc(size, ht->persistent);
	status = zend_hash_next_index_insert(ht, &p, sizeof(void *), NULL);
	return (status == SUCCESS ? p : NULL);
}

/*---------*/

static zend_always_inline void *zend_hash_update_ptr(HashTable *ht
	, zend_string *key, void *pData)
{
	int status;

	status = zend_hash_quick_update(ht, ZSTR_VAL(key), (uint)ZSTR_LEN(key), (ulong)ZSTR_HASH(key), &pData, sizeof(void *), NULL);
	return (status == SUCCESS ? pData : NULL);
}

/*---------*/

static zend_always_inline void *zend_hash_update_mem(HashTable *ht
	, zend_string *key, void *pData, size_t size)
{
	int status;
	void *p;

	p = pemalloc(size, ht->persistent);
	status = zend_hash_quick_update(ht, ZSTR_VAL(key), (uint)ZSTR_LEN(key), (ulong)ZSTR_HASH(key), &p, sizeof(void *), NULL);
	return (status == SUCCESS ? p : NULL);
}

/*---------*/

static zend_always_inline void *zend_hash_str_update_ptr(HashTable *ht
	, const char *str, size_t len, void *pData)
{
	int status;

	status = zend_hash_quick_update(ht, str, len
		, zend_get_hash_value(str, len + 1), &pData, sizeof(void *), NULL);
	return (status == SUCCESS ? pData : NULL);
}

/*---------*/

static zend_always_inline void *zend_hash_str_update_mem(HashTable *ht
	, const char *str, size_t len, void *pData, size_t size)
{
	int status;
	void *p;

	p = pemalloc(size, ht->persistent);
	status = zend_hash_quick_update(ht, str, len
		, zend_get_hash_value(str, len + 1), &p, sizeof(void *), NULL);
	return (status == SUCCESS ? p : NULL);
}

#define zend_hash_index_update_ptr(ht, h, pData) \
	zend_hash_index_add_ptr(ht, h, pData)

#define zend_hash_index_update_mem(ht, h, pData, size) \
	zend_hash_index_add_mem(ht, h, pData, size)

/*---------*/

static zend_always_inline void *zend_hash_find_ptr(const HashTable *ht
	, zend_string *key)
{
	int status;
	void *p;

	status = zend_hash_quick_find(ht, ZSTR_VAL(key), (uint)ZSTR_LEN(key), (ulong)ZSTR_HASH(key), &p);
	return (status == SUCCESS ? p : NULL);
}

/*---------*/

static zend_always_inline void *zend_hash_str_find_ptr(const HashTable *ht
	, const char *str, size_t len)
{
	int status;
	void *p;

	status = zend_hash_quick_find(ht, str, len, zend_get_hash_value(str, len + 1), &p);
	return (status == SUCCESS ? p : NULL);
}

/*---------*/

static zend_always_inline void *zend_hash_index_find_ptr(const HashTable *ht
	, zend_ulong h)
{
	int status;
	void *p;

	status = zend_hash_index_find(ht, h, &p);
	return (status == SUCCESS ? p : NULL);
}

/*---------*/

static zend_always_inline void *zend_hash_get_current_data_ptr_ex(HashTable *ht
	, HashPosition *pos)
{
	int status;
	void *p;

	status = zend_hash_get_current_data_ex(ht, &p, pos);
	return (status == SUCCESS ? p : NULL);
}

#define zend_hash_get_current_data_ptr(ht, pData) \
	zend_hash_get_current_data_ptr_ex(ht, pData, NULL)

/*---------*/

static zend_always_inline zend_bool compat_zend_hash_exists(const HashTable *ht
	, zend_string *key)
{
	return zend_hash_quick_exists(ht, ZSTR_VAL(key), (uint)ZSTR_LEN(key)
		, (ulong)ZSTR_HASH(key));
}

/*---------*/

static zend_always_inline zend_bool compat_zend_hash_str_exists(const HashTable *ht
	, const char *str, size_t len)
{
	return zend_hash_exists(ht, str, (uint)len);
}

/*------------------------------------------------------------*/

#define ZEND_HASH_FOREACH(_ht, indirect) do { \
	HashPosition _pos; \
	for (zend_hash_internal_pointer_reset_ex(_ht, &_pos) \
		;;zend_hash_move_forward_ex(_ht, &_pos)) { \
		if (zend_hash_has_more_elements_ex(_ht, &_pos) != SUCCESS) break;

#define ZEND_HASH_FOREACH_END() \
	} \
} while (0)

#define ZEND_HASH_FOREACH_PTR(_ht, _ptr) \
	ZEND_HASH_FOREACH(_ht, 0); \
	zend_hash_get_current_data_ex(_ht, &_ptr, &_pos);

#define ZEND_HASH_FOREACH_NUM_KEY(_ht, _h) \
	ZEND_HASH_FOREACH(_ht, 0); \
	{ \
	ulong _idx; \
	char *_key; \
	uint _keylen; \
	zend_hash_get_current_key_ex(_ht, &_key, &_keylen, &_h, 0, &_pos); \
	_h = (zend_ulong)_idx; \
	}

/*============================================================================*/
#endif /* PHP_7 */
#endif /* _COMPAT_ZEND_HASH_H */
