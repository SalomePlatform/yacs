
libdir             = $(prefix)/lib/salome
bindir             = $(prefix)/bin/salome

lib_LTLIBRARIES = libGenericGui.la

BUILT_SOURCES =       \
  ui_FormComponent.h  \
  ui_FormContainer.h  \
  ui_FormEachLoop.h   \
  ui_FormEditItem.h   \
  ui_FormEditTree.h   \
  ui_FormLoop.h       \
  ui_FormSchemaView.h \
  ui_LogViewer.h      \
  ui_TablePorts.h     \
  ui_TableSwitch.h

libGenericGui_la_SOURCES =        \
  CatalogWidget.cxx               \
  EditionBloc.cxx                 \
  EditionBloc_moc.cxx             \
  EditionComponent.cxx            \
  EditionComponent_moc.cxx        \
  EditionContainer.cxx            \
  EditionContainer_moc.cxx        \
  EditionControlLink.cxx          \
  EditionControlLink_moc.cxx      \
  EditionDataLink.cxx             \
  EditionDataLink_moc.cxx         \
  EditionDataType.cxx             \
  EditionDataType_moc.cxx         \
  EditionElementaryNode.cxx       \
  EditionElementaryNode_moc.cxx   \
  EditionForEachLoop.cxx          \
  EditionForEachLoop_moc.cxx      \
  EditionInputPort.cxx            \
  EditionInputPort_moc.cxx        \
  EditionLoop.cxx                 \
  EditionLoop_moc.cxx             \
  EditionNode.cxx                 \
  EditionNode_moc.cxx             \
  EditionOutNode.cxx              \
  EditionOutNode_moc.cxx          \
  EditionOutputPort.cxx           \
  EditionOutputPort_moc.cxx       \
  EditionPresetNode.cxx           \
  EditionPresetNode_moc.cxx       \
  EditionProc.cxx                 \
  EditionProc_moc.cxx             \
  EditionPyFunc.cxx               \
  EditionPyFunc_moc.cxx           \
  EditionSalomeNode.cxx           \
  EditionSalomeNode_moc.cxx       \
  EditionScript.cxx               \
  EditionScript_moc.cxx           \
  EditionStudyInNode.cxx          \
  EditionStudyInNode_moc.cxx      \
  EditionStudyOutNode.cxx         \
  EditionStudyOutNode_moc.cxx     \
  EditionSwitch.cxx               \
  EditionSwitch_moc.cxx           \
  EditionWhile.cxx                \
  EditionWhile_moc.cxx            \
  FormComponent.cxx               \
  FormComponent_moc.cxx           \
  FormContainer.cxx               \
  FormContainer_moc.cxx           \
  FormEachLoop.cxx                \
  FormEachLoop_moc.cxx            \
  FormEditItem.cxx                \
  FormEditItem_moc.cxx            \
  FormEditTree.cxx                \
  FormEditTree_moc.cxx            \
  FormLoop.cxx                    \
  FormLoop_moc.cxx                \
  FormSchemaView.cxx              \
  FormSchemaView_moc.cxx          \
  GenericGui.cxx                  \
  GenericGui_moc.cxx              \
  GraphicsView.cxx                \
  GraphicsView_moc.cxx            \
  GuiEditor.cxx                   \
  GuiExecutor.cxx                 \
  GuiObserver_i.cxx               \
  ItemEdition.cxx                 \
  ItemEdition_moc.cxx             \
  ItemMimeData.cxx                \
  LinkAStar.cxx                   \
  LinkMatrix.cxx                  \
  LogViewer.cxx                   \
  LogViewer_moc.cxx               \
  Menus.cxx                       \
  Menus_moc.cxx                   \
  QtGuiContext.cxx                \
  SceneComposedNodeItem.cxx       \
  SceneCtrlInPortItem.cxx         \
  SceneCtrlLinkItem.cxx           \
  SceneCtrlOutPortItem.cxx        \
  SceneCtrlPortItem.cxx           \
  Scene.cxx                       \
  SceneDataPortItem.cxx           \
  SceneElementaryNodeItem.cxx     \
  SceneHeaderItem.cxx             \
  SceneHeaderNodeItem.cxx         \
  SceneInPortItem.cxx             \
  SceneItem.cxx                   \
  SceneLinkItem.cxx               \
  SceneNodeItem.cxx               \
  SceneObserverItem.cxx           \
  SceneOutPortItem.cxx            \
  ScenePortItem.cxx               \
  SceneProcItem.cxx               \
  SceneTextItem.cxx               \
  SchemaComponentItem.cxx         \
  SchemaComponentItem_moc.cxx     \
  SchemaComposedNodeItem.cxx      \
  SchemaComposedNodeItem_moc.cxx  \
  SchemaContainerItem.cxx         \
  SchemaContainerItem_moc.cxx     \
  SchemaDataTypeItem.cxx          \
  SchemaDataTypeItem_moc.cxx      \
  SchemaDirContainersItem.cxx     \
  SchemaDirContainersItem_moc.cxx \
  SchemaDirLinksItem.cxx          \
  SchemaDirLinksItem_moc.cxx      \
  SchemaDirTypesItem.cxx          \
  SchemaDirTypesItem_moc.cxx      \
  SchemaInPortItem.cxx            \
  SchemaInPortItem_moc.cxx        \
  SchemaItem.cxx                  \
  SchemaItem_moc.cxx              \
  SchemaLinkItem.cxx              \
  SchemaLinkItem_moc.cxx          \
  SchemaModel.cxx                 \
  SchemaModel_moc.cxx             \
  SchemaNodeItem.cxx              \
  SchemaNodeItem_moc.cxx          \
  SchemaOutPortItem.cxx           \
  SchemaOutPortItem_moc.cxx       \
  SchemaProcItem.cxx              \
  SchemaProcItem_moc.cxx          \
  SchemaReferenceItem.cxx         \
  SchemaReferenceItem_moc.cxx     \
  TablePortsEdition.cxx           \
  TablePortsEdition_moc.cxx       \
  TableSwitch.cxx                 \
  TableSwitch_moc.cxx             \
  TreeView.cxx                    \
  TreeView_moc.cxx                \
  ValueDelegate.cxx               \
  ValueDelegate_moc.cxx           \
  VisitorSaveGuiSchema.cxx        \
  WrapGraphicsView_moc.cxx


libGenericGui_la_CXXFLAGS  = \
	$(qt4_cppflags) \
	$(qsci4_cppflags) \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(OMNIORB_INCLUDES) \
	$(LIBXML_INCLUDES) \
	-I$(KERNEL_ROOT_DIR)/include/salome \
	-I$(srcdir)/../bases \
	-I$(srcdir)/../engine \
	-I$(srcdir)/../runtime \
	-I$(srcdir)/../yacsloader \
	-I$(srcdir)/../hmi \
	-I../../idl \
	-I../yacsorb 

libGenericGui_la_LDFLAGS   = $(qt4_ldflags)

libGenericGui_la_LIBADD    = $(qt4_libs) $(qsci4_libs)\
	../runtime/libYACSRuntimeSALOME.la \
	../yacsloader/libYACSloader.la \
	../salomewrap/libSalomeWrap.la \
	../hmi/libHMI.la \
	$(EXPAT_LIBS) $(LIBXML_LIBS)



LIBICONS = \
	add_in_study.png \
	add_node.png \
	arrange_nodes.png \
	block_node.png \
	breakpoints_active.png \
	breakpoints.png \
	change_informations.png \
	component.png \
	connect_to_batch_session.png \
	container.png \
	control_link.png \
	control_view.png \
	dataflow_view.png \
	data_link.png \
	datastream_view.png \
	execute_in_batch.png \
	export_dataflow.png \
	filter_next_steps.png \
	filter_notification.png \
	folder_cyan.png \
	folder_cyan_open.png \
	full_view.png \
	icon_down.png \
	icon_insert.png \
	icon_minus.png \
	icon_plus.png \
	icon_select.png \
	icon_select.png \
	icon_text.png \
	icon_up.png \
	import_dataflow.png \
	import_superv_dataflow.png \
	in_port.png \
	insert_file.png \
	kill.png \
	load_execution_state.png \
	loop_node.png \
	mode_continue.png \
	modify_dataflow.png \
	modify_superv_dataflow.png \
	ModuleYacs.png \
	new_batch_execution.png \
	new_block_node.png \
	new_corba_component.png \
	new_corba_service_node.png \
	new_cpp_node.png \
	new_dataflow.png \
	new_edition.png \
	new_execution.png \
	new_foreach_loop_node.png \
	new_for_loop_node.png \
	new_from_library_node.png \
	new_inline_function_node.png \
	new_inline_script_node.png \
	new_link.png \
	new_nodenode_service_node.png \
	new_salome_component.png \
	new_salomepy_component.png \
	new_salome_service_node.png \
	new_service_inline_node.png \
	new_switch_loop_node.png \
	new_while_loop_node.png \
	new_xml_node.png \
	node.png \
	ob_service_node.png \
	out_port.png \
	pause.png \
	rebuild_links.png \
	reload.png \
	remote_run.png \
	reset.png \
	resume.png \
	run_active.png \
	run_object.png \
	run.png \
	sample.png \
	save_dataflow.png \
	save_dataflow_state.png \
	schema.png \
	step_by_step_active.png \
	step_by_step.png \
	stream_link.png \
	suspend_resume.png \
	switch_node.png \
	table_view.png \
	toggle_stop_on_error.png

salomeresdir = $(prefix)/share/salome/resources/@MODULE_NAME@
dist_salomeres_DATA = $(QMFILES) ${ICONS}
ICONS = $(LIBICONS:%=resources/%)


ui_%.h: %.ui
	$(QT_UIC) $< -o $@

# --------------------------------------------
# *.h --> *_moc.cxx
# --------------------------------------------

SUFFIXES = .hxx _moc.cxx .qrc _qrc.cxx

.hxx_moc.cxx :
	$(QT_MOC) -p . -o $@ $<

.qrc_qrc.cxx :
	$(QT_RCC) -name $(*F)  $< -o $@

#.ui.h :
#	$(QT_UIC) -o $@ $<


clean-local-qt :
	rm -f *_moc.cxx *_qrc.cxx


clean-local: clean-local-qt