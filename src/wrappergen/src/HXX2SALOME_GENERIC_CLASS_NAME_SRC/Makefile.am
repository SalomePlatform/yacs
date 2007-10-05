include $(top_srcdir)/adm_local/unix/make_common_starter.am

if SALOME_KERNEL
   SUBDIRS=idl
   DIST_SUBDIRS=idl
else
   SUBDIRS=
   DIST_SUBDIRS=
endif

SUBDIRS += adm_local src doc bin resources
DIST_SUBDIRS += adm_local src doc bin resources

DISTCLEANFILES = a.out

install-exec-local:
	cp ${srcdir}/env_HXX2SALOME_GENERIC_CLASS_NAME.sh ${prefix}

dist-hook:
	rm -rf `find $(distdir) -name CVS`

usr_docs:
	(cd doc && $(MAKE) $(AM_MAKEFLAGS) usr_docs)

docs:usr_docs

dev_docs:
	(cd doc && $(MAKE) $(AM_MAKEFLAGS) dev_docs)

