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

# Check availability of Qt's msg2qm tool binary distribution
#
# Author : Jerome Roy (CEA, 2003)
#

AC_DEFUN([CHECK_MSG2QM],[

AC_CHECKING(for msg2qm)

msg2qm_ok=no

AC_ARG_WITH(msg2qm,
	    [  --with-msg2qm=DIR               root directory path of MSG2QM installation],
	    MSG2QM_DIR="$withval",MSG2QM_DIR="")

if test "x$MSG2QM_DIR" == "x" ; then

# no --with-MSG2QM-dir option used

   if test "x$MSG2QM_ROOT" != "x" ; then

    # MSG2QM_ROOT environment variable defined
      MSG2QM_DIR=$MSG2QM_ROOT

   else

    # search MSG2QM binaries in PATH variable
      AC_PATH_PROG(TEMP, msg2qm)
      if test "x$TEMP" != "x" ; then
         MSG2QM_DIR=`dirname $TEMP`
      fi
      
   fi
# 
fi

# look for msg2qm in ${MSG2QM_DIR} directory
if test -f ${MSG2QM_DIR}/msg2qm ; then
   msg2qm_ok=yes
   MSG2QM="${MSG2QM_DIR}/msg2qm"
   AC_MSG_RESULT(Using MSG2QM executable in ${MSG2QM_DIR})
else
   # if not found, look for msg2qm in ${MSG2QM_DIR}/bin directory
   if test -f ${MSG2QM_DIR}/bin/msg2qm ; then
      msg2qm_ok=yes
      MSG2QM="${MSG2QM_DIR}/bin/msg2qm"
      AC_MSG_RESULT(Using MSG2QM executable in ${MSG2QM_DIR}/bin)
   else	    
      AC_MSG_WARN("Cannot find MSG2QM executable")
   fi
fi

AC_SUBST(MSG2QM)
AC_MSG_RESULT(for MSG2QM: $msg2qm_ok)
 
])dnl
 
