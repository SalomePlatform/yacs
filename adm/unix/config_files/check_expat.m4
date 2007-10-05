
AC_DEFUN([AC_CHECK_EXPAT],[

AC_CHECKING(for expat)

AC_CHECK_HEADER(expat.h,expat_ok="yes",expat_ok="no")

if  test "x$expat_ok" = "xyes"
then
  AC_CHECK_LIB(expat,XML_ExpatVersionInfo, LIBS="-lexpat $LIBS",,)
fi

AC_MSG_RESULT(for expat: $expat_ok)

EXPAT_LIBS=$LIBS
AC_SUBST(EXPAT_LIBS)

])dnl
dnl

