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

check_SCRIPTS = \
	YacsLoaderTest.sh \
	YacsLoaderInSessionTest.sh 

TESTS = YacsLoaderTest.sh
TESTS_ENVIRONMENT=$(SHELL) -x

clean-local:
	rm -rf yacs* schema1_* schema2_* dump1_* dump2_* eo eo__POA dump*.xml CatalogResources.xml traceExec_*
	rm -f samples toto titi log1 xmlrun.sh echo_idl.py echo.hh echoSK.cc g.data
	if test -f "./bin/salome/appli_clean.sh"; then ./bin/salome/appli_clean.sh -f ; fi


check-local:
	cat /tmp/${USER}/UnitTestsResult

EXTRA_DIST = testEdit.py  testExec.py  testLoader.py  testRefcount.py  testResume.py  testSave.py

include $(top_srcdir)/adm/unix/make_end.am
