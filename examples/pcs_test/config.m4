
PHP_ARG_ENABLE(ptest, whether to enable the PCS test extension,
[  --enable-ptest            Enable the PCS test extension])

if test "$PHP_PTEST" != "no"; then
  AC_DEFINE(HAVE_PTEST, 1, [Whether you have the PCS test extension])
  PHP_NEW_EXTENSION(ptest, php_ptest.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi

PHP_ADD_MAKEFILE_FRAGMENT

