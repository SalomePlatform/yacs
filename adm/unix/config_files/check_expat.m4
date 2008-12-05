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
AC_DEFUN([AC_CHECK_EXPAT],[

AC_CHECKING(for expat)

EXPAT_INCLUDES=
AC_CHECK_HEADER(expat.h,expat_ok="yes",expat_ok="no")

if  test "x$expat_ok" = "xyes"
then
  EXPAT_LIBS="-lexpat"
  LIBS_old=$LIBS
  AC_CHECK_LIB(expat,XML_ExpatVersionInfo, LIBS="-lexpat $LIBS",,)
  LIBS=$LIBS_old
fi

AC_MSG_RESULT(for expat: $expat_ok)

AC_SUBST(EXPAT_LIBS)
AC_SUBST(EXPAT_INCLUDES)

])dnl
dnl

