
PHP_ARG_ENABLE(ex1, whether to enable the EX1 extension,
[  --enable-ex1            Enable the EX1 extension])

if test "$PHP_EX1" != "no"; then
  AC_DEFINE(HAVE_EX1, 1, [Whether you have the EX1 extension])
  PHP_NEW_EXTENSION(ex1, php_ex1.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
