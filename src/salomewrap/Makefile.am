
include $(top_srcdir)/adm/unix/make_begin.am

# Libraries targets
lib_LTLIBRARIES = libSalomeWrap.la

dist_libSalomeWrap_la_SOURCES = \
	SalomeWrap_Module.cxx \
	SalomeWrap_DataModel.cxx \
	SuitWrapper.cxx \
	WrapGraphicsView.cxx

# MOC pre-processing
MOC_FILES = \
	SalomeWrap_Module_moc.cxx

nodist_libSalomeWrap_la_SOURCES = $(MOC_FILES)

libSalomeWrap_la_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(OMNIORB_INCLUDES) \
	-I$(KERNEL_ROOT_DIR)/include/salome \
	-I$(GUI_ROOT_DIR)/include/salome \
	-I$(srcdir)/../bases \
	-I$(srcdir)/../engine \
	-I$(srcdir)/../runtime \
	-I$(srcdir)/../yacsloader \
	-I$(srcdir)/../hmi \
	-I$(srcdir)/../genericgui \
	$(qt4_cppflags)

#	$(THREAD_DEF) \
#	$(PYTHON_CPPFLAGS) \
#	$(OMNIORB_INCLUDES) \
#	$(LIBXML_INCLUDES) \
#	$(BOOST_CPPFLAGS) \
#	-I$(KERNEL_ROOT_DIR)/include/salome \
#	-I$(GUI_ROOT_DIR)/include/salome \
#	$(qt4_cppflags) \
#	$(qsci4_cppflags) \
#	@GRAPHVIZ_CPPFLAGS@

libSalomeWrap_la_LDFLAGS  = \
	-L$(GUI_ROOT_DIR)/lib/salome \
	 $(qt4_ldflags)

#	 @GRAPHVIZ_LDFLAGS@ \
#	 -L$(GUI_ROOT_DIR)/lib/salome -L$(KERNEL_ROOT_DIR)/lib/salome \
#	 $(qt4_ldflags)

libSalomeWrap_la_LIBADD  = \
	$(qt4_libs) \
	-lSalomeApp

# resources files


# --------------------------------------------
# *.h --> *_moc.cxx
# --------------------------------------------

SUFFIXES = .hxx _moc.cxx .qrc _qrc.cxx

.hxx_moc.cxx :
	$(QT_MOC) -p . -o $@ $<


clean-local-qt :
	rm -f *_moc.cxx *_qrc.cxx


clean-local: clean-local-qt