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
