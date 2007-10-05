bindir=${prefix}/bin/HXX2SALOME_Test

ENV_SH=${bindir}/env_HXX2SALOME.sh
ENV_CSH=${bindir}/env_HXX2SALOME.csh

bin_SCRIPTS=    hxx2salome hxx2salome_check hxx2salome_cpp hxx2salome_corba \
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


install-exec-hook: install_sh install_csh
	\cp -rf ${srcdir}/HXX2SALOME_GENERIC_CLASS_NAME_SRC ${bindir}

install_sh:
	echo "#------ HXX2SALOME ------" > ${ENV_SH}
	echo "export HXX2SALOME_ROOT_DIR=${bindir}" >> ${ENV_SH}
	echo "export PATH=\$${HXX2SALOME_ROOT_DIR}:\$${PATH}" >> ${ENV_SH}
	echo "##" >> ${ENV_SH}

install_csh:
	echo "#------ HXX2SALOME ------" > ${ENV_CSH}
	echo "setenv HXX2SALOME_ROOT_DIR ${bindir}" >> ${ENV_CSH}
	echo "setenv PATH \$${HXX2SALOME_ROOT_DIR}:\$${PATH}" >> ${ENV_CSH}
	echo "##" >> ${ENV_CSH}
