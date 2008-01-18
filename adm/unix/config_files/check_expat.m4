
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

