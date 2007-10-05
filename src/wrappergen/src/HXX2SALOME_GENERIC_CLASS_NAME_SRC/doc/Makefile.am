include $(top_srcdir)/adm_local/unix/make_common_starter.am

SUBDIRS = 

doc_DATA =

EXTRA_DIST=$(doc_DATA)

STYLESHEET=$(srcdir)/rst.css

usr_docs:
	(cd salome && $(MAKE) $(AM_MAKEFLAGS) usr_docs)

dev_docs:
	(cd salome && $(MAKE) $(AM_MAKEFLAGS) dev_docs)
