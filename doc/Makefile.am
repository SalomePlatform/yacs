#  Copyright (C) 2006-2010  CEA/DEN, EDF R&D
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

.PHONY : latex

if SPHINX_IS_OK

yacsdoc_DATA=html/index.html
html/index.html:$(RSTFILES)
	make htm

endif

dev_docs: build_dev_docs install_dev_docs

build_dev_docs:
	doxygen

DOXFILES= ref/bases.dox ref/engine.dox ref/hmi.dox ref/python.dox ref/runtime.dox ref/yacs.dox ref/yacsloader.dox

EXTRA_DIST= html htmldev _static exemples $(DOXFILES)

yacsdocdir=$(docdir)/gui/YACS
yacsdevdocdir=$(docdir)/gui/YACS/dev

SPHINXOPTS      =
SOURCEDIR       = $(srcdir)
SPHINXBUILD     = sphinx-build
PAPEROPT_a4     = -D latex_paper_size=a4
ALLSPHINXOPTS   = -d doctrees $(PAPEROPT_a4) $(SPHINXOPTS) $(SOURCEDIR)

htm:
	mkdir -p html doctrees
	$(SPHINXBUILD) -b html $(ALLSPHINXOPTS) html
	@echo
	@echo "Build finished. The HTML pages are in html."

latex:
	mkdir -p latex doctrees
	$(SPHINXBUILD) -b latex $(ALLSPHINXOPTS) latex
	@echo "Running LaTeX files through pdflatex..."
	make -C latex all-pdf
	@echo "pdflatex finished; the PDF files are in latex."

pdf:
	$(SPHINXBUILD) -b pdf $(ALLSPHINXOPTS) pdf
	@echo
	@echo "Build finished. The pdf documents are in pdf."

html:
		 mkdir -p $@

htmldev:
		 mkdir -p $@

RSTFILES=          \
accesCorba.rst     \
accesLocal.rst     \
advancepy.rst      \
annexe_common.rst  \
batch.rst          \
calcium.rst        \
calciumyacs.rst    \
calciumapi.rst     \
calciummod.rst     \
calculator.rst     \
compInterne.rst    \
components.rst     \
console.rst        \
cppsalome.rst      \
dataflow.rst       \
etapes.rst         \
execpy.rst         \
execution.rst      \
execxml.rst        \
gui_components.rst \
gui.rst            \
hxx2salome.rst     \
index.rst          \
integration.rst    \
using.rst          \
main_menu.rst      \
modification.rst   \
operations.rst     \
principes.rst      \
progdsc.rst        \
pysalome.rst       \
python.rst         \
rappels.rst        \
references.rst     \
representation.rst \
salome.rst         \
schemapy.rst       \
schemaxml.rst      \
scratch.rst        \
toolbars.rst       \
optimizer.rst      \
yacsgen.rst


EXTRA_DIST+= $(RSTFILES) images		 

EXTRA_DIST+= \
ref/classDiagram.png \
conf.py \
ref/createNodeCollaboration.png \
opml.dtd \
opml.xslt \
ref/programingRules.sxw \
ref/schema.jpeg \
ref/sequenceCreateNode.png \
ref/undoCreateNodeCollaboration.png

install-data-local:
	$(INSTALL) -d $(DESTDIR)$(yacsdocdir)
	if test -d "html"; then b=; else b="$(srcdir)/"; fi; \
	cp -rf $$b"html"/* $(DESTDIR)$(yacsdocdir) ; \
	if test -f $$b"latex"/using.pdf; then cp -f $$b"latex"/using.pdf $(DESTDIR)$(yacsdocdir) ; fi; \
	if test -f $$b"latex"/integration.pdf; then cp -f $$b"latex"/integration.pdf $(DESTDIR)$(yacsdocdir) ; fi;

install_dev_docs:
	$(INSTALL) -d $(DESTDIR)$(yacsdevdocdir)
	if test -d "htmldev"; then b=; else b="$(srcdir)/"; fi; \
	find $$b"htmldev" -name "*.*" -exec cp -rf {} $(DESTDIR)$(yacsdevdocdir) ';' ; \
 cp -rf $(top_srcdir)/src/yacsloader/samples $(DESTDIR)$(yacsdevdocdir); \
 rm -rf $(DESTDIR)$(yacsdevdocdir)/samples/CVS;

uninstall-local:
	chmod -R +w $(DESTDIR)$(yacsdocdir)
	rm -rf $(DESTDIR)$(yacsdocdir)/*

clean-local:
	-rm -rf html latex doctrees
	if test -d "html"; then rm -rf html ; fi
