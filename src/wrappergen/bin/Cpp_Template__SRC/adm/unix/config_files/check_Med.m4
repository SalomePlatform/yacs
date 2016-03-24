dnl Copyright (C) 2006-2016  CEA/DEN, EDF R&D
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2.1 of the License, or (at your option) any later version.
dnl
dnl This library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl Lesser General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public
dnl License along with this library; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
dnl
dnl See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
dnl

# Check availability of Med binary distribution
#
# Author : Nicolas REJNERI (OPEN CASCADE, 2003)
#

AC_DEFUN([CHECK_MED],[

CHECK_HDF5
CHECK_MED2

AC_CHECKING(for Med)

Med_ok=no

AC_ARG_WITH(med,
	    [  --with-med=DIR root directory path of MED installation ],
	    MED_DIR="$withval",MED_DIR="")

if test "x$MED_DIR" == "x" ; then

# no --with-med-dir option used

   if test "x$MED_ROOT_DIR" != "x" ; then

    # MED_ROOT_DIR environment variable defined
      MED_DIR=$MED_ROOT_DIR

   else

    # search Med binaries in PATH variable
      AC_PATH_PROG(TEMP, libMEDMEM_Swig.py)
      if test "x$TEMP" != "x" ; then
         MED_BIN_DIR=`dirname $TEMP`
         MED_DIR=`dirname $MED_BIN_DIR`
      fi
      
   fi
# 
fi

if test -f ${MED_DIR}/bin/salome/libMEDMEM_Swig.py ; then
   Med_ok=yes
   AC_MSG_RESULT(Using Med module distribution in ${MED_DIR})

   if test "x$MED_ROOT_DIR" == "x" ; then
      MED_ROOT_DIR=${MED_DIR}
   fi
   AC_SUBST(MED_ROOT_DIR)
   MED_INCLUDES="-I${MED_ROOT_DIR}/include/salome ${MED2_INCLUDES} ${HDF5_INCLUDES} -I${KERNEL_ROOT_DIR}/include/salome"
   MED_LIBS="-L${MED_ROOT_DIR}/lib/salome -lmedmem ${MED2_LIBS} ${HDF5_LIBS} -L${KERNEL_ROOT_DIR}/lib/salome -lSALOMELocalTrace"
   AC_SUBST(MED_INCLUDES)
   AC_SUBST(MED_LIBS)

else
   AC_MSG_WARN("Cannot find Med module sources")
fi
  
AC_MSG_RESULT(for Med: $Med_ok)
 
])dnl
 
