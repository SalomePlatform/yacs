#!/bin/sh
# Copyright (C) 2006-2020  CEA/DEN, EDF R&D
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
OMNIORB_CONFIG=${TESTDIR}/omniorb.cfg
OMNINAMES_LOGDIR=${TESTDIR}/omnilog

export TESTDIR
export OMNIORB_CONFIG
export OMNINAMES_LOGDIR

echo ${TESTDIR}
echo ${OMNIORB_CONFIG}

# do not use the default port 2810 for omninames (to improve, cf. SALOME)
echo "InitRef = NameService=corbaname::localhost:2910" > ${OMNIORB_CONFIG}

rm -rf ${OMNINAMES_LOGDIR}
mkdir  ${OMNINAMES_LOGDIR}

echo $$

omniNames -start 2910 &
pidomni=$!
echo $pidomni

#wait enough time to let omniNames start
sleep 2

./runtimeTestEchoSrv &
pidecho=$!
echo $pidecho

# this find should work both for make test and for salome test
mkdir -p ${TESTDIR}/lib/salome
LIBTEST=`find .. -name libTestComponentLocal.so | head -n 1`
cp $LIBTEST ${TESTDIR}/lib/salome

export TESTCOMPONENT_ROOT_DIR=${TESTDIR}

#wait enough time to let runtimeTestEchoSrv start and register
sleep 2

cp xmlrun.sh ${TESTDIR}
cp TestRuntime ${TESTDIR}
cd ${TESTDIR}
./TestRuntime
ret=$?
cd -

kill -9 $pidecho $pidomni
cat /tmp/${USER}/UnitTestsResult
exit $ret
