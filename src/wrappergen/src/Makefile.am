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

bindir=${prefix}/bin/HXX2SALOME_Test

ENV_SH=$(DESTDIR)${bindir}/env_HXX2SALOME.sh
ENV_CSH=$(DESTDIR)${bindir}/env_HXX2SALOME.csh

HXX2SALOMESCRIPTS =    hxx2salome hxx2salome_check hxx2salome_cpp hxx2salome_corba \
		parse01.awk \
		parse0.awk \
		parse1.awk \
		parse2.awk \
		parse30.awk \
		parse3.awk \
		parse4.awk \
		parse5.awk \
		renameSalomeModule \
		runIDLparser \
		compile_HXX2SALOME_GENERIC_CLASS_NAME.sh

bin_SCRIPTS=  $(HXX2SALOMESCRIPTS)

EXTRA_DIST = $(HXX2SALOMESCRIPTS) HXX2SALOME_GENERIC_CLASS_NAME_SRC

install-exec-hook: install_sh install_csh
	\cp -rf ${srcdir}/HXX2SALOME_GENERIC_CLASS_NAME_SRC $(DESTDIR)${bindir}

install_sh:
	echo "#------ HXX2SALOME ------" > ${ENV_SH}
	echo "export HXX2SALOME_ROOT_DIR=$(DESTDIR)${bindir}" >> ${ENV_SH}
	echo "export PATH=\$${HXX2SALOME_ROOT_DIR}:\$${PATH}" >> ${ENV_SH}
	echo "##" >> ${ENV_SH}

install_csh:
	echo "#------ HXX2SALOME ------" > ${ENV_CSH}
	echo "setenv HXX2SALOME_ROOT_DIR $(DESTDIR)${bindir}" >> ${ENV_CSH}
	echo "setenv PATH \$${HXX2SALOME_ROOT_DIR}:\$${PATH}" >> ${ENV_CSH}
	echo "##" >> ${ENV_CSH}

uninstall-local:
	if test -d $(DESTDIR)$(bindir)/HXX2SALOME_GENERIC_CLASS_NAME_SRC; then \
	chmod -R +w $(DESTDIR)$(bindir)/HXX2SALOME_GENERIC_CLASS_NAME_SRC ; fi
	rm -rf $(DESTDIR)$(bindir)/HXX2SALOME_GENERIC_CLASS_NAME_SRC
	rm -rf $(ENV_CSH) ${ENV_SH}
