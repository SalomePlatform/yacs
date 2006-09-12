# Check presence of "CPPUNIT" product presence
#
# Author : Anthony GEAY (CEA, 2006)
#

AC_DEFUN([CHECK_THREAD],[

AC_CHECKING(for THREADs)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
thread_ok=no

AC_SUBST(THREAD_DEF)
if test `uname` = "Linux"
then
   THREAD_DEF="-DYACS_PTHREAD"
   AC_CHECK_HEADER(pthread.h,thread_ok=yes ,thread_ok=no)
   thread_ok="yes"
fi

AC_LANG_RESTORE
AC_MSG_RESULT(for THREADs: $thread_ok)

])dnl
