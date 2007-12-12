
include $(top_srcdir)/adm/unix/make_begin.am

lib_LTLIBRARIES = libYACSGui.la

# Implementation files
LIBSOURCES = \
	YACSGui_Node.cxx \
	YACSGui_Graph.cxx \
	YACSGui_Graph_1.cxx \
	YACSGui_Module.cxx \
	YACSGui_DataModel.cxx \
	YACSGui_DataObject.cxx \
	YACSGui_TreeView.cxx \
	YACSGui_TreeViewItem.cxx \
	YACSGui_Executor.cxx \
	YACSGui_Observer.cxx \
	YACSGui_XMLDriver.cxx \
	YACSGui_Swig.cxx \
	YACSGui_RunMode.cxx \
	YACSGui_InputPanel.cxx \
	YACSGui_Selection.cxx \
	YACSGui_PlusMinusGrp.cxx \
	YACSGui_Table.cxx


# Headers
LIBHEADERS = \
	YACSGui_Node.h \
	YACSGui_Graph.h \
	YACSGui_Module.h \
	YACSGui_DataModel.h \
	YACSGui_DataObject.h \
	YACSGui_TreeView.h \
	YACSGui_TreeViewItem.h \
	YACSGui_Executor.h \
	YACSGui_Observer.h \
	YACSGui_XMLDriver.h \
	YACSGui_Swig.h \
	YACSGui_Selection.h \
	YACSGui_PlusMinusGrp.h \
	YACSGui_Table.h


# MOC-generated files
MOCSOURCES = \
	YACSGui_Module_moc.cxx \
	YACSGui_Executor_moc.cxx \
	runmode_moc.cxx \
	YACSGui_RunMode_moc.cxx \
	YACSGui_InputPanel_moc.cxx \
	YACSGui_TreeView_moc.cxx \
	YACSGui_PlusMinusGrp_moc.cxx \
	YACSGui_Table_moc.cxx \
	schemapage_moc.cxx \
	containerpage_moc.cxx \
	componentpage_moc.cxx \
	inlinenodepage_moc.cxx \
	servicenodepage_moc.cxx \
	forloopnodepage_moc.cxx \
	foreachloopnodepage_moc.cxx \
	whileloopnodepage_moc.cxx \
	switchnodepage_moc.cxx \
	blocknodepage_moc.cxx

# UIC-generated files
runmode.cxx : runmode.h
schemapage.cxx : schemapage.h
containerpage.cxx : containerpage.h
componentpage.cxx : componentpage.h
inlinenodepage.cxx : inlinenodepage.h
servicenodepage.cxx : servicenodepage.h
forloopnodepage.cxx : forloopnodepage.h
foreachloopnodepage.cxx : foreachloopnodepage.h
whileloopnodepage.cxx : whileloopnodepage.h
switchnodepage.cxx : switchnodepage.h
blocknodepage.cxx : blocknodepage.h
UISOURCES = runmode.cxx schemapage.cxx containerpage.cxx componentpage.cxx inlinenodepage.cxx servicenodepage.cxx \
            forloopnodepage.cxx foreachloopnodepage.cxx whileloopnodepage.cxx switchnodepage.cxx blocknodepage.cxx

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
	dataflow_view.png \
	datastream_view.png \
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
	arrange_nodes.png \
	reload.png \
	remote_run.png \
	run.png \
	save_dataflow_state.png \
	step_by_step.png \
	suspend_resume.png \
	table_view.png \
	toggle_stop_on_error.png \
	schema.png \
	block_node.png \
	loop_node.png \
	switch_node.png \
	node.png \
	out_port.png \
	in_port.png \
	control_link.png \
	data_link.png \
	stream_link.png \
	container.png \
	component.png \
	run_object.png \
	new_salome_component.png \
	new_salomepy_component.png \
	new_corba_component.png \
	new_salome_service_node.png \
	new_corba_service_node.png \
	new_nodenode_service_node.png \
	new_cpp_node.png \
	new_service_inline_node.png \
	new_xml_node.png \
	new_inline_script_node.png \
	new_inline_function_node.png \
	new_block_node.png \
	new_for_loop_node.png \
	new_foreach_loop_node.png \
	new_while_loop_node.png \
	new_switch_loop_node.png \
	new_from_library_node.png \
	new_link.png \
	icon_select.png \
	icon_up.png \
	icon_down.png \
	icon_insert.png \
	icon_select.png \
	icon_plus.png \
	icon_minus.png \
	new_execution.png \
	new_batch_execution.png \
	load_execution_state.png \
	execute_in_batch.png \
	pause.png \
	resume.png \
	reset.png \
	connect_to_batch_session.png

# Add "resources" subdirectory to resource file names
POFILES += $(LIBPOFILES:%=resources/%)
ICONS += $(LIBICONS:%=resources/%)

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

libYACSGui_la_LDFLAGS = \
	 @GRAPHVIZ_LDFLAGS@ \
	 @GUI_LDFLAGS@ \
	 @QT_MT_LDFLAGS@

libYACSGui_la_LIBADD = \
	../runtime/libYACSRuntimeSALOME.la \
	../yacsloader/libYACSloader.la \
	../prs/libYACSPrs.la \
	../lineconn2d/libLineConn2d.la \
	../hmi/libHMI.la \
	../yacsorb/libYACSorb.la \
	@GRAPHVIZ_LIBADD@
	@QT_MT_LIBADD@\
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
	-I$(srcdir)/../hmi \
	-I../../idl \
	-I../yacsorb \
	@QT_INCLUDES@ @GUI_CXXFLAGS@ @CAS_CPPFLAGS@ @GRAPHVIZ_CPPFLAGS@

include $(top_srcdir)/adm/unix/make_end.am
