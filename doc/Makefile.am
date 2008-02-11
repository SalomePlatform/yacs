
include $(top_srcdir)/adm/unix/make_begin.am

html/main.html:yacs.dox
	doxygen

DOXFILES= bases.dox  engine.dox  hmi.dox  python.dox  runtime.dox  yacs.dox  yacsloader.dox

EXTRA_DIST= html $(DOXFILES)

install-data-local:html/main.html
	$(INSTALL) -d $(DESTDIR)$(docdir)/gui/YACS;
	if test -d "html"; then b=; else b="$(srcdir)/"; fi; \
	find $$b"html" -name "*.*" -exec cp -rf {} $(DESTDIR)$(docdir)/gui/YACS ';' ; \
  cp -rf $(DESTDIR)$(docdir)/gui/YACS/index.html $(DESTDIR)$(docdir)/gui/YACS/index.htm; \
  cp -rf $(top_srcdir)/src/yacsloader/samples $(DESTDIR)$(docdir)/gui/YACS; \
  rm -rf $(DESTDIR)$(docdir)/gui/YACS/samples/CVS;

uninstall-local:
	chmod -R +w $(DESTDIR)$(docdir)/gui/YACS
	find $(DESTDIR)$(docdir) -name "*.html" -type f -exec rm -rf {} \;
	find $(DESTDIR)$(docdir) -name "*.jpg" -type f -exec rm -rf {} \;
	find $(DESTDIR)$(docdir) -name "*.map" -type f -exec rm -rf {} \;
	find $(DESTDIR)$(docdir) -name "*.md5" -type f -exec rm -rf {} \;
	find $(DESTDIR)$(docdir) -name "*.png" -type f -exec rm -rf {} \;
	find $(DESTDIR)$(docdir) -name "*.gif" -type f -exec rm -rf {} \;
	find $(DESTDIR)$(docdir) -name "*.css" -type f -exec rm -rf {} \;
	find $(DESTDIR)$(docdir) -name "*.js*" -type f -exec rm -rf {} \;
	find $(DESTDIR)$(docdir) -name "*.jpeg*" -type f -exec rm -rf {} \;
	rm -rf $(DESTDIR)$(docdir)/gui/YACS/samples
	rm -rf $(DESTDIR)$(docdir)/gui/YACS/index.htm

clean-local:
	if test -d "html"; then rm -rf html ; fi

