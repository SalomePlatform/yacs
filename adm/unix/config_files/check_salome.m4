dnl  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
dnl
dnl  This library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public
dnl  License as published by the Free Software Foundation; either
dnl  version 2.1 of the License.
dnl
dnl  This library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public
dnl  License along with this library; if not, write to the Free Software
dnl  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
dnl
dnl  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
dnl
# Check availability of Salome's KERNEL binary distribution
#
# Author : Jerome Roy (CEA, 2003)
#

AC_DEFUN([AC_CHECK_KERNEL],[

AC_CHECKING(for Kernel)

Kernel_ok=no
dsc_ok=no

AC_ARG_WITH(kernel,
	    [  --with-kernel=DIR               root directory path of KERNEL build or installation],
	    KERNEL_DIR="$withval",KERNEL_DIR="")

if test "x$KERNEL_DIR" = "x" ; then

# no --with-kernel-dir option used

   if test "x$KERNEL_ROOT_DIR" != "x" ; then

    # KERNEL_ROOT_DIR environment variable defined
      KERNEL_DIR=$KERNEL_ROOT_DIR

   else

    # search Kernel binaries in PATH variable
      AC_PATH_PROG(TEMP, runSalome)
      if test "x$TEMP" != "x" ; then
         KERNEL_BIN_DIR=`dirname $TEMP`
         KERNEL_DIR=`dirname $KERNEL_BIN_DIR`
      fi
      
   fi
# 
fi


if test -f ${KERNEL_DIR}/bin/salome/runSalome ; then
   Kernel_ok=yes
   AC_MSG_RESULT(Using Kernel module distribution in ${KERNEL_DIR})
   AC_DEFINE(SALOME_KERNEL,,[With Salome KERNEL])

   if test "x$KERNEL_ROOT_DIR" = "x" ; then
      KERNEL_ROOT_DIR=${KERNEL_DIR}
   fi
   if test "x$KERNEL_SITE_DIR" = "x" ; then
      KERNEL_SITE_DIR=${KERNEL_ROOT_DIR}
   fi
   SALOME_ROOT_DIR=`AS_DIRNAME(["$KERNEL_ROOT_DIR"])`
   SALOME_VERSION="`echo $KERNEL_ROOT_DIR | sed s@$SALOME_ROOT_DIR/KERNEL@@`"

   if test -f ${KERNEL_DIR}/idl/salome/DSC_Engines.idl ; then
      # DSC extension available
      dsc_ok=yes
      AC_MSG_RESULT(Using DSC Ports module distribution in ${KERNEL_DIR})
      AC_DEFINE(DSC_PORTS,,[With DSC ports])
   else
      AC_MSG_WARN("Cannot find DSC Ports module distribution")
   fi

   AC_SUBST(KERNEL_ROOT_DIR)
   AC_SUBST(KERNEL_SITE_DIR)
   AC_SUBST(SALOME_ROOT_DIR)
   AC_SUBST(SALOME_VERSION)

else
   AC_MSG_WARN("Cannot find compiled Kernel module distribution")
fi

AC_MSG_RESULT(for Kernel: $Kernel_ok)
 
])dnl
 
