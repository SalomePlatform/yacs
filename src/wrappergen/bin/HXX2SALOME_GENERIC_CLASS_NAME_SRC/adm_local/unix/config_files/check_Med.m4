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

# Check availability of Med binary distribution
#
# Author : Anthony GEAY (CEA, 2005)
#

AC_DEFUN([CHECK_MED],[

CHECK_HDF5
CHECK_MED2

AC_CHECKING(for Med)

Med_ok=no

AC_ARG_WITH(med,
	    [  --with-med=DIR root directory path of MED installation ],
	    MED_DIR="$withval",MED_DIR="")

if test "x$MED_DIR" == "x" ; then

# no --with-med-dir option used

   if test "x$MED_ROOT_DIR" != "x" ; then

    # MED_ROOT_DIR environment variable defined
      MED_DIR=$MED_ROOT_DIR

   else

    # search Med binaries in PATH variable
      AC_PATH_PROG(TEMP, libMEDMEM_Swig.py)
      if test "x$TEMP" != "x" ; then
         MED_BIN_DIR=`dirname $TEMP`
         MED_DIR=`dirname $MED_BIN_DIR`
      fi
      
   fi
# 
fi

if test -f ${MED_DIR}/bin/salome/libMEDMEM_Swig.py ; then
   Med_ok=yes
   AC_MSG_RESULT(Using Med module distribution in ${MED_DIR})

   if test "x$MED_ROOT_DIR" == "x" ; then
      MED_ROOT_DIR=${MED_DIR}
   fi
   AC_SUBST(MED_ROOT_DIR)
   MED_INCLUDES="-I${MED_ROOT_DIR}/include/salome ${MED2_INCLUDES} ${HDF5_INCLUDES} -I${KERNEL_ROOT_DIR}/include/salome"
   MED_LIBS="-L${MED_ROOT_DIR}/lib/salome -lmedmem"
   AC_SUBST(MED_INCLUDES)
   AC_SUBST(MED_LIBS)

else
   AC_MSG_WARN("Cannot find Med module sources")
fi

AC_MSG_CHECKING([for MED memory version])
[medmem_version=`cat ${MED_ROOT_DIR}/bin/salome/VERSION | cut -d" " -f7`]
[medmem_version=`expr $medmem_version : '\([0-9.]*\).*'`]
AC_MSG_RESULT([$medmem_version])
AC_MSG_CHECKING([for g++ version])
[gpp_version=`g++ --version | sed -e '2,$d' | cut -d" " -f3`]
AC_MSG_RESULT([$gpp_version])
[available=$gpp_version]
dnl Analyzing g++ version
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
dnl Testing if g++ verion >= 3.4.0 or not
if test $available_major -ne "3" \
	-o $available_minor -ne "4" \
	-o $available_patch -lt "0" ; then
		[required_medmem_major=2]
		[required_medmem_minor=2]
		[required_medmem_patch=0]
else
		[required_medmem_major=2]
		[required_medmem_minor=2]
		[required_medmem_patch=4]
fi
[available=$medmem_version]
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
[available_num=`expr $available_major \* 100 + $available_minor \* 10 + $available_patch`]
[required_num=`expr $required_medmem_major \* 100 + $required_medmem_minor \* 10 + $required_medmem_patch`]
if test "x$Med_ok" == "xyes" ; then
	if test $available_num -lt $required_num ; then
		AC_MSG_WARN([MEDMEM version invalid with your compiler : MEDMEM version >=2.2.4 required !!!])
		Med_ok=no
	fi
fi
AC_MSG_RESULT(for MED memory: $Med_ok)
 
])dnl
 
