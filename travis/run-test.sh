#!/bin/sh
#
# This script is needed because 'make test' does not always report
# a non-zero code when it fails.

# Xdebug versions < 2.3.3 causes PHP to crash when enabled with PCS ext (see
# http://bugs.xdebug.org/view.php?id=1151).
# **TODO: To be confirmed on PCS
# Note that xdebug is not pre-installed on every PHP version (so, phpenv may fail)

phpenv config-rm xdebug.ini || :

# -m : test using valgrind
# -q : No interaction

REPORT_EXIT_STATUS=1 TEST_PHP_ARGS='-m -q' USE_ZEND_ALLOC=0 make test

ret=0

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
