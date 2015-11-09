#!/bin/sh
#
# This script is needed because 'make test' does not always report
# a non-zero code when it fails.

# Xdebug versions < 2.3.3 causes PHP to crash when enabled with PCS ext (see
# http://bugs.xdebug.org/view.php?id=1151).
# **TODO: To be confirmed on PCS
# Note that xdebug is not pre-installed on every PHP version (so, phpenv may fail)

ret=0

phpenv config-rm xdebug.ini || :

#-- First, run in base dir (no tests)

# -m : test using valgrind
# -q : No interaction

TEST_PHP_ARGS='-m -q' make test

#-- The, test using the pcs_test client extension

make install

cd examples/pcs_test

phpize && ./configure
make -k clean all CFLAGS='-g -Wall'

TEST_PHP_ARGS='-m -q' make test

for i in tests/*.diff tests/*.mem
	do
	[ -f "$i" ] || continue
	ret=1
	echo
	echo "----------------------------"
	echo "$i"
	echo "----------------------------"
	cat $i
done















exit $ret
