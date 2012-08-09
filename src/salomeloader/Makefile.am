# Copyright (C) 2006-2012  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

include $(top_srcdir)/adm/unix/make_begin.am

dist_salomescript_SCRIPTS = salomeloader.sh salomeloader.bat
salomepython_PYTHON = salomeloader.py ElementTree.py ElementPath.py graph.py

check-local:testSalomeLoader.py
	python testSalomeLoader.py

EXTRA_DIST = samples

dist-hook:
	rm -rf `find $(distdir) -name CVS`

install-data-local:
	$(INSTALL) -d $(DESTDIR)$(prefix)/share/salome/yacssupervsamples
	cp -rf $(srcdir)/samples/*.xml $(DESTDIR)$(prefix)/share/salome/yacssupervsamples

uninstall-local:
	rm -rf $(DESTDIR)$(prefix)/share/salome/yacssupervsamples

distclean-local:
	rm -rf tata.xml

include $(top_srcdir)/adm/unix/make_end.am
