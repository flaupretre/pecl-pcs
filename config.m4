
PHP_ARG_ENABLE(pcs, whether to enable the PHP Code Service extension,
[  --enable-pcs            Enable the PHP Code Service extension])

if test "$PHP_PCS" != "no"; then
  AC_DEFINE(HAVE_PCS, 1, [Whether you have the PHP Code Service extension])
  PHP_NEW_EXTENSION(pcs, php_pcs.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
