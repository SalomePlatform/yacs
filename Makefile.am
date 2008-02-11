include $(top_srcdir)/adm/unix/make_begin.am

ACLOCAL_AMFLAGS = -I adm/unix/config_files

if SALOME_KERNEL
  SUBDIRS = idl src doc Demo
else
  SUBDIRS = src doc Demo
endif

install-exec-hook:
	if [ "x`uname -m`" == "xx86_64" ] ; then cd ${prefix} && ln -sf lib64 lib ; fi

check-local:
	cat /tmp/${USER}/UnitTestsResult

