#  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
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
#  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
#
#------------------------------------------------------------
#  Check availability of Salome binary distribution
#
#  Author : Marc Tajchman (CEA, 2002)
#------------------------------------------------------------

AC_DEFUN([CHECK_SALOME_GUI],[

AC_CHECKING(for SalomeGUI)

SalomeGUI_ok=yes

AC_ARG_WITH(gui,
	    --with-salome_gui=DIR root directory path of SALOME GUI installation,
	    SALOME_GUI_DIR="$withval",SALOME_GUI_DIR="")

if test "x$SALOME_GUI_DIR" = "x" ; then
  if test "x$GUI_ROOT_DIR" != "x" ; then
    SALOME_GUI_DIR=$GUI_ROOT_DIR
  else
    # search Salome binaries in PATH variable
    AC_PATH_PROG(TEMP, libSalomeApp.so)
    if test "x$TEMP" != "x" ; then
      SALOME_GUI_DIR=`dirname $TEMP`
    fi
  fi
fi

if test -f ${SALOME_GUI_DIR}/lib/salome/libSalomeApp.so  ; then
  SalomeGUI_ok=yes
  AC_MSG_RESULT(Using SALOME GUI distribution in ${SALOME_GUI_DIR})
  GUI_ROOT_DIR=${SALOME_GUI_DIR}
  AC_SUBST(GUI_ROOT_DIR)
else
  AC_MSG_WARN("Cannot find compiled SALOME GUI distribution")
fi
  
AC_MSG_RESULT(for SALOME GUI: $SalomeGUI_ok)
 
])dnl
 
