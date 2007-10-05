
include $(top_srcdir)/adm/unix/make_begin.am

docs:
	doxygen

install-data-local:docs
	$(INSTALL) -d $(docdir)/gui/YACSGui;
	cp -rf $(top_builddir)/doc/html/* $(docdir)/gui/YACSGui;
	cp -rf $(top_builddir)/doc/html/index.html $(docdir)/gui/YACSGui/index.htm;
	cp -rf $(top_srcdir)/src/yacsloader/samples $(docdir)/gui/YACSGui;
	rm -rf $(docdir)/gui/YACSGui/samples/CVS;
