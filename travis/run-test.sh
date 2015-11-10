#!/bin/sh
# Run PCS base and example clients tests
#=============================================================================

#set -x

run_tests()
{
_ret=0

echo
echo "=============== Running tests in $PWD"
echo

phpize && ./configure --quiet && make -k clean all CFLAGS='-g -Wall'

# Remove this when Bob Weinand has fixed the valgrind options in run-tests.php
sed 's/--vex-iropt-register-updates=allregs-at-mem-access/--vex-iropt-precise-memory-exns=yes/g' <run-tests.php >r
rm -f run-tests.php
cp r run-tests.php

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

export NO_INTERACTION=1

# Xdebug versions < 2.3.3 causes PHP to crash when enabled with PCS ext (see
# http://bugs.xdebug.org/view.php?id=1151).
# **TODO: To be confirmed on PCS
# Note that xdebug is not pre-installed on every PHP version (so, phpenv may fail)

phpenv config-rm xdebug.ini || :

valgrind --version

#-- First, run base dir tests

run_tests
[ $? = 0 ] || ret=$?

#-- Then, test the client extensions

make install

cd examples/pcs_test

run_tests
[ $? = 0 ] || ret=$?

cd ../example1

run_tests
[ $? = 0 ] || ret=$?

#----

exit $ret
