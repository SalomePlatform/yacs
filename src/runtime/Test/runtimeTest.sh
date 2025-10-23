#!/bin/sh
# Copyright (C) 2006-2025  CEA, EDF
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

BASEDIR=`pwd`
TESTDIR=$(mktemp -d  --suffix=.yacstest)

export TESTDIR

echo ${TESTDIR}

# this find should work both for make test and for salome test
mkdir -p ${TESTDIR}/lib/salome
#LIBTEST=`find .. -name libTestComponentLocal.so | head -n 1`
LIBTEST=$BASEDIR/../lib/libTestComponentLocal.so
cp $LIBTEST ${TESTDIR}/lib/salome

export TESTCOMPONENT_ROOT_DIR=${TESTDIR}

cp xmlrun.sh ${TESTDIR}
( cd ${TESTDIR} && ln -s $BASEDIR/TestRuntime )
( cd ${TESTDIR} && ln -s $BASEDIR/runtimeTestEchoSrv )
LIBDIR=$BASEDIR/../lib
cd ${TESTDIR}
# ---------------------------------------------------------------------
# Here we overwrite LD_LIBRARY_PATH to include only LIBDIR.
# All required shared libraries are loaded via the RUNPATH of the binaries
# Modifying LD_LIBRARY_PATH could interfere with Python/pip environments.
# See EDF33869
# ---------------------------------------------------------------------
LD_LIBRARY_PATH=$LIBDIR ./TestRuntime
ret=$?
echo $ret
cd -

cat ${TESTDIR}/UnitTestsResult
exit $ret
