# Copyright (C) 2006-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

include $(top_srcdir)/adm_local/unix/make_common_starter.am


IDL_FILES = \
  SALOME_Component.idl \
  SALOME_Exception.idl \
  SALOME_Comm.idl \
  SALOME_GenericObj.idl \
  SALOMEDS.idl \
  SALOMEDS_Attributes.idl \
  ${dummy}

IDL_SRC = \
  SALOME_ComponentSK.cc \
  SALOME_ExceptionSK.cc \
  SALOME_CommSK.cc \
  SALOME_GenericObjSK.cc \
  SALOMEDSSK.cc \
  SALOMEDS_AttributesSK.cc \
  ${dummy}

USE_MED=0

IDL_FILES += \
  HXX2SALOME_GENERIC_CLASS_NAME_Gen.idl
IDL_SRC += \
  HXX2SALOME_GENERIC_CLASS_NAME_GenSK.cc

lib_LIBRARIES = libHXX2SALOME_GENERIC_CLASS_NAMESK.a

AM_CXXFLAGS = -fPIC -DPIC
libHXX2SALOME_GENERIC_CLASS_NAMESK_a_SOURCES = $(IDL_SRC)
libHXX2SALOME_GENERIC_CLASS_NAMESK_a_CPPFLAGS = \
	-I$(top_builddir)/adm_local/unix \
	-I$(top_builddir)/idl \
	@CORBA_CXXFLAGS@ @CORBA_INCLUDES@

salomeidl_DATA = compo1_Gen.idl

# These variables defines the building process of CORBA files
OMNIORB_IDL         = @OMNIORB_IDL@
OMNIORB_IDLCXXFLAGS = @OMNIORB_IDLCXXFLAGS@
IDLCXXFLAGS = -bcxx @IDLCXXFLAGS@ -I$(top_builddir)/salome/idl -I${srcdir} -I${KERNEL_ROOT_DIR}/idl/salome -DXXX

%SK.cc %.hh: ${srcdir}/%.idl
	$(OMNIORB_IDL) $(IDLCXXFLAGS) $(OMNIORB_IDLCXXFLAGS) $<

%SK.cc %.hh: ${KERNEL_ROOT_DIR}/idl/salome/%.idl
	$(OMNIORB_IDL) $(IDLCXXFLAGS) $(OMNIORB_IDLCXXFLAGS) $<

%SK.cc %.hh: ${MED_ROOT_DIR}/idl/salome/%.idl
	$(OMNIORB_IDL) $(IDLCXXFLAGS) $(OMNIORB_IDLCXXFLAGS) $<

mostlyclean-local:
	-rm -f *.hh *.cc .depidl

# we use cpp to generate dependencies between idl files.
# option x c tells the preprocessor to consider idl as a c file.
# if an idl is modified, all idl dependencies are rebuilt

.depidl: $(IDL_FILES)
	@echo "" > $@
	@for dep in $^ dummy; do \
	  if [ $$dep != "dummy" ]; then \
	    echo Building dependencies for $$dep; \
	    $(CPP) $(C_DEPEND_FLAG) -x c -I$(srcdir) -I${KERNEL_ROOT_DIR}/idl/salome $$dep 2>/dev/null | \
	    sed 's/\.o/\SK.cc/' >>$@; \
	  fi; \
	done ;

-include .depidl

#
