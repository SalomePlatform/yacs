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

# Check if component environment is either defined or not
#
# Author : Jean-Yves PRADILLON (OPEN CASCADE, 2005)
#

AC_DEFUN([CHECK_COMPONENT_ENV],[

AC_CHECKING(for Component Environment)

Comp_Env_ok=no

if test -d "$HXX2SALOME_GENERIC_CLASS_NAMECPP_ROOT_DIR" ; then
   Comp_Env_ok=yes
   AC_MSG_RESULT(Using Component Root Dir ${HXX2SALOME_GENERIC_CLASS_NAMECPP_ROOT_DIR})
else
   AC_MSG_WARN(Cannot find Component Root Dir "${HXX2SALOME_GENERIC_CLASS_NAMECPP_ROOT_DIR}")
   if test "x$HXX2SALOME_GENERIC_CLASS_NAMECPP_ROOT_DIR" = "x" ; then
      AC_MSG_WARN(Did you source the environment file?)
   fi
fi
  
AC_MSG_RESULT(for Component Environment: $Comp_Env_ok)
 
])dnl
 
