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
dnl when HAS_GUI is false (no salome gui) and nothing is said for qt4, qt4 is not checked
dnl usages
dnl ./configure --prefix=/home/prascle/partage/maquettes/install 
dnl ./configure --prefix=/home/prascle/partage/maquettes/install --with-qt4
dnl ./configure --prefix=/home/prascle/partage/maquettes/install --with-qt4=/opt/qt443
dnl ./configure --prefix=/home/prascle/partage/maquettes/install --with-qt4-includes=/usr/include/qt4
dnl ./configure --prefix=/home/prascle/partage/maquettes/install --with-qt4-includes=/usr/include/qt4 --with-qt4-libraries=/usr/lib -with-qt4-tools=/usr/bin
dnl ./configure --prefix=/home/prascle/partage/maquettes/install --without-qt4
dnl
AC_DEFUN([I2_CHECK_QT4],
[
  AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AC_PROG_CPP])

  qt_ok=no
  qt4_wanted=yes
  WITH_QT4=0

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

  # --- if qt4 standard install directory is not defined
  #       if HAS_GUI= 0: Qt4 not wanted
  #       else: QTDIR value if defined, or /usr
  if test x${withval} = xnotset
  then
    if test x${HAS_GUI} = x0
    then
      qt4_wanted=no
      AC_MSG_NOTICE([SALOME GUI not present, Qt4 not specified, skip detection])
    else
      if test -z $QTDIR
      then
        qt4_install_path="/usr"
      else
        if test $QTDIR = /usr/lib/qt3 ; then
          if test -d /usr/lib/qt4 ; then
            AC_MSG_RESULT(it is strange for a qt4 installation !)
            AC_MSG_RESULT(/usr/lib/qt4 is present)
            AC_MSG_RESULT(replacing QTDIR by /usr/lib/qt4)
            QTDIR=/usr/lib/qt4
          fi
        fi
        qt4_install_path=$QTDIR
      fi
    fi
  fi
  
  # --- if qt4 standard install directory is not wanted: --without-qt4 or --with-qt4=no
  if test x${withval} = xno
  then
    qt4_wanted=no
    AC_MSG_NOTICE([Qt4 is not wanted, skip detection])
  fi

  QT_DIR=${qt4_install_path}
  AC_SUBST(QT_DIR)

  # ----------------------------------------------------------------------
  # --- only when qt4 wanted (no explicit --without-qt4 nor --with-qt4=no)

  if test x${qt4_wanted} = xyes
  then

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


  # test if qt4 is completely defined
  qt4_defined=yes
  if test x${qt4_include_path} = x
  then
    qt4_defined=no
    AC_MSG_NOTICE([No Qt4 include path defined])
  else
    AC_MSG_NOTICE([Qt4 include path is ${qt4_include_path}])
  fi
  if test x${qt4_library_path} = x
  then
    qt4_defined=no
    AC_MSG_NOTICE([No Qt4 library path defined])
  else
    AC_MSG_NOTICE([Qt4 library path is ${qt4_library_path}])
  fi
  if test x${qt4_tools_path} = x
  then
    qt4_defined=no
    AC_MSG_NOTICE([No Qt4 tools path defined])
  else
    AC_MSG_NOTICE([Qt4 tools   path is ${qt4_tools_path}])
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
    QTINC=""
    AC_CHECK_FILE($qt4_include_path/qt4/QtCore/qglobal.h,QTINC="/qt4",QTINC="")
    qt4_include_path="$qt4_include_path${QTINC}"
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
    if test x${qt4_library_path} = x/usr/lib
    then
      qt4_ldflags=""
    else
      qt4_ldflags="-L$qt4_library_path"
    fi

    AC_MSG_NOTICE([checking whether link with qt4 is working])
    qt4_libs="-lQtCore -lQtGui"
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
    qt_ok=yes
    AC_MSG_NOTICE([Qt4 support is OK])

  else
    # --- no Qt4 support
    AC_MSG_NOTICE([No Qt4 support])
  fi

  # restoring saved values
  CPPFLAGS=$saved_CPPFLAGS
  LDFLAGS=$saved_LDFLAGS
  LIBS=$saved_LIBS

  fi
  # --- end: only when qt4 wanted (no explicit --without-qt4 nor --with-qt4=no)
  # ----------------------------------------------------------------------

  # Propagate test into atlocal
  AC_SUBST(WITH_QT4)

  # Propagate test into Makefiles...
  AM_CONDITIONAL(WITH_QT4, test $WITH_QT4 = 1)

  # ... and into source files
  AC_DEFINE_UNQUOTED(HAS_QT4, $WITH_QT4, [Qt4 library])

])


AC_DEFUN([I2_CHECK_QSCINTILLA],
[
  AC_REQUIRE([I2_CHECK_QT4])
  qscintilla_ok=no
  WITH_QSCI4=0

  # ----------------------------------------------------------------------
  # --- check qscintilla only when qt4 OK

  if test x${WITH_QT4} = x1
  then

  # --- check if qsci4 includes directory is defined
  qsci4_include_path=""
  qsci4_cppflags=""

  AC_ARG_WITH([qsci4-includes],
    AC_HELP_STRING([--with-qsci4-includes@<:@=DIR@:>@], [where the Qsci4 includes are: default @<:@$QTDIR/include/Qsci@:>@ or @<:@/usr/include/Qsci@:>@]),
              [qsci4_include_path=$withval])

  if test x${qsci4_include_path} = x ; then
    # --- "" --- try to use QSCINTILLA_DIR/include/Qsci
    if test "x${QSCINTILLA_DIR}" != "x" ; then
      qsci4_include_path="${QSCINTILLA_DIR}/include"
    else
      qsci4_include_path=no
    fi
  fi

  if test x${qsci4_include_path} = xno ; then
    # --- "no" --- no native installation, try to find in Qt
    qsci4_include_path="${QT_DIR}/include${QTINC}/Qsci"
    qsci4_cppflags=" -I$qsci4_include_path"
    # " -I${QT_DIR}/include${QTINC}" is included in qt4_cppflags
  else
    if test x${qsci4_include_path} = xyes ; then
      # --- "yes" --- try to find in /usr/include/Qsci
      qsci4_include_path="/usr/include${QTINC}/Qsci"
      qsci4_cppflags=" -I$qsci4_include_path"
      # " -I/usr/include${QTINC}" is included in qt4_cppflags or by default (if QTINC is empty)
    else
      # --- "other" --- try to find in given dir
      qsci4_cppflags=" -I$qsci4_include_path/Qsci -I$qsci4_include_path"
    fi
  fi

  AC_MSG_NOTICE($qsci4_cppflags)

  # --- check if qsci4 libraries directory is defined
  qsci4_library_path=""

  AC_ARG_WITH([qsci4-libraries],
    AC_HELP_STRING([--with-qsci4-libraries@<:@=DIR@:>@], [where the Qsci4 libraries are: default @<:@$QTDIR/lib@:>@ or @<:@/usr/lib@:>@]),
              [qsci4_library_path=$withval])

  if test x${qsci4_library_path} = x ; then
    # --- "" --- try to use QSCINTILLA_DIR/lib
    if test "x${QSCINTILLA_DIR}" != "x" ; then
      qsci4_library_path="${QSCINTILLA_DIR}/lib"
    else
      qsci4_library_path=no
    fi
  fi

  if test x${qsci4_library_path} = xno ; then
    # --- "no" --- no native installation, try to find in Qt
    qsci4_library_path="${QT_DIR}/lib"
  else
    if test x${qsci4_library_path} = xyes ; then
      # --- "yes" --- try to find in /usr/lib
      qsci4_library_path="/usr/lib"
    fi
  fi

  AC_MSG_NOTICE($qsci4_library_path)

  # saving values for compilation variables
  saved_CPPFLAGS=$CPPFLAGS
  saved_LDFLAGS=$LDFLAGS
  saved_LIBS=$LIBS

  # ask for qsci4 support
  AC_MSG_NOTICE([checking whether qscintilla for qt4 headers are present])

  # --- we test the header file presence and usability
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
    if test x${qsci4_library_path} = x/usr/lib/qt4/lib
    then
      qsci4_library_path=/usr/lib
    fi
    if test x${qsci4_library_path} = x/usr/lib
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
      qscintilla_ok=yes
    fi

  else
    AC_MSG_NOTICE([no support for qscintilla for qt4])
  fi

  # restoring saved values
  CPPFLAGS=$saved_CPPFLAGS
  LDFLAGS=$saved_LDFLAGS
  LIBS=$saved_LIBS

  else
  # --- end: check qscintilla only when qt4 OK
  # ----------------------------------------------------------------------
    AC_MSG_NOTICE([qscintilla for qt4 not checked because Qt4 not wanted or not detected])
  fi

  # Propagate test into atlocal
  AC_SUBST(WITH_QSCI4)
  
  # Propagate test into Makefiles...
  AM_CONDITIONAL(WITH_QSCI4, test $WITH_QSCI4 = 1)

  # ... and into source files
  AC_DEFINE_UNQUOTED(HAS_QSCI4, $WITH_QSCI4, [QsciScintilla library])

])
