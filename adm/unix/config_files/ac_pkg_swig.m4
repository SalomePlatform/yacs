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

dnl @synopsis AC_PROG_SWIG([major.minor.micro])
dnl This macro searches for a SWIG installation on your system. If found you
dnl should) SWIG via $(SWIG).  You can use the optional first argument to check
dnl if the version of the available SWIG is greater than or equal to the
dnl value of the argument.  It should have the format: N[.N[.N]] (N is a
dnl number between 0 and 999.  Only the first N is mandatory.)
dnl If the version argument is given (e.g. 1.3.17), AC_PROG_SWIG checks that the
dnl swig package is this version number or higher.
dnl In configure.in, use as:
dnl		AC_PROG_SWIG(1.3.17)
dnl		SWIG_ENABLE_CXX
dnl		SWIG_MULTI_MODULE_SUPPORT
dnl		SWIG_PYTHON
dnl @authors Sebastian Huber <address@hidden>, Alan W. Irwin
dnl <address@hidden>, Rafael Laboissiere <address@hidden> and
dnl Andrew Collier <address@hidden>.
dnl
AC_DEFUN([AC_PROG_SWIG],[
  swig_ok=no
	AC_PATH_PROG([SWIG],[swig])
	if test -z "$SWIG" ; then
		AC_MSG_WARN([cannot find 'swig' program. You should look at http://www.swig.org])
		SWIG='echo "Error: SWIG is not installed. You should look at http://www.swig.org" ; false'
	elif test -n "$1" ; then
		AC_MSG_CHECKING([for SWIG version])
		[swig_version=`$SWIG -version 2>&1 | grep 'SWIG Version' | sed 's/.*\([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\).*/\1/g'`]
		AC_MSG_RESULT([$swig_version])
		if test -n "$swig_version" ; then
			# Calculate the required version number components
			[required=$1]
			[required_major=`echo $required | sed 's/[^0-9].*//'`]
			if test -z "$required_major" ; then
				[required_major=0]
			fi
			[required=`echo $required | sed 's/[0-9]*[^0-9]//'`]
			[required_minor=`echo $required | sed 's/[^0-9].*//'`]
			if test -z "$required_minor" ; then
				[required_minor=0]
			fi
			[required=`echo $required | sed 's/[0-9]*[^0-9]//'`]
			[required_patch=`echo $required | sed 's/[^0-9].*//'`]
			if test -z "$required_patch" ; then
				[required_patch=0]
			fi
			# Calculate the available version number components
			[available=$swig_version]
			[available_major=`echo $available | sed 's/[^0-9].*//'`]
			if test -z "$available_major" ; then
				[available_major=0]
			fi
			[available=`echo $available | sed 's/[0-9]*[^0-9]//'`]
			[available_minor=`echo $available | sed 's/[^0-9].*//'`]
			if test -z "$available_minor" ; then
				[available_minor=0]
			fi
			[available=`echo $available | sed 's/[0-9]*[^0-9]//'`]
			[available_patch=`echo $available | sed 's/[^0-9].*//'`]
			if test -z "$available_patch" ; then
				[available_patch=0]
			fi
      if test $available_major -lt $required_major ; then
        AC_MSG_WARN([SWIG version >= $1 is required.  You have $swig_version (major version too low).  You should look at http://www.swig.org])
        SWIG='echo "Error: SWIG version >= $1 is required.  You have '"$swig_version"'.  You should look at http://www.swig.org" ; false'
      elif test $available_major -eq $required_major; then
        if test $available_minor -lt $required_minor ; then
          AC_MSG_WARN([SWIG version >= $1 is required.  You have $swig_version (minor version too low).  You should look at http://www.swig.org])
          SWIG='echo "Error: SWIG version >= $1 is required.  You have '"$swig_version"'.  You should look at http://www.swig.org" ; false'
        elif test $available_minor -eq $required_minor ;then
          if test $available_patch -lt $required_patch; then
            AC_MSG_WARN([SWIG version >= $1 is required.  You have $swig_version (maintenance version too low).  You should look at http://www.swig.org])
            SWIG='echo "Error: SWIG version >= $1 is required.  You have '"$swig_version"'.  You should look at http://www.swig.org" ; false'
          else
            AC_MSG_NOTICE([SWIG executable is '$SWIG'])
            swig_ok=yes
            SWIG_LIB=`$SWIG -swiglib`
            AC_MSG_NOTICE([SWIG runtime library directory is '$SWIG_LIB'])
          fi
        else
          AC_MSG_NOTICE([SWIG executable is '$SWIG'])
          swig_ok=yes
          SWIG_LIB=`$SWIG -swiglib`
          AC_MSG_NOTICE([SWIG runtime library directory is '$SWIG_LIB'])
        fi
      else
        AC_MSG_NOTICE([SWIG executable is '$SWIG'])
        swig_ok=yes
        SWIG_LIB=`$SWIG -swiglib`
        AC_MSG_NOTICE([SWIG runtime library directory is '$SWIG_LIB'])
      fi
		else
			AC_MSG_WARN([cannot determine SWIG version])
			SWIG='echo "Error: Cannot determine SWIG version.  You should look at http://www.swig.org" ; false'
		fi
	fi
	AC_SUBST([SWIG_LIB])
])

# SWIG_ENABLE_CXX()
#
# Enable SWIG C++ support.  This affects all invocations of $(SWIG).
AC_DEFUN([SWIG_ENABLE_CXX],[
	AC_REQUIRE([AC_PROG_SWIG])
	AC_REQUIRE([AC_PROG_CXX])
	SWIG="$SWIG -c++"
])

# SWIG_MULTI_MODULE_SUPPORT()
#
# Enable support for multiple modules.  This effects all invocations
# of $(SWIG).  You have to link all generated modules against the
# appropriate SWIG runtime library.  If you want to build Python
# modules for example, use the SWIG_PYTHON() macro and link the
# modules against $(SWIG_PYTHON_LIBS).
AC_DEFUN([SWIG_MULTI_MODULE_SUPPORT],[
	AC_REQUIRE([AC_PROG_SWIG])
	SWIG="$SWIG -c"
])

# SWIG_PYTHON([use-shadow-classes = {no, yes}])
#
# Checks for Python and provides the $(SWIG_PYTHON_CPPFLAGS),
# $(SWIG_PYTHON_LIBS) and $(SWIG_PYTHON_OPT) output variables.
# $(SWIG_PYTHON_OPT) contains all necessary SWIG options to generate
# code for Python.  Shadow classes are enabled unless the value of the
# optional first argument is exactly 'no'.  If you need multi module
# support (provided by the SWIG_MULTI_MODULE_SUPPORT() macro) use
# $(SWIG_PYTHON_LIBS) to link against the appropriate library.  It
# contains the SWIG Python runtime library that is needed by the type
# check system for example.
AC_DEFUN([SWIG_PYTHON],[
	AC_REQUIRE([AC_PROG_SWIG])
	AC_REQUIRE([AC_PYTHON_DEVEL])
	test "x$1" != "xno" || swig_shadow=" -noproxy"
	AC_SUBST([SWIG_PYTHON_OPT],[-python$swig_shadow])
	AC_SUBST([SWIG_PYTHON_CPPFLAGS],[$PYTHON_CPPFLAGS])
	AC_SUBST([SWIG_PYTHON_LIBS],["-L$SWIG_LIB -lswigpy"])
])


dnl @synopsis AC_LIB_WAD
dnl
dnl This macro searches for installed WAD library.
dnl
AC_DEFUN([AC_LIB_WAD],
[
	AC_REQUIRE([AC_PYTHON_DEVEL])
	AC_ARG_ENABLE(wad,
	AC_HELP_STRING([--enable-wad], [enable wad module]),
	[
		case "${enableval}" in
			no)	;;
			*)	if test "x${enableval}" = xyes;
				then
					check_wad="yes"
				fi ;;
		esac
	], [])

	if test -n "$check_wad";
	then
		AC_CHECK_LIB(wadpy, _init, [WADPY=-lwadpy], [], $PYTHON_LDFLAGS $PYTHON_EXTRA_LIBS)
		AC_SUBST(WADPY)
	fi
])
