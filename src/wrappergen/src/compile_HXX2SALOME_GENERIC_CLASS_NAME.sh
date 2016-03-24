#! /bin/bash
# Copyright (C) 2006-2016  CEA/DEN, EDF R&D
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

export BASE=PREFIX/tests
export COMP_NAME=HXX2SALOME_GENERIC_CLASS_NAME
export COMP_BASE=${BASE}/${COMP_NAME}

cd ${COMP_BASE}
export HXX2SALOME_ROOT_DIR=PREFIX/bin/HXX2SALOME_Test

if [ ! -d ${COMP_NAME}_SRC ] ; then
   ${HXX2SALOME_ROOT_DIR}/hxx2salome -q -q \
         ${BASE} \
         ${COMP_NAME}.hxx \
         lib${COMP_NAME}.so \
         ${BASE}
fi

cd ${COMP_BASE}
if [ ! -f ${COMP_NAME}_SRC/configure ] ; then 
   cd ${COMP_NAME}_SRC && ./build_configure
fi

cd ${COMP_BASE}
source ${COMP_NAME}_SRC/env_${COMP_NAME}.sh

if [ ! -f ${COMP_NAME}_BUILD/config.log ] ; then 
   cd ${COMP_NAME}_BUILD && \
   ../${COMP_NAME}_SRC/configure \
          --prefix=${COMP_BASE}/${COMP_NAME}_INSTALL 
fi

cd ${COMP_BASE}/${COMP_NAME}_BUILD
make && make install


