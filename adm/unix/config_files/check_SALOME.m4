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
# -----------------------------------------------------------------------------
# --- from KERNEL_SRC 3.2.3
# Check availability of Salome's KERNEL binary distribution
#
# Author : Jerome Roy (CEA, 2003)
#

AC_DEFUN([CHECK_KERNEL],[
AC_REQUIRE([AC_LINKER_OPTIONS])dnl

AC_CHECKING(for Kernel)

Kernel_ok=no

KERNEL_LDFLAGS=""
KERNEL_CXXFLAGS=""

AC_ARG_WITH(kernel,
	    [--with-kernel=DIR  root directory path of KERNEL build or installation],
	    [KERNEL_DIR="$withval"],
	    [KERNEL_DIR=""])

if test "x${KERNEL_DIR}" = "x" ; then
  AC_MSG_RESULT(for \${KERNEL_ROOT_DIR}: ${KERNEL_ROOT_DIR})
  # no --with-kernel-dir option used
  if test "x${KERNEL_ROOT_DIR}" != "x" ; then
    # KERNEL_ROOT_DIR environment variable defined
    KERNEL_DIR=${KERNEL_ROOT_DIR}
  else
    # search Kernel binaries in PATH variable
    AC_PATH_PROG(TEMP,runSalome)
    if test "x${TEMP}" != "x" ; then
      AC_MSG_RESULT(runSalome was found at : ${TEMP})
      KERNEL_BIN_DIR=`dirname ${TEMP}`
      KERNEL_DIR=`cd ${KERNEL_BIN_DIR}/../..; pwd`
    fi
  fi
fi

if test -f ${KERNEL_DIR}/bin/salome/runSalome ; then
   AC_MSG_RESULT(Using Kernel module distribution in ${KERNEL_DIR})
   Kernel_ok=yes

   if test "x${KERNEL_ROOT_DIR}" = "x" ; then
      KERNEL_ROOT_DIR=${KERNEL_DIR}
   fi

   if test "x${KERNEL_SITE_DIR}" = "x" ; then
      KERNEL_SITE_DIR=${KERNEL_ROOT_DIR}
   fi

   AC_SUBST(KERNEL_ROOT_DIR)
   AC_SUBST(KERNEL_SITE_DIR)

   KERNEL_LDFLAGS=-L${KERNEL_DIR}/lib${LIB_LOCATION_SUFFIX}/salome
   KERNEL_CXXFLAGS=-I${KERNEL_DIR}/include/salome

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
  AC_MSG_RESULT(Using SALOME GUI distribution in ${SALOME_GUI_DIR})

  if test "x${GUI_ROOT_DIR}" == "x" ; then
    GUI_ROOT_DIR=${SALOME_GUI_DIR}
  fi

  AC_SUBST(GUI_ROOT_DIR)

  GUI_LDFLAGS=-L${SALOME_GUI_DIR}/lib${LIB_LOCATION_SUFFIX}/salome
  GUI_CXXFLAGS=-I${SALOME_GUI_DIR}/include/salome

  AC_SUBST(GUI_LDFLAGS)
  AC_SUBST(GUI_CXXFLAGS)
else
  AC_MSG_WARN("Cannot find compiled SALOME GUI distribution")
fi
  
AC_MSG_RESULT(for $2: ${SalomeGUI_ok})
 
])dnl

AC_DEFUN([CHECK_SALOME_GUI],[
  CHECK_GUI([SUITApp],
            [SALOME GUI])
])dnl

# -----------------------------------------------------------------------------
# --- from PYHELLO1_SRC 3.2.3
#  Check availability of PYHELLO binary distribution
#
#  Author : Marc Tajchman (CEA, 2002)
#------------------------------------------------------------

AC_DEFUN([CHECK_PYHELLO],[

AC_CHECKING(for PyHello)

PyHello_ok=no

AC_ARG_WITH(pyHello,
	    --with-py-hello=DIR root directory path of PYHELLO installation,
	    PYHELLO_DIR="$withval",PYHELLO_DIR="")

if test "x$PYHELLO_DIR" = "x" ; then

# no --with-py-hello option used

  if test "x$PYHELLO_ROOT_DIR" != "x" ; then

    # PYHELLO_ROOT_DIR environment variable defined
    PYHELLO_DIR=$PYHELLO_ROOT_DIR

  else

    # search PyHello binaries in PATH variable
    AC_PATH_PROG(TEMP, PYHELLOGUI.py)
    if test "x$TEMP" != "x" ; then
      PYHELLO_BIN_DIR=`dirname $TEMP`
      PYHELLO_DIR=`dirname $PYHELLO_BIN_DIR`
    fi

  fi
#
fi

if test -f ${PYHELLO_DIR}/bin/salome/PYHELLOGUI.py  ; then
  PyHello_ok=yes
  AC_MSG_RESULT(Using PYHELLO distribution in ${PYHELLO_DIR})

  if test "x$PYHELLO_ROOT_DIR" == "x" ; then
    PYHELLO_ROOT_DIR=${PYHELLO_DIR}
  fi
  AC_SUBST(PYHELLO_ROOT_DIR)
else
  AC_MSG_WARN("Cannot find compiled PYHELLO distribution")
fi
  
AC_MSG_RESULT(for PYHELLO: $PyHello_ok)
 
])dnl
 
# -----------------------------------------------------------------------------
# --- from GEOM_SRC 3.2.3
# Check availability of Geom binary distribution
#
# Author : Nicolas REJNERI (OPEN CASCADE, 2003)
#

AC_DEFUN([CHECK_GEOM],[

AC_CHECKING(for Geom)

Geom_ok=no

GEOM_LDFLAGS=""
GEOM_CXXFLAGS=""

AC_ARG_WITH(geom,
	    [  --with-geom=DIR root directory path of GEOM installation ],
	    GEOM_DIR="$withval",GEOM_DIR="")

if test "x$GEOM_DIR" == "x" ; then

# no --with-geom-dir option used

   if test "x$GEOM_ROOT_DIR" != "x" ; then

    # GEOM_ROOT_DIR environment variable defined
      GEOM_DIR=$GEOM_ROOT_DIR

   else

    # search Geom binaries in PATH variable
      AC_PATH_PROG(TEMP, libGEOM_Swig.py)
      if test "x$TEMP" != "x" ; then
         GEOM_BIN_DIR=`dirname $TEMP`
         GEOM_DIR=`dirname $GEOM_BIN_DIR`
      fi
      
   fi
# 
fi

if test -f ${GEOM_DIR}/bin/salome/libGEOM_Swig.py ; then
   Geom_ok=yes
   AC_MSG_RESULT(Using Geom module distribution in ${GEOM_DIR})

   if test "x$GEOM_ROOT_DIR" == "x" ; then
      GEOM_ROOT_DIR=${GEOM_DIR}
   fi
   AC_SUBST(GEOM_ROOT_DIR)

   GEOM_LDFLAGS=-L${GEOM_DIR}/lib${LIB_LOCATION_SUFFIX}/salome
   GEOM_CXXFLAGS=-I${GEOM_DIR}/include/salome

   AC_SUBST(GEOM_LDFLAGS)
   AC_SUBST(GEOM_CXXFLAGS)

else
   AC_MSG_WARN("Cannot find compiled Geom module distribution")
fi

AC_MSG_RESULT(for Geom: $Geom_ok)
 
])dnl
 
# -----------------------------------------------------------------------------
# --- from VISU_SRC 3.2.3
###########################################################
# File    : check_Visu.m4
# Author  : Vadim SANDLER (OCN)
# Created : 13/07/05
# Check availability of VISU binary distribution
###########################################################

AC_DEFUN([CHECK_VISU],[

AC_CHECKING(for VISU)

Visu_ok=no

VISU_LDFLAGS=""
VISU_CXXFLAGS=""


AC_ARG_WITH(visu,
	    [  --with-visu=DIR root directory path of VISU module installation ],
	    VISU_DIR="$withval",VISU_DIR="")

if test "x$VISU_DIR" == "x" ; then
   # no --with-visu-dir option used
   if test "x$VISU_ROOT_DIR" != "x" ; then
      # VISU_ROOT_DIR environment variable defined
      VISU_DIR=$VISU_ROOT_DIR
   fi
fi

if test -f ${VISU_DIR}/idl/salome/VISU_Gen.idl ; then
   Visu_ok=yes
   AC_MSG_RESULT(Using VISU module distribution in ${VISU_DIR})

   if test "x$VISU_ROOT_DIR" == "x" ; then
      VISU_ROOT_DIR=${VISU_DIR}
   fi
   AC_SUBST(VISU_ROOT_DIR)

   VISU_LDFLAGS=-L${VISU_DIR}/lib${LIB_LOCATION_SUFFIX}/salome
   VISU_CXXFLAGS=-I${VISU_DIR}/include/salome

   AC_SUBST(VISU_LDFLAGS)
   AC_SUBST(VISU_CXXFLAGS)

else
   AC_MSG_WARN("Cannot find VISU module sources")
fi
  
AC_MSG_RESULT(for VISU: $Visu_ok)
 
])dnl
 
# -----------------------------------------------------------------------------

AC_DEFUN([CHECK_SALOME_ENV],[

AC_CHECKING(for SALOME_ENV)

Salome_Env_ok=no

AC_ARG_WITH(salome_env,
	    [  --with-salome_env= path of script that sets salome prerequisites  ],
	    SALOME_ENV_PATH="$withval",SALOME_ENV_PATH="")

if test "x$SALOME_ENV_PATH" == "x" ; then
   # no --with-salome_env option used
   if test "x$PREREQUISITE_SH" != "x" ; then
      # PREREQUISITE_SH environment variable defined
      SALOME_ENV_PATH=$PREREQUISITE_SH
   fi
fi

if test "x$SALOME_ENV_PATH" != "x" ; then
   if test -f ${SALOME_ENV_PATH} ; then
      Salome_Env_ok=yes
      AC_MSG_RESULT(Using script ${SALOME_ENV_PATH} for salome prerequisites in tests)

      if test "x$PREREQUISITE_SH" == "x" ; then
         PREREQUISITE_SH=${SALOME_ENV_PATH}
      fi
      AC_SUBST(PREREQUISITE_SH)
   fi
fi

if test -f ${SALOME_ENV_PATH} ; then
   AC_MSG_RESULT(for SALOME_ENV: $Salome_Env_ok)   
else
   AC_MSG_WARN("Cannot find SALOME_ENV module sources")
   AC_MSG_RESULT(for SALOME_ENV: $Salome_Env_ok)   
fi
 
])dnl

