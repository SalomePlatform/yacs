include $(top_srcdir)/adm/unix/make_begin.am

if SALOME_KERNEL
  SUBDIRS = idl src doc
else
  SUBDIRS = src doc
endif

install-exec-hook:
	if [ "x`uname -m`" == "xx86_64" ] ; then cd ${prefix} && ln -sf lib64 lib ; fi

check-local:
	cat /tmp/${USER}/UnitTestsResult

