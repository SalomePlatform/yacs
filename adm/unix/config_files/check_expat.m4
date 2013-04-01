dnl Copyright (C) 2006-2013  CEA/DEN, EDF R&D
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2.1 of the License.
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

AC_DEFUN([AC_CHECK_EXPAT],[

AC_CHECKING(for expat)

CXXFLAGS_old=$CXXFLAGS
CPPFLAGS_old=$CPPFLAGS

# Custom location of expat package can be specified
# through EXPAT_DIR variable
if test "x$EXPAT_DIR" != "x"
then
  if test "x$EXPAT_DIR" = "x/usr"
  then
    AC_MSG_NOTICE(Trying native Expat...)
    TMPLIBS="-lexpat $LIBS"
  else
    AC_MSG_NOTICE(Trying Expat from $EXPAT_DIR ...)
    TMPLIBS="-L$EXPAT_DIR/lib -lexpat $LIBS"
    CXXFLAGS="$CXXFLAGS -I$EXPAT_DIR/include"
    CPPFLAGS="$CPPFLAGS -I$EXPAT_DIR/include"
    EXPAT_INCLUDES="-I$EXPAT_DIR/include"
  fi
else
  AC_MSG_NOTICE(Trying native Expat...)
  TMPLIBS="-lexpat $LIBS"
fi

AC_CHECK_HEADER(expat.h,expat_ok="yes",expat_ok="no")

if  test "x$expat_ok" = "xyes"
then
  LIBS_old=$LIBS
  LIBS=$TMPLIBS
  AC_CHECK_LIB(expat,XML_ExpatVersionInfo,expat_ok="yes",expat_ok="no",)
  LIBS=$LIBS_old
fi

if test "x$expat_ok" = "xyes"
then
  EXPAT_LIBS=$TMPLIBS
fi

AC_MSG_RESULT(for expat: $expat_ok)

CXXFLAGS=$CXXFLAGS_old
CPPFLAGS=$CPPFLAGS_old

AC_SUBST(EXPAT_LIBS)
AC_SUBST(EXPAT_INCLUDES)

])dnl
dnl

