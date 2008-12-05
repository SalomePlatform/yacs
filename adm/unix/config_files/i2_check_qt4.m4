dnl  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
dnl
dnl  This library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public
dnl  License as published by the Free Software Foundation; either
dnl  version 2.1 of the License.
dnl
dnl  This library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public
dnl  License along with this library; if not, write to the Free Software
dnl  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
dnl
dnl  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
dnl
dnl the following variables are exported:
dnl  QT_MOC
dnl  QT_UIC
dnl  QT_DIR
dnl  qt4_cppflags
dnl  qt4_ldflags
dnl  qt4_libs
dnl  WITH_QT4
dnl qt4 is searched in the following order
dnl   path given with --with-qt4 options
dnl   presence of QTDIR variable
dnl   /usr
dnl usages
dnl ../demoQt4/configure --prefix=/home/prascle/partage/maquettes/install 
dnl ../demoQt4/configure --prefix=/home/prascle/partage/maquettes/install --with-qt4
dnl ../demoQt4/configure --prefix=/home/prascle/partage/maquettes/install --with-qt4=/opt/qt433
dnl ../demoQt4/configure --prefix=/home/prascle/partage/maquettes/install --with-qt4-includes=/usr/include/qt4
dnl ../demoQt4/configure --prefix=/home/prascle/partage/maquettes/install --with-qt4-includes=/usr/include/qt4 --with-qt4-libraries=/usr/lib -with-qt4-tools=/usr/bin
dnl ../demoQt4/configure --prefix=/home/prascle/partage/maquettes/install --without-qt4
dnl
AC_DEFUN([I2_CHECK_QT4],
[
  AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AC_PROG_CPP])


  # --- check if qt4 standard install directory is defined (with subdirectories bin lib include)
  qt4_install_path=""
  AC_ARG_WITH([qt4],
    AC_HELP_STRING([--with-qt4@<:@=DIR@:>@], [where the Qt4 package with a standard install is: default @<:@$QTDIR@:>@ or @<:@/usr@:>@]),
                 [qt4_install_path=$withval], [withval=notset])

  # --- if qt4 standard install directory is defined but with no path: QTDIR value if defined, or /usr
  if test x${withval} = xyes
  then
    if test -z $QTDIR
    then
      qt4_install_path="/usr"
    else
      qt4_install_path=$QTDIR
    fi
  fi

  # --- if qt4 standard install directory is not defined: QTDIR value if defined, or /usr
  if test x${withval} = xnotset
  then
    if test -z $QTDIR
    then
      qt4_install_path="/usr"
    else
      qt4_install_path=$QTDIR
    fi
  fi
  
  # --- if qt4 standard install directory is not wanted: --without-qt4 or --with-qt4=no
  if test x${withval} = xno
  then
    qt4_install_path=""
  fi

  QT_DIR=${qt4_install_path}
  AC_SUBST(QT_DIR)

  # --- check if qt4 includes directory is defined
  qt4_include_path=""
  AC_ARG_WITH([qt4-includes],
    AC_HELP_STRING([--with-qt4-includes@<:@=DIR@:>@], [where the Qt4 includes are: default @<:@$QTDIR/include@:>@ or @<:@/usr/include@:>@]),
                 [qt4_include_path=$withval], [withval=no])
  if test x${withval} != xno
  then
    # --- qt4 includes directory is defined
    if test x${qt4_include_path} = xyes
    then
      qt4_include_path="/usr/include"
    fi
  else
    if test x${qt4_install_path} != x
    then
      qt4_include_path="${qt4_install_path}/include"
    fi
  fi


  # --- check if qt4 libraries directory is defined
  qt4_library_path=""
  AC_ARG_WITH([qt4-libraries],
    AC_HELP_STRING([--with-qt4-libraries@<:@=DIR@:>@], [where the Qt4 libraries are: default @<:@$QTDIR/lib@:>@ or @<:@/usr/lib@:>@]),
                 [qt4_library_path=$withval], [withval=no])
  if test x${withval} != xno
  then
    # --- qt4 libraries directory is defined
    if test x${qt4_library_path} = xyes
    then
      qt4_library_path="/usr/lib"
    fi
  else
    if test x${qt4_install_path} != x
    then
      qt4_library_path="${qt4_install_path}/lib"
    fi
  fi

  # --- check if qt4 tools directory is defined
  qt4_tools_path=""
  AC_ARG_WITH([qt4-tools],
    AC_HELP_STRING([--with-qt4-tools@<:@=DIR@:>@], [where the Qt4 executable tools are: default @<:@$QTDIR/bin@:>@ or @<:@/usr/bin@:>@]),
                 [qt4_tools_path=$withval], [withval=no])
  if test x${withval} != xno
  then
    # --- qt4 tools directory is defined
    if test x${qt4_tools_path} = xyes
    then
      qt4_tools_path="/usr/bin"
    fi
  else
    if test x${qt4_install_path} != x
    then
      qt4_tools_path="${qt4_install_path}/bin"
    fi
  fi

  WITH_QT4=0

  AC_MSG_NOTICE(${qt4_include_path})
  AC_MSG_NOTICE(${qt4_library_path})
  AC_MSG_NOTICE(${qt4_tools_path})

  # test if qt4 is completely defined
  qt4_defined=yes
  if test x${qt4_include_path} = x
  then
    qt4_defined=no
    AC_MSG_NOTICE([No Qt4 include path defined])
  fi
  if test x${qt4_library_path} = x
  then
    qt4_defined=no
    AC_MSG_NOTICE([No Qt4 library path defined])
  fi
  if test x${qt4_tools_path} = x
  then
    qt4_defined=no
    AC_MSG_NOTICE([No Qt4 tools path defined])
  fi

  # saving values for compilation variables
  saved_CPPFLAGS=$CPPFLAGS
  saved_LDFLAGS=$LDFLAGS
  saved_LIBS=$LIBS

  if ! test x${qt4_defined} = xno
  then
    # ask for qt4 support
    AC_MSG_NOTICE([checking whether qt4 tools and headers are present])

    # --- we test the presence of moc,first under the form moc-qt4, then moc
    AC_PATH_PROGS(qt4_moc_found, [moc-qt4 moc], no, ${qt4_tools_path})
    test x${qt4_moc_found} = xno && AC_MSG_ERROR([Qt4 moc command NOT FOUND])
    QT_MOC=$qt4_moc_found
    AC_SUBST(QT_MOC)

    # --- we test the presence of uic,first under the form uic-qt4, then uic
    AC_PATH_PROGS(qt4_uic_found, [uic-qt4 uic], no, ${qt4_tools_path})
    test x${qt4_uic_found} = xno && AC_MSG_ERROR([Qt4 uic command NOT FOUND])
    QT_UIC=$qt4_uic_found
    AC_SUBST(QT_UIC)

    # --- we test the presence of rcc,first under the form rcc-qt4, then rcc
    AC_PATH_PROGS(qt4_rcc_found, [rcc-qt4 rcc], no, ${qt4_tools_path})
    test x${qt4_rcc_found} = xno && AC_MSG_ERROR([Qt4 rcc command NOT FOUND])
    QT_RCC=$qt4_rcc_found
    AC_SUBST(QT_RCC)

    # --- we test the header file presence and usability
    qt4_cppflags=""
    qt4_cppflags="${qt4_cppflags} -I$qt4_include_path/QtCore"
    qt4_cppflags="${qt4_cppflags} -I$qt4_include_path/QtGui"
    qt4_cppflags="${qt4_cppflags} -I$qt4_include_path/Qt"
    qt4_cppflags="${qt4_cppflags} -I$qt4_include_path"
    CPPFLAGS="${CPPFLAGS} ${qt4_cppflags}"
    AC_LANG_PUSH(C++)
    AC_CHECK_HEADERS([qapplication.h],
                     [qt4_header_found=yes],
                     [qt4_header_found=no],
                     [])
    AC_LANG_POP(C++)
    test x${qt4_header_found} = xno && AC_MSG_ERROR([Qt4 include file NOT FOUND])
    AC_SUBST(qt4_cppflags)

    # --- we test the library file presence and usability
    if test x${qt4_library_path} = /usr/lib
    then
      qt4_ldflags=""
    else
      qt4_ldflags="-L$qt4_library_path"
    fi

    AC_MSG_NOTICE([checking whether link with qt4 is working])
    qt4_lib_name=QtGui
    qt4_libs="-l$qt4_lib_name"
    LDFLAGS="${LDFLAGS} ${qt4_ldflags}"
    LIBS="${LIBS} ${qt4_libs}"
    AC_LANG_PUSH(C++)

    AC_LINK_IFELSE([AC_LANG_PROGRAM(
    [[ #include <QApplication>
       #include <QPushButton> ]],
    [[ QApplication app(0, 0);
       QPushButton hello("Hello world!");
       hello.resize(100, 30);
       hello.show(); ]] )],
    [qt4_lib_found=yes], [qt4_lib_found=no] )

    test x${qt4_lib_found} = xno && AC_MSG_ERROR([Qt4 library NOT FOUND])

    AC_LANG_POP(C++)
    AC_SUBST(qt4_ldflags)
    AC_SUBST(qt4_libs)

    # --- after all tests are successful, we support Qt4
    WITH_QT4=1
    AC_MSG_NOTICE([Qt4 support is OK])

  else
    # --- no Qt4 support
    AC_MSG_NOTICE([No Qt4 support])
  fi

  # Propagate test into atlocal
  AC_SUBST(WITH_QT4)

  # Propagate test into Makefiles...
  AM_CONDITIONAL(WITH_QT4, test $WITH_QT4 = 1)

  # ... and into source files
  AC_DEFINE_UNQUOTED(HAS_QT4, $WITH_QT4, [Support for regular expression library])

  # restoring saved values
  CPPFLAGS=$saved_CPPFLAGS
  LDFLAGS=$saved_LDFLAGS
  LIBS=$saved_LIBS

])


AC_DEFUN([I2_CHECK_QSCINTILLA],
[
  AC_REQUIRE([I2_CHECK_QT4])

  # --- check if qsci4 includes directory is defined
  qsci4_include_path=""
  AC_ARG_WITH([qsci4-includes],
    AC_HELP_STRING([--with-qsci4-includes@<:@=DIR@:>@], [where the Qsci4 includes are: default @<:@$QTDIR/include/Qsci@:>@ or @<:@/usr/include/Qsci@:>@]),
                 [qsci4_include_path=$withval], [withval=no])
  if test x${withval} != xno
  then
    # --- qsci4 includes directory is defined
    if test x${qsci4_include_path} = xyes
    then
      qsci4_include_path="/usr/include/Qsci"
    fi
  else
    qsci4_include_path="${QT_DIR}/include/Qsci"
  fi
  AC_MSG_NOTICE($qsci4_include_path)

  # --- check if qsci4 libraries directory is defined
  qsci4_library_path=""
  AC_ARG_WITH([qsci4-libraries],
    AC_HELP_STRING([--with-qsci4-libraries@<:@=DIR@:>@], [where the Qsci4 libraries are: default @<:@$QTDIR/lib@:>@ or @<:@/usr/lib@:>@]),
                 [qsci4_library_path=$withval], [withval=no])
  if test x${withval} != xno
  then
    # --- qsci4 libraries directory is defined
    if test x${qsci4_library_path} = xyes
    then
      qsci4_library_path="/usr/lib"
    fi
  else
    qsci4_library_path="${QT_DIR}/lib"
  fi
  AC_MSG_NOTICE($qsci4_library_path)

  # saving values for compilation variables
  saved_CPPFLAGS=$CPPFLAGS
  saved_LDFLAGS=$LDFLAGS
  saved_LIBS=$LIBS

  # ask for qsci4 support
  AC_MSG_NOTICE([checking whether qscintilla for qt4 headers are present])

  # --- we test the header file presence and usability
  qsci4_cppflags=" -I$qsci4_include_path"
  CPPFLAGS="${CPPFLAGS} ${qsci4_cppflags} ${qt4_cppflags}"
  AC_LANG_PUSH(C++)
  AC_CHECK_HEADERS([qsciscintilla.h],
                   [qsci4_header_found=yes],
                   [qsci4_header_found=no],
                   [])
  AC_LANG_POP(C++)
  if test x${qsci4_header_found} = xyes
  then
    AC_SUBST(qsci4_cppflags)

    # --- we test the library file presence and usability
    if test x${qsci4_library_path} = /usr/lib
    then
      qsci4_ldflags=""
    else
      qsci4_ldflags="-L$qsci4_library_path"
    fi

    AC_MSG_NOTICE([checking whether qscintilla2 library is present])
    qsci4_libs=-lqscintilla2
    AC_CHECK_FILE([${qsci4_library_path}/libqscintilla2.so],
                   [qsci4_library_found=yes],
                   [qsci4_library_found=no])
    if test x${qsci4_library_found} = xyes
    then
      AC_MSG_NOTICE([qscintilla for qt4 support])
      AC_SUBST(qsci4_ldflags)
      AC_SUBST(qsci4_libs)
      WITH_QSCI4=1
      # Propagate test into atlocal
      AC_SUBST(WITH_QSCI4)

    fi

  else
    AC_MSG_NOTICE([no support for qscintilla for qt4])
  fi

  # Propagate test into Makefiles...
  AM_CONDITIONAL(WITH_QSCI4, test $WITH_QSCI4 = 1)

  # ... and into source files
  AC_DEFINE_UNQUOTED(HAS_QSCI4, $WITH_QSCI4, [Support for regular expression library])

  # restoring saved values
  CPPFLAGS=$saved_CPPFLAGS
  LDFLAGS=$saved_LDFLAGS
  LIBS=$saved_LIBS

])

