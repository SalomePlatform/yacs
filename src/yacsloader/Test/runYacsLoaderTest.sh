#!/bin/bash
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

# script used by "salome test" command
./echoSrv &
pidecho=$!

BASEDIR=`pwd`
TESTDIR=$(mktemp -d --suffix=.yacstest)
export TESTCOMPONENT_ROOT_DIR=${TESTDIR}

mkdir -p ${TESTDIR}/lib/salome
LIBTEST=$BASEDIR/../runtime/lib/salome/libTestComponentLocal.so
cp $LIBTEST ${TESTDIR}/lib/salome
LIBDIR=$BASEDIR/../lib

cp xmlrun.sh $TESTDIR
cp *.py $TESTDIR
ln -s $BASEDIR/samples $TESTDIR/samples
cd $TESTDIR
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$LIBDIR $BASEDIR/TestYacsLoader
ret=$?
cd $BASEDIR
echo "exec status TestYacsLoader " $ret

kill -9 $pidecho

exit $ret
