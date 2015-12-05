#!/bin/sh
# Run PCS tests
#
# Tests we run here include :
#		- the PCS base tests,
#		- those from pcs-example if available on the local host,
#		- and those from the pcs-test extension (download if not present).
#=============================================================================

#set -x
set -e	# Exit on non-zero status

#-----

run_tests()
{
dir="$1"

_ret=0

echo
echo "=============== Testing <base>/$dir"
echo

cd $BASE/$dir
echo "--- Configuring ---"
phpize || return 1
./configure --quiet || return 1
echo "--- Rebuilding phpc files ---"
make phpc || return 1
echo "--- Compiling/installing ---"
make -k clean all CFLAGS='-g -Wall -Werror' || return 1

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

echo "--- Running tests ---"
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

cd $BASE

return $_ret
}

#---------------------------------------------------------------------------

export USE_VALGRIND

BASE=`pwd`
export BASE

if [ ! -f $BASE/php_pcs.c ] ; then
	echo "**ERROR: This script must be run from the PCS extension base directory"
	exit 1
fi

ret=0

# Xdebug versions < 2.3.3 causes PHP to crash when enabled with PCS ext (see
# http://bugs.xdebug.org/view.php?id=1151).
# **TODO: To be confirmed on PCS
# Note that xdebug is not pre-installed on every PHP version (so, phpenv may fail)

if [ -n "$TRAVIS" ] ; then
	phpenv config-rm xdebug.ini || :
fi

#===========================================================================
#-- Run base tests

run_tests . || ret=$?

echo "--- Installing the PCS extension ---"
make install

#-- Run pcs-example tests, just if they are present locally

rdir=../pcs-example
if [ -d $BASE/$rdir ] ;then
	run_tests $rdir || ret=$?
else
	echo "----- Ignoring pcs-example tests ------"
fi

#-- Run tests from pcs-test
# If source tree is not present, download it

rdir=../pcs-test
if [ ! -d $BASE/$rdir ] ; then
	echo "---- Downloading pcs-test"
	git clone --depth 20 https://github.com/flaupretre/pcs-test $BASE/$rdir
	cd $BASE/$rdir
	git status
fi

run_tests $rdir || ret=$?

#----

if [ "$ret" = 0 ] ;then
	echo "======== SUCCESS ============="
else
	echo "************ One or more errors were encoutered **************"
fi

exit $ret
