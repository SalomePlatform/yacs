dnl Copyright (C) 2006-2021  CEA/DEN, EDF R&D
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

AC_DEFUN([CHECK_HTML_GENERATORS],[

AC_CHECKING(for html generators)

doxygen_ok=yes
dnl were is doxygen ?
AC_PATH_PROG(DOXYGEN,doxygen) 
if test "x$DOXYGEN" = "x"
then
  AC_MSG_WARN(doxygen not found)
  doxygen_ok=no
fi
if test "x$doxygen_ok" = "xyes"
then
  version=`$DOXYGEN --version`
  AC_MSG_RESULT(doxygen version $version)
  case "$version" in
	1.4.4*)
	  DOXYGEN_WITH_PYTHON=yes
	  DOXYGEN_WITH_STL=no
	  ;;
	1.4.5*)
	  DOXYGEN_WITH_PYTHON=yes
	  DOXYGEN_WITH_STL=yes
	  ;;
	1.4.6*)
	  DOXYGEN_WITH_PYTHON=yes
	  DOXYGEN_WITH_STL=yes
	  ;;
	1.4.7*)
	  DOXYGEN_WITH_PYTHON=yes
	  DOXYGEN_WITH_STL=yes
	  ;;
	1.4.8*)
	  DOXYGEN_WITH_PYTHON=yes
	  DOXYGEN_WITH_STL=yes
	  ;;
	1.4.9*)
	  DOXYGEN_WITH_PYTHON=yes
	  DOXYGEN_WITH_STL=yes
	  ;;
	[[1-9]].[[5-9]]*)
	  DOXYGEN_WITH_PYTHON=yes
	  DOXYGEN_WITH_STL=yes
	  ;;
	*)
	  DOXYGEN_WITH_PYTHON=no
	  DOXYGEN_WITH_STL=no
	  ;;
  esac
  AC_MSG_RESULT(doxygen with support STL    - $DOXYGEN_WITH_STL)
  AC_MSG_RESULT(doxygen with support PYTHON - $DOXYGEN_WITH_PYTHON)
  AC_SUBST(DOXYGEN_WITH_PYTHON)
  AC_SUBST(DOXYGEN_WITH_STL)
fi
AM_CONDITIONAL(DOXYGEN_IS_OK, [test x"$doxygen_ok" = xyes])
dnl AC_SUBST(DOXYGEN)

AC_SUBST(GRAPHVIZHOME)
AC_SUBST(GRAPHVIZ_CPPFLAGS)
AC_SUBST(GRAPHVIZ_LDFLAGS)
AC_SUBST(GRAPHVIZ_LIBADD)
GRAPHVIZ_CPPFLAGS=""
GRAPHVIZ_LDFLAGS=""
GRAPHVIZ_LIBADD=""

AC_CHECKING([graphviz configuration])

graphviz_ok=yes
dnl were is graphviz ?
AC_PATH_PROG(DOT,dot) 
if test "x$DOT" = "x" ; then
  AC_MSG_WARN(graphviz not found)
  graphviz_ok=no
else
  GRAPHVIZ_PREFIX=`echo $DOT | sed -e "s,[[^/]]*$,,;s,/$,,;s,^$,.,"`
  GRAPHVIZ_PREFIX=`echo $GRAPHVIZ_PREFIX | sed -e "s,[[^/]]*$,,;s,/$,,;s,^$,.,"`
  GRAPHVIZHOME=$GRAPHVIZ_PREFIX

  GRAPHVIZ_CPPFLAGS="-I${GRAPHVIZHOME}/include/graphviz"
  if test "x$GRAPHVIZHOME" = "x/usr" ; then
    GRAPHVIZ_LDFLAGS="-L${GRAPHVIZHOME}/lib/graphviz"
  else
    GRAPHVIZ_LDFLAGS="-L${GRAPHVIZHOME}/lib -L${GRAPHVIZHOME}/lib/graphviz"
  fi
fi
dnl AC_SUBST(DOT)
CPPFLAGS_old=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $GRAPHVIZ_CPPFLAGS"
old_graphviz="yes"
AC_CHECK_HEADERS(dotneato.h, old_graphviz="yes", old_graphviz="no")
if test "x$old_graphviz" = "xyes" ; then
  GRAPHVIZ_LIBADD="-ldotneato"
else
  GRAPHVIZ_LIBADD="-lgvc"
fi
CPPFLAGS=$CPPFLAGS_old

AC_PATH_PROG(LATEX,latex) 
if test "x$LATEX" = "x" ; then
  AC_MSG_WARN(latex not found)
fi
AC_SUBST(LATEX)

AC_PATH_PROG(DVIPS,dvips)
if test "x$DVIPS" = "x" ; then
  AC_MSG_WARN(dvips not found)
fi
AC_SUBST(DVIPS)

AC_PATH_PROG(PDFLATEX,pdflatex)
if test "x$PDFLATEX" = "x" ; then
  AC_MSG_WARN(pdflatex not found)
fi
AC_SUBST(PDFLATEX)

rst2html_ok=yes
dnl were is rst2html ?
AC_PATH_PROG(RST2HTML,rst2html) 
if test "x$RST2HTML" = "x"; then
  AC_PATH_PROG(RST2HTML,rst2html.py)
fi

if test "x$RST2HTML" = "x"; then
  AC_MSG_WARN(rst2html not found)
  rst2html_ok=no
fi
AC_SUBST(RST2HTML)

AM_CONDITIONAL(RST2HTML_IS_OK, [test x"$rst2html_ok" = xyes])

])dnl
dnl
