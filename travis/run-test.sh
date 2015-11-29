#!/bin/sh
# Run PCS base and example clients tests
#=============================================================================

#set -x

run_tests()
{
dir="$1"

_ret=0

echo
echo "=============== Running tests in <base>/$dir"
echo

cd $BASE/$dir
phpize || return 1
./configure --quiet || return 1
make phpc || return 1
make -k clean install CFLAGS='-g -Wall -Werror' || return 1

# -m : test using valgrind
# -q : No interaction

TEST_PHP_ARGS='-q'
if [ -n "$USE_VALGRIND" ] ; then
	TEST_PHP_ARGS="-m $TEST_PHP_ARGS"

	# Remove this when the valgrind options in run-tests.php are fixed
	if [ "`valgrind --version`" = 'valgrind-3.7.0' ] ; then
		sed 's/--vex-iropt-register-updates=allregs-at-mem-access/--vex-iropt-precise-memory-exns=yes/g' <run-tests.php >r
		rm -f run-tests.php
		cp r run-tests.php
	fi
fi
export TEST_PHP_ARGS

make test

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

export USE_VALGRIND

BASE=`pwd`
export BASE

ret=0

# Xdebug versions < 2.3.3 causes PHP to crash when enabled with PCS ext (see
# http://bugs.xdebug.org/view.php?id=1151).
# **TODO: To be confirmed on PCS
# Note that xdebug is not pre-installed on every PHP version (so, phpenv may fail)

if [ -n "$TRAVIS" ] ; then
	phpenv config-rm xdebug.ini || :
fi

#-- First, run base tests, then examples

for d in . examples/example1 examples/pcs_test
do
	run_tests $d
	[ $? = 0 ] || ret=$?
done

#----

if [ "$ret" = 0 ] ;then
	echo "======== SUCCESS ============="
else
	echo "************ One or more errors were encoutered **************"
fi

exit $ret
