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
      AC_PATH_PROG(TEMP, runSalome.py)
      if test "x$TEMP" != "x" ; then
         KERNEL_BIN_DIR=`dirname $TEMP`
         KERNEL_DIR=`dirname $KERNEL_BIN_DIR`
      fi

   fi
#
fi


if test -f ${KERNEL_DIR}/bin/salome/runSalome.py ; then
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

   KERNEL_LDFLAGS=-L${KERNEL_ROOT_DIR}/lib/salome
   KERNEL_CXXFLAGS=-I${KERNEL_ROOT_DIR}/include/salome

   AC_SUBST(KERNEL_ROOT_DIR)
   AC_SUBST(KERNEL_SITE_DIR)
   AC_SUBST(SALOME_ROOT_DIR)
   AC_SUBST(SALOME_VERSION)
   AC_SUBST(KERNEL_LDFLAGS)
   AC_SUBST(KERNEL_CXXFLAGS)

else
   AC_MSG_WARN("Cannot find compiled Kernel module distribution")
fi

AC_MSG_RESULT(for Kernel: $Kernel_ok)

])dnl

# -----------------------------------------------------------------------------
# --- from GUI_SRC 3.2.3
#  Check availability of Salome binary distribution
#
#  Author : Marc Tajchman (CEA, 2002)
#------------------------------------------------------------

AC_DEFUN([CHECK_GUI],[
AC_REQUIRE([AC_LINKER_OPTIONS])dnl

AC_CHECKING(for $2 $1 )

SalomeGUI_ok=no
HAS_GUI=0

GUI_LDFLAGS=""
GUI_CXXFLAGS=""

AC_ARG_WITH(gui,
            [  --with-gui=DIR          root directory path of SALOME GUI installation],
            [SALOME_GUI_DIR="$withval"],[SALOME_GUI_DIR=""])

if test "x${SALOME_GUI_DIR}" = "x" ; then
  # no --with-gui-dir option used
  AC_MSG_RESULT(try \${GUI_ROOT_DIR}: ${GUI_ROOT_DIR})
  if test "x${GUI_ROOT_DIR}" != "x" ; then
    # SALOME_ROOT_DIR environment variable defined
    SALOME_GUI_DIR=${GUI_ROOT_DIR}
  else
    # search Salome binaries in PATH variable
    AC_PATH_PROG(TEMP, $1)
    if test "x${TEMP}" != "x" ; then
      AC_MSG_RESULT($1 was found at : ${TEMP})
      SALOME_BIN_DIR=`dirname ${TEMP}`
      SALOME_GUI_DIR=`cd ${SALOME_BIN_DIR}/../..; pwd`
    fi
  fi
fi

if test -f ${SALOME_GUI_DIR}/bin/salome/$1 ; then
  SalomeGUI_ok=yes
  HAS_GUI=1
  AC_MSG_RESULT(Using SALOME GUI distribution in ${SALOME_GUI_DIR})

  if test "x${GUI_ROOT_DIR}" == "x" ; then
    GUI_ROOT_DIR=${SALOME_GUI_DIR}
  fi

  AC_SUBST(GUI_ROOT_DIR)

  GUI_LDFLAGS=-L${SALOME_GUI_DIR}/lib/salome
  GUI_CXXFLAGS=-I${SALOME_GUI_DIR}/include/salome

  AC_SUBST(GUI_LDFLAGS)
  AC_SUBST(GUI_CXXFLAGS)
else
  AC_MSG_WARN("Cannot find compiled SALOME GUI distribution")
fi

AC_MSG_RESULT(for $2: ${SalomeGUI_ok})

# Propagate test into atlocal
AC_SUBST(HAS_GUI)

# Propagate test into Makefiles...
AM_CONDITIONAL(HAS_GUI, test $HAS_GUI = 1)

# ... and into source files
AC_DEFINE_UNQUOTED(WITH_GUI, $HAS_GUI, [SALOME GUI])

])dnl

AC_DEFUN([CHECK_SALOME_GUI],[
  CHECK_GUI([SUITApp],
            [SALOME GUI])
])dnl

