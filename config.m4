
PHP_ARG_ENABLE(pcs, whether to enable the PHP Code Service,
[  --disable-pcs            Disable the PHP Code Service], yes)

if test "$PHP_PCS" != "no"; then
	PHP_PCS=yes
	AC_DEFINE(HAVE_PCS, 1, [Whether you have the PHP Code Service])
	if test ! -f "$srcdir/config.m4" ; then
		# If in a PHP build, force static compile
		if test "$ext_shared" = "yes" ; then
			AC_MSG_ERROR([PCS cannot be compiled as a shared module])
		fi
	fi
	PHP_NEW_EXTENSION(pcs, php_pcs.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
	# PCS must be loaded after tokenizer, pcre, and SPL
	PHP_ADD_EXTENSION_DEP(pcs, tokenizer)
	PHP_ADD_EXTENSION_DEP(pcs, pcre)
	PHP_ADD_EXTENSION_DEP(pcs, spl)
fi

PHP_ADD_MAKEFILE_FRAGMENT

PHP_INSTALL_HEADERS(ext/pcs, client.h)
