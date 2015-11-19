
PHP_ARG_ENABLE(ex1, whether to enable the EX1 extension,
[  --enable-ex1            Enable the EX1 extension])

if test "$PHP_EX1" != "no"; then
  AC_DEFINE(HAVE_EX1, 1, [Whether you have the EX1 extension])
  PHP_NEW_EXTENSION(ex1, php_ex1.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi

PHP_ADD_MAKEFILE_FRAGMENT

#-- Check if PCS is installed

AC_MSG_CHECKING([if the PCS extension is installed])
if test -f "$phpincludedir/ext/pcs/client.h" ; then
	AC_MSG_RESULT([yes])
else
	AC_MSG_RESULT([no])
	AC_MSG_ERROR([The PCS extension must be installed first])
fi
