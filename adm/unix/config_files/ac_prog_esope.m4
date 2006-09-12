# Check availability of Med binary distribution
#
# Author : Anthony GEAY (CEA, 2005)
#

AC_DEFUN([AC_PROG_ESOPE],[
CHECK_MED

AC_CHECKING(for Esope)

srcdirea=`( cd $srcdir && pwd )`

esope_ok=yes

AC_ARG_WITH(esope,
    [  --with-esope=EXEC esope executable ],
    [ESOPE="$withval"
      AC_MSG_RESULT("select $withval as esope executable")
    ], [
      AC_PATH_PROG(ESOPE, esopv10_0)
    ])
if test "x$ESOPE" = "x" ;
then
     ESOPE=$srcdirea/bin/esopv10_0
     AC_MSG_RESULT(use esope furnished in sources at location : $ESOPE)
fi
AC_SUBST(ESOPE)

dnl --- To improve UNIX 32
EFLAGS="ESOPE=2000000,NORME=TRADUCTEUR,FORT=UNIX32"
AC_SUBST(EFLAGS)

AC_PATH_PROG(CPPFORESOPE, cpp)
if test "x$CPPFORESOPE" = "x" ;
then
    esope_ok=no
    AC_MSG_RESULT(cpp not found in PATH !!! - used by esope to generate Fortran files)
fi
AC_SUBST(CPPFORESOPE)

CPPFORESOPEFLAGS="-Dunix32 -Dlinux ${MED_INCLUDES}"
AC_SUBST(CPPFORESOPEFLAGS)
dnl --- Segment directory
SEGMENTS_DIR="-I"$srcdirea/bin/SEGMENTS
AC_SUBST(SEGMENTS_DIR)

AC_MSG_RESULT(for Esope: $esope_ok)
 
])dnl
 
