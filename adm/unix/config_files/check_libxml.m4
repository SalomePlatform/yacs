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

AC_DEFUN([AC_CHECK_LIBXML],[

AC_CHECKING(for libxml)

CPPFLAGS_old=$CPPFLAGS
CXXFLAGS_old=$CXXFLAGS

# Custom location of libxml2 package can be specified
# thorugh LIBXML_DIR variable
if test "x$LIBXML_DIR" != "x"
then
  CPPFLAGS="$CPPFLAGS -I$LIBXML_DIR/include/libxml2"
  CXXFLAGS="$CXXFLAGS -I$LIBXML_DIR/include/libxml2"
  if test "x$LIBXML_DIR" = "x/usr"
  then
    AC_MSG_NOTICE(Trying native Libxml2...)
    TMPLIBS="-lxml2 $LIBS"
  else
    AC_MSG_NOTICE(Trying Libxml2 from $LIBXML_DIR ...)
    TMPLIBS="-L$LIBXML_DIR/lib -lxml2 $LIBS"
  fi
  LIBXML_INCLUDES="-I$LIBXML_DIR/include/libxml2"
else
  CPPFLAGS="$CPPFLAGS -I/usr/include/libxml2"
  CXXFLAGS="$CXXFLAGS -I/usr/include/libxml2"
  TMPLIBS="-lxml2 $LIBS"
  LIBXML_INCLUDES="-I/usr/include/libxml2"
fi

AC_CHECK_HEADER(libxml/parser.h,libxml_ok="yes",libxml_ok="no")

if  test "x$libxml_ok" = "xyes"
then
  LIBS_old=$LIBS
  LIBS=$TMPLIBS
  AC_CHECK_LIB(xml2,xmlInitParser,libxml_ok="yes",libxml_ok="no",)
  LIBS=$LIBS_old
fi

if test "x$libxml_ok" = "xyes"
then
  LIBXML_LIBS=$TMPLIBS
fi

AC_MSG_RESULT(for libxml: $libxml_ok)

CXXFLAGS=$CXXFLAGS_old
CPPFLAGS=$CPPFLAGS_old

AC_SUBST(LIBXML_LIBS)
AC_SUBST(LIBXML_INCLUDES)

])dnl
dnl

