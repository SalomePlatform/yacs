
include $(top_srcdir)/adm/unix/make_begin.am

lib_LTLIBRARIES = libYACSGui.la

# Implementation files
LIBSOURCES = \
	YACSGui_Node.cxx \
	YACSGui_Graph.cxx \
	YACSGui_Module.cxx \
	YACSGui_DataModel.cxx \
	YACSGui_DataObject.cxx \
	YACSGui_Executor.cxx \
	YACSGui_Observer.cxx \
	YACSGui_XMLDriver.cxx \
	YACSGui_Swig.cxx \
	YACSGui_RunMode.cxx

# Headers
LIBHEADERS = \
	YACSGui_Node.h \
	YACSGui_Graph.h \
	YACSGui_Module.h \
	YACSGui_DataModel.h \
	YACSGui_DataObject.h \
	YACSGui_Executor.h \
	YACSGui_Observer.h \
	YACSGui_XMLDriver.h \
	YACSGui_Swig.h

# MOC-generated files
MOCSOURCES = \
	YACSGui_Module_moc.cxx \
	YACSGui_Executor_moc.cxx \
	runmode_moc.cxx \
	YACSGui_RunMode_moc.cxx

# UIC-generated files
runmode.cxx : runmode.h
UISOURCES = runmode.cxx

# Resources
LIBPOFILES = \
	YACSGui_msg_en.po \
	YACSGui_images.po

LIBICONS = \
	sample.png \
	ModuleYacs.png \
	SalomeApp.xml \
	YACSGuiCatalog.xml \
	add_in_study.png \
	add_node.png \
	change_informations.png \
	control_view.png \
	export_dataflow.png \
	filter_next_steps.png \
	filter_notification.png \
	full_view.png \
	import_dataflow.png \
	import_superv_dataflow.png \
	insert_file.png \
	kill.png \
	mode_breakpoint.png \
	mode_continue.png \
	modify_dataflow.png \
	modify_superv_dataflow.png \
	new_dataflow.png \
	rebuild_links.png \
	reload.png \
	remote_run.png \
	run.png \
	save_dataflow_state.png \
	step_by_step.png \
	suspend_resume.png \
	table_view.png \
	toggle_stop_on_error.png

# Add "resources" subdirectory to resource file names
POFILES = $(LIBPOFILES:%=resources/%)
ICONS = $(LIBICONS:%=resources/%)

libYACSGui_la_SOURCES = $(LIBSOURCES) $(LIBHEADERS)
nodist_libYACSGui_la_SOURCES = $(UISOURCES) $(MOCSOURCES)

# List all generated files here
BUILT_SOURCES = $(UISOURCES) $(MOCSOURCES) YACSGui_SwigWRAP.cxx

MYSWIG_FLAGS = -noexcept -I$(srcdir)/../bases

pkgpython_PYTHON = YACSGui_Swig.py \
    yacsguiloader.py
pkgpyexec_LTLIBRARIES = _YACSGui_Swig.la

YACSGui_SwigWRAP.cxx:YACSGui_Swig.i YACSGui_Swig.h
	$(SWIG) $(SWIG_PYTHON_OPT) $(SWIG_PYTHON_INCLUDES) $(MYSWIG_FLAGS) -o $@ $<

_YACSGui_Swig_la_SOURCES =   \
	YACSGui_SwigWRAP.cxx

_YACSGui_Swig_la_CXXFLAGS = \
	-I$(srcdir)/../bases \
	-I$(srcdir)/../engine \
	$(PYTHON_CPPFLAGS)

_YACSGui_Swig_la_LDFLAGS  = -module

_YACSGui_Swig_la_LIBADD = libYACSGui.la

libYACSGui_la_LIBADD = \
	../runtime/libYACSRuntimeSALOME.la \
	../yacsloader/libYACSloader.la \
	../prs/libYACSPrs.la \
	../lineconn2d/libLineConn2d.la \
	../yacsorb/libYACSorb.la \
	@QT_MT_LIBS@ @GUI_LDFLAGS@ \
	-lQxGraph -lCAM -lsuit -lLightApp -lexpat -lxml2

libYACSGui_la_CXXFLAGS = \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	-I$(KERNEL_ROOT_DIR)/include/salome \
	-I$(srcdir)/../bases \
	-I$(srcdir)/../engine \
	-I$(srcdir)/../runtime \
	-I$(srcdir)/../yacsloader \
	-I$(srcdir)/../prs \
	-I$(srcdir)/../lineconn2d \
	-I../../idl \
	-I../yacsorb \
	@QT_INCLUDES@ @GUI_CXXFLAGS@ @CAS_CPPFLAGS@

include $(top_srcdir)/adm/unix/make_end.am
