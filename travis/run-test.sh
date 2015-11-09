#!/bin/sh
# Run PCS base and example clients tests
#=============================================================================

run_tests()
{
_ret=0

phpize && ./configure && make -k clean all CFLAGS='-g -Wall'

# -m : test using valgrind
# -q : No interaction

TEST_PHP_ARGS='-m -q' make test

for i in tests/*.diff tests/*.mem
	do
	[ -f "$i" ] || continue
	_ret=1
	echo
	echo "----------------------------"
	echo "$i"
	echo "----------------------------"
	cat $i
done

return $_ret
}

#----------------

ret=0

# Xdebug versions < 2.3.3 causes PHP to crash when enabled with PCS ext (see
# http://bugs.xdebug.org/view.php?id=1151).
# **TODO: To be confirmed on PCS
# Note that xdebug is not pre-installed on every PHP version (so, phpenv may fail)

phpenv config-rm xdebug.ini || :

valgrind --version

#-- First, run base dir tests

run_tests
[ $? != 0 ] && ret=$?

#-- Then, test the client extensions

make install

cd examples/pcs_test

run_tests
[ $? != 0 ] && ret=$?

cd ../example1

run_tests
[ $? != 0 ] && ret=$?

#----

exit $ret
