#!/bin/sh
# Run PCS tests
#
# This script can be run :
#	- Manually, from the PCS source dir
#	- By Travis-ci
#	- By Jenkins
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
_ret=0

echo
echo "==== Running tests in $PWD"
echo

echo "--- Configuring ---"
phpize || return 1
./configure --quiet || return 1
echo "--- Rebuilding phpc files ---"
make phpc || return 1
echo "--- Compiling ---"
make -k clean all CFLAGS='-g -Wall -Werror' || return 1

dlname=`cat modules/*.la | grep dlname= | sed 's/^.*=//' | tr -d "'"`
make_opts="-d extension=$dlname"
[ "X$dlname" = 'Xpcs.so' ] || make_opts="-d extension=pcs $make_opts"
if [ ! -f modules/pcs.so ] ; then
  for i in ../modules ../pecl-pcs/modules ; do
    if [ -f $i/pcs.so ] ; then
      ln -s ../$i/pcs.so modules
      break
    fi
  done
fi

# -m : test using valgrind
# -q : No interaction

TEST_PHP_ARGS='-q'
if [ "$USE_VALGRIND" = true ] ; then
	TEST_PHP_ARGS="-m $TEST_PHP_ARGS"

	# Fix incompatibility between some versions of run-test.php and valgrind 3.7
	if [ "`valgrind --version`" = 'valgrind-3.7.0' ] ; then
		sed 's/--vex-iropt-register-updates=allregs-at-mem-access/--vex-iropt-precise-memory-exns=yes/g' <run-tests.php >r
		rm -f run-tests.php
		cp r run-tests.php
	fi
fi
export TEST_PHP_ARGS

echo "--- Running tests ---"
REPORT_EXIT_STATUS=1 make test PHP_TEST_SHARED_EXTENSIONS="$make_opts" || _ret=1

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

#---------------------------------------------------------------------------

jk_sync_tree()
{
# $1 = subdir

rsync -a --delete --exclude .git "$SOURCE_BASE/$1/" .

[ -f Makefile ] && make clean
[ -f configure ] && phpize --clean
}

#---------------------------------------------------------------------------
#-- Run tests from another directory
# On travis, clone tree

run_subdir_tests()
{
subdir="$1"
wsdir="$WS_BASE/$subdir"

[ -d $wsdir ] || mkdir -p $wsdir
cd $wsdir

case $RUNTYPE in
	jenkins)
		jk_sync_tree $subdir
		;;
	travis)
		echo "---- Cloning $subdir"
		git clone --depth 20 https://github.com/flaupretre/$subdir .
		;;
esac

run_tests
}

#---------------------------------------------------------------------------

RUNTYPE=local
[ -n "$TRAVIS" ] && RUNTYPE=travis
[ -n "$JENKINS_HOME" ] && RUNTYPE=jenkins

# When vagrind issues are fixed, uncomment the line below.
# Valgrind (memcheck) must be on by default.
#[ -z "$USE_VALGRIND" ] && USE_VALGRIND=true

WS_BASE=`pwd`
[ $RUNTYPE = local ] && WS_BASE="`dirname $WS_BASE`"

ret=0

export RUNTYPE USE_VALGRIND WS_BASE

# Environment-specific initialization

case $RUNTYPE in
	jenkins)
		# Choose PHP install to use
		phpbin=$JK_PHP_BASE$JK_PHP_VERSION/bin
		if [ ! -x $phpbin/phpize ] ; then
			echo "**ERROR: Cannot find phpize script in '$phpbin'"
			exit 1
		fi
		PATH=$phpbin:$PATH
		export PATH

		# Check workspace dir (security)
		echo "$WS_BASE" | grep /var/lib/jenkins/workspace >/dev/null 2>&1
		if [ $? != 0 ] ; then
			echo "**ERROR: Jenkins workspace error"
			exit 1
		fi

		[ -z "$SOURCE_BASE" ] && SOURCE_BASE=/proj/pcs/public
		export SOURCE_BASE
		;;

	travis)
		# Xdebug versions < 2.3.3 causes PHP to crash when enabled with PCS ext (see
		# http://bugs.xdebug.org/view.php?id=1151).
		# **TODO: To be confirmed on PCS
		# Note that xdebug is not pre-installed on every PHP version (so, phpenv may fail)

		phpenv config-rm xdebug.ini || :
		;;
esac

#===========================================================================
#-- Run base tests

run_subdir_tests pecl-pcs || ret=$?

#echo "--- Installing the PCS extension ---"
#make install

#-- Run tests from pcs-example and pcs-test

run_subdir_tests pcs-example || ret=$?

run_subdir_tests pcs-test || ret=$?

#----

if [ "$ret" = 0 ] ;then
	echo "======== SUCCESS ============="
else
	echo "************ One or more errors were encoutered **************"
fi

exit $ret
