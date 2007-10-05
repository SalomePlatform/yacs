#! /bin/bash

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


