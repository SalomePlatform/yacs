# Check presence of "CPPUNIT" product presence
#
# Author : Anthony GEAY (CEA, 2006)
#

AC_DEFUN([CHECK_CPPUNIT],[
AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_PROG_CPP])dnl

AC_CHECKING(for CPPUNIT)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
cppunit_ok=no
AC_ARG_WITH(cppunit,
    [AC_HELP_STRING([--with-cppunit=DIR],[cppunit directory path where cppunit is installed])],
    [CPPUNITINSTDIR="$withval"
      AC_MSG_RESULT("select $withval as path to cppunit")
    ])

AC_SUBST(CPPUNIT_INCLUDES)
AC_SUBST(CPPUNIT_LIBS)
AC_SUBST(CPPUNIT_LIBS_DIR)
AC_SUBST(CPPUNITINSTDIR)

CPPUNIT_INCLUDES=""
CPPUNIT_LIBS=""

LOCAL_INCLUDES=""
LOCAL_LIBS=""

if test "x$CPPUNITINSTDIR" = "x"  
then
   LOCAL_INCLUDES=""
   LOCAL_LIBS_DIR=""
else
   LOCAL_INCLUDES="-I$CPPUNITINSTDIR/include"
   LOCAL_LIBS_DIR="$CPPUNITINSTDIR/lib"
fi

dnl cppunit headers

CPPFLAGS_old="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $LOCAL_INCLUDES"
AC_LANG_CPLUSPLUS
AC_CHECK_HEADER(cppunit/ui/text/TestRunner.h,cppunit_ok=yes ,cppunit_ok=no)
CPPFLAGS="$CPPFLAGS_old"

if test "x$cppunit_ok" = "xyes"
then
  if test "x$LOCAL_INCLUDES" = "x"
  then
     CPPUNIT_INCLUDES=""
     CPPUNIT_LIBS="-lcppunit"
     CPPUNIT_LIBS_DIR=""
  else
     CPPUNIT_INCLUDES="$LOCAL_INCLUDES"
     CPPUNIT_LIBS="-L$LOCAL_LIBS_DIR -lcppunit"
     CPPUNIT_LIBS_DIR="$LOCAL_LIBS_DIR"
  fi
fi
AC_LANG_RESTORE
AC_MSG_RESULT(for cppunit: $cppunit_ok)

])dnl
