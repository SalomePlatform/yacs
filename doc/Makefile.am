#  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
include $(top_srcdir)/adm/unix/make_begin.am

yacsdocdir=$(docdir)/gui/YACS

if SPHINX_IS_OK
yacsdoc_DATA=html/index.html

SPHINXOPTS      =
SOURCEDIR       = $(srcdir)
SPHINXBUILD     = sphinx-build
PAPEROPT_a4     = -D latex_paper_size=a4
ALLSPHINXOPTS   = -d doctrees $(PAPEROPT_a4) $(SPHINXOPTS) $(SOURCEDIR)

html/index.html:$(RSTFILES)
	make htm

htm:
	mkdir -p html doctrees
	$(SPHINXBUILD) -b html $(ALLSPHINXOPTS) html
	@echo
	@echo "Build finished. The HTML pages are in html."

latex:
	mkdir -p latex doctrees
	$(SPHINXBUILD) -b latex $(ALLSPHINXOPTS) latex
	@echo
	@echo "Build finished; the LaTeX files are in latex."
	@echo "Run \`make all-pdf' or \`make all-ps' in that directory to" \
	      "run these through (pdf)latex."

RSTFILES=advancepy.rst   console.rst        execution.rst hxx2salome.rst main_menu.rst     \
				 progdsc.rst     representation.rst toolbars.rst  execpy.rst     gui_components.rst \
         batch.rst       cppsalome.rst      execxml.rst   index.rst      modification.rst  \
				 pysalome.rst    schemapy.rst       yacsgen.rst   calcium.rst    dataflow.rst  \
				 integration.rst operations.rst     python.rst    schemaxml.rst  calculator.rst  \
				 gui.rst         intro.rst          principes.rst rappels.rst    scratch.rst


EXTRA_DIST=html $(RSTFILES) images

clean-local:
	-rm -rf html latex doctrees

install-data-local:
	$(INSTALL) -d $(DESTDIR)$(yacsdocdir)
	if test -d "html"; then b=; else b="$(srcdir)/"; fi; \
	cp -rf $$b"html"/* $(DESTDIR)$(yacsdocdir) ; \
	if test -f $$b"latex"/yacs.pdf; then cp -f $$b"latex"/yacs.pdf $(DESTDIR)$(yacsdocdir) ; fi;

uninstall-local:
	chmod -R +w $(DESTDIR)$(yacsdocdir)
	rm -rf $(DESTDIR)$(yacsdocdir)/*

endif

