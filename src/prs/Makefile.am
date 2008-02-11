
include $(top_srcdir)/adm/unix/make_begin.am

lib_LTLIBRARIES = libYACSPrs.la

# Implementation files
LIBSOURCES = \
	YACSPrs_ElementaryNode.cxx \
	YACSPrs_ServiceNode.cxx \
	YACSPrs_InlineNode.cxx \
	YACSPrs_IfNode.cxx \
	YACSPrs_SwitchNode.cxx \
	YACSPrs_LoopNode.cxx \
	YACSPrs_ForEachLoopNode.cxx \
	YACSPrs_BlocNode.cxx \
	YACSPrs_Link.cxx

# Headers
LIBHEADERS = \
	YACSPrs_Def.h \
	YACSPrs_ElementaryNode.h \
	YACSPrs_ServiceNode.h \
	YACSPrs_InlineNode.h \
	YACSPrs_IfNode.h \
	YACSPrs_SwitchNode.h \
	YACSPrs_LoopNode.h \
	YACSPrs_ForEachLoopNode.h \
	YACSPrs_BlocNode.h \
	YACSPrs_Link.h

# MOC-generated files
MOCSOURCES = \
	YACSPrs_Link_moc.cxx

# Resources
LIBPOFILES = \
	YACSPrs_msg_en.po \
	YACSPrs_images.po

LIBICONS = \
	no_status.png \
	ledgreen.png \
	ledred.png \
	disabled.png \
	running.png \
	waiting.png \
	aborted1.png \
	aborted2.png \
	aborted3.png \
	done.png \
	right_arrows.png \
	left_arrows.png

# Add "resources" subdirectory to resource file names
POFILES += $(LIBPOFILES:%=resources/%)
ICONS += $(LIBICONS:%=resources/%)

libYACSPrs_la_SOURCES = $(LIBSOURCES) $(LIBHEADERS)
nodist_libYACSPrs_la_SOURCES = $(MOCSOURCES)

# List all generated files here
BUILT_SOURCES = $(MOCSOURCES)

libYACSPrs_la_LDFLAGS = \
	 @GUI_LDFLAGS@ @PYTHON_LDFLAGS@ @QT_MT_LDFLAGS@

libYACSPrs_la_LIBADD = \
	../engine/libYACSEngine.la \
	../runtime/libYACSRuntimeSALOME.la \
	../hmi/libHMI.la \
	@QT_MT_LIBADD@ -lQxGraph

libYACSPrs_la_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(OMNIORB_INCLUDES) \
	-I$(KERNEL_ROOT_DIR)/include/salome \
	-I$(srcdir)/../bases \
	-I$(srcdir)/../engine \
	-I$(srcdir)/../runtime \
	-I$(srcdir)/../hmi \
	@QT_INCLUDES@ @GUI_CXXFLAGS@

include $(top_srcdir)/adm/unix/make_end.am
