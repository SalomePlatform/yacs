#!/bin/sh
#  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
killall -9 omniNames echoSrv

BASEREP=`pwd`
OMNIORB_CONFIG=${BASEREP}/omniorb.cfg
OMNINAMES_LOGDIR=${BASEREP}/omnilog

export BASEREP
export OMNIORB_CONFIG
export OMNINAMES_LOGDIR

echo ${BASEREP}
echo ${OMNIORB_CONFIG}

# do not use the default port 2810 for omninames (to improve, cf. SALOME)
echo "InitRef = NameService=corbaname::localhost:2910" > ${OMNIORB_CONFIG}

rm -rf ${OMNINAMES_LOGDIR}
mkdir  ${OMNINAMES_LOGDIR}

mkdir -p /tmp/${USER}
\rm -f /tmp/${USER}/UnitTestsResult

echo $$

omniNames -start 2910 &
pidomni=$!
echo $pidomni

./echoSrv &
pidecho=$!
echo $pidecho

export PYTHONPATH=${BASEREP}:${PYTHONPATH}
./.libs/TestStandAlone
ret=$?
# 
# kill -9 $pidecho $pidomni
cat /tmp/${USER}/UnitTestsResult
exit $ret
