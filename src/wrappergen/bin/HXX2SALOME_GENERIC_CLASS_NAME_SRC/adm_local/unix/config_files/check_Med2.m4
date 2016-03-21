dnl Copyright (C) 2007-2016  CEA/DEN, EDF R&D
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

AC_DEFUN([CHECK_MED2],[
AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_PROG_CPP])dnl
AC_REQUIRE([CHECK_HDF5])dnl
AC_REQUIRE([MED_CHECK_SIZEOF_MEDINT])

AC_CHECKING(for MED2)

AC_ARG_WITH(med2,
    [  --with-med2=DIR                 root directory path to med2 installation ],
    [MED2HOME="$withval"
      AC_MSG_RESULT("select $withval as path to med2")
    ])

AC_SUBST(MED2_INCLUDES)
AC_SUBST(MED2_LIBS)
AC_SUBST(MED2_MT_LIBS)
AC_SUBST(MED_CPPFLAGS)

MED2_INCLUDES=""
MED2_LIBS=""
MED2_MT_LIBS=""
MED_CPPFLAGS="$DEFINED_F77INT64"

med2_ok=no

LOCAL_INCLUDES="$HDF5_INCLUDES"
LOCAL_LIBS="-lmed -lmedimportcxx $HDF5_LIBS"

if test -z $MED2HOME
then
   AC_MSG_WARN(undefined MED2HOME variable which specify med2 installation directory)
   AC_PATH_PROG(MDUMP, mdump)
   if test "xMDUMP" != "x" ; then
      MED2HOME=$MDUMP
      MED2HOME=`echo ${MED2HOME} | sed -e "s,[[^/]]*$,,;s,/$,,;s,^$,.,"`
      MED2HOME=`echo ${MED2HOME} | sed -e "s,[[^/]]*$,,;s,/$,,;s,^$,.,"`
   fi
fi
if test ! -z $MED2HOME
then
   LOCAL_INCLUDES="$LOCAL_INCLUDES -I$MED2HOME/include"
   if test "x$MED2HOME" = "x/usr"
   then
     LOCAL_LIBS="-lmed  $LOCAL_LIBS"
   else
     LOCAL_LIBS="-L$MED2HOME/lib $LOCAL_LIBS"
   fi
fi

dnl check med2 header

f77int="F77INT32"
case  $host_os in
   irix5.* | irix6.* | osf4.* | osf5.* | linux*  )

        linux64="true"
# porting on intel processor 64 bits
        expr "$host_os" : 'linux' >/dev/null && ( test ! x"$host_cpu" = x"x86_64" && test ! x"$host_cpu" = x"ia64" ) && linux64="false"
	if test ! x"$linux64" = "xfalse" ; then
	  echo "$as_me:$LINENO: checking for 64bits integers size in F77/F90" >&5
echo $ECHO_N "checking for 64bits integers size in F77/F90... $ECHO_C" >&6
	  # Check whether --enable-int64 or --disable-int64 was given.
if test "${enable_int64+set}" = set; then
  enableval="$enable_int64"

fi;
	  case "X-$enable_int64" in
	    X-no)
	     echo "$as_me:$LINENO: result: \"disabled\"" >&5
echo "${ECHO_T}\"disabled\"" >&6
	     SUFFIXES="_32"
	     ;;
	    *)
	     echo "$as_me:$LINENO: result: \"enabled\"" >&5
echo "${ECHO_T}\"enabled\"" >&6
	     SUFFIXES=""
	     f77int="F77INT64"
	     ;;
	  esac
	fi
     ;;
   *)
     ;;
esac

case $host_os in
    linux*)
        test x"$linux64" = x"true" && \
          MACHINE="PCLINUX64${SUFFIXES}" || \
	MACHINE=PCLINUX
	;;
    hpux*)
	MACHINE=HP9000
	;;
    aix4.*)
	MACHINE=RS6000
	host_os_novers=aix4.x
	;;
    irix5.*)
	MACHINE="IRIX64${SUFFIXES}"
	host_os_novers=irix5.x
	;;
    irix6.*)
	MACHINE="IRIX64${SUFFIXES}"
	host_os_novers=irix6.x
	;;
    osf4.*)
	MACHINE="OSF1${SUFFIXES}"
	host_os_novers=osf4.x
	;;
    osf5.*)
	MACHINE="OSF1${SUFFIXES}"
	 host_os_novers=osf5.x
	 ;;
    solaris2.*)
	MACHINE=SUN4SOL2
	 host_os_novers=solaris2.x
	 ;;
    uxpv*)
	MACHINE=VPP5000
	 ;;
    *)
	MACHINE=
	 host_os_novers=$host_os
	 ;;
esac

CPPFLAGS_old="$CPPFLAGS"
dnl we must test system : linux = -DPCLINUX
dnl we must test system : Alpha-OSF = -DOSF1
case $host_os in
   linux*)
      CPPFLAGS="$CPPFLAGS -D$MACHINE $LOCAL_INCLUDES"
dnl      CPPFLAGS="$CPPFLAGS -DPCLINUX $LOCAL_INCLUDES"
      ;;
   osf*)
      CPPFLAGS="$CPPFLAGS -DOSF1 $LOCAL_INCLUDES"
      ;;
esac
AC_CHECK_HEADER(med.h,med2_ok=yes ,med2_ok=no)
CPPFLAGS="$CPPFLAGS_old"

if  test "x$med2_ok" = "xyes"
then

dnl check med2 library

  LIBS_old="$LIBS"
  LIBS="$LIBS $LOCAL_LIBS"
  AC_CHECK_LIB(med,MEDouvrir,med2_ok=yes,med2_ok=no)

  if  test "x$med2_ok" = "xyes"
  then
   AC_CHECK_LIB(medimportcxx,HAVE_MEDimport,med2_ok=yes,med2_ok=no)
  fi
  LIBS="$LIBS_old"
 
fi

if  test "x$med2_ok" = "xyes"
then
case $host_os in
   linux*)
      MED2_INCLUDES="-D$MACHINE $LOCAL_INCLUDES"
dnl      MED2_INCLUDES="-DPCLINUX $LOCAL_INCLUDES"
      ;;
   osf*)
      MED2_INCLUDES="-DOSF1 $LOCAL_INCLUDES"
      ;;
esac
  MED2_LIBS="$LOCAL_LIBS"
  MED2_MT_LIBS="$LOCAL_LIBS"
fi


AC_MSG_RESULT(for med2: $med2_ok)

])dnl
