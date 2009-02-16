#  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
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
  ui_TableSwitch.h    \
  ui_CaseSwitch.h

EXTRA_DIST =        \
  FormComponent.ui  \
  FormContainer.ui  \
  FormEachLoop.ui   \
  FormEditItem.ui   \
  FormEditTree.ui   \
  FormLoop.ui       \
  FormSchemaView.ui \
  LogViewer.ui      \
  TablePorts.ui     \
  TableSwitch.ui    \
  CaseSwitch.ui

libGenericGui_la_SOURCES =        \
  CaseSwitch.hxx                  \
  CaseSwitch.cxx                  \
  CatalogWidget.hxx               \
  CatalogWidget.cxx               \
  EditionBloc.hxx                 \
  EditionBloc.cxx                 \
  EditionComponent.hxx            \
  EditionComponent.cxx            \
  EditionContainer.hxx            \
  EditionContainer.cxx            \
  EditionControlLink.hxx          \
  EditionControlLink.cxx          \
  EditionDataLink.hxx             \
  EditionDataLink.cxx             \
  EditionDataType.hxx             \
  EditionDataType.cxx             \
  EditionElementaryNode.hxx       \
  EditionElementaryNode.cxx       \
  EditionForEachLoop.hxx          \
  EditionForEachLoop.cxx          \
  EditionInputPort.hxx            \
  EditionInputPort.cxx            \
  EditionLoop.hxx                 \
  EditionLoop.cxx                 \
  EditionNode.hxx                 \
  EditionNode.cxx                 \
  EditionOutNode.hxx              \
  EditionOutNode.cxx              \
  EditionOutputPort.hxx           \
  EditionOutputPort.cxx           \
  EditionPresetNode.hxx           \
  EditionPresetNode.cxx           \
  EditionProc.hxx                 \
  EditionProc.cxx                 \
  EditionPyFunc.hxx               \
  EditionPyFunc.cxx               \
  EditionSalomeNode.hxx           \
  EditionSalomeNode.cxx           \
  EditionScript.hxx               \
  EditionScript.cxx               \
  EditionStudyInNode.hxx          \
  EditionStudyInNode.cxx          \
  EditionStudyOutNode.hxx         \
  EditionStudyOutNode.cxx         \
  EditionSwitch.hxx               \
  EditionSwitch.cxx               \
  EditionWhile.hxx                \
  EditionWhile.cxx                \
  FormComponent.hxx               \
  FormComponent.cxx               \
  FormContainer.hxx               \
  FormContainer.cxx               \
  FormEachLoop.hxx                \
  FormEachLoop.cxx                \
  FormEditItem.hxx                \
  FormEditItem.cxx                \
  FormEditTree.hxx                \
  FormEditTree.cxx                \
  FormLoop.hxx                    \
  FormLoop.cxx                    \
  FormSchemaView.hxx              \
  FormSchemaView.cxx              \
  GenericGui.hxx                  \
  GenericGui.cxx                  \
  GraphicsView.hxx                \
  GraphicsView.cxx                \
  GuiEditor.hxx                   \
  GuiEditor.cxx                   \
  GuiExecutor.hxx                 \
  GuiExecutor.cxx                 \
  GuiObserver_i.hxx               \
  GuiObserver_i.cxx               \
  ItemEdition.hxx                 \
  ItemEdition.cxx                 \
  ItemMimeData.hxx                \
  ItemMimeData.cxx                \
  LinkAStar.hxx                   \
  LinkAStar.cxx                   \
  LinkMatrix.hxx                  \
  LinkMatrix.cxx                  \
  LogViewer.hxx                   \
  LogViewer.cxx                   \
  Menus.hxx                       \
  Menus.cxx                       \
  Message.hxx                     \
  Message.cxx                     \
  QtGuiContext.hxx                \
  QtGuiContext.cxx                \
  SceneComposedNodeItem.hxx       \
  SceneComposedNodeItem.cxx       \
  SceneCtrlInPortItem.hxx         \
  SceneCtrlInPortItem.cxx         \
  SceneCtrlLinkItem.hxx           \
  SceneCtrlLinkItem.cxx           \
  SceneCtrlOutPortItem.hxx        \
  SceneCtrlOutPortItem.cxx        \
  SceneCtrlPortItem.hxx           \
  SceneCtrlPortItem.cxx           \
  Scene.cxx                       \
  Scene.hxx                       \
  SceneBlocItem.hxx               \
  SceneBlocItem.cxx               \
  SceneDataPortItem.hxx           \
  SceneDataPortItem.cxx           \
  SceneElementaryNodeItem.hxx     \
  SceneElementaryNodeItem.cxx     \
  SceneHeaderItem.hxx             \
  SceneHeaderItem.cxx             \
  SceneHeaderNodeItem.hxx         \
  SceneHeaderNodeItem.cxx         \
  SceneInPortItem.hxx             \
  SceneInPortItem.cxx             \
  SceneItem.hxx                   \
  SceneItem.cxx                   \
  SceneLinkItem.hxx               \
  SceneLinkItem.cxx               \
  SceneNodeItem.hxx               \
  SceneNodeItem.cxx               \
  SceneObserverItem.hxx           \
  SceneObserverItem.cxx           \
  SceneOutPortItem.hxx            \
  SceneOutPortItem.cxx            \
  ScenePortItem.hxx               \
  ScenePortItem.cxx               \
  SceneProcItem.hxx               \
  SceneProcItem.cxx               \
  SceneTextItem.hxx               \
  SceneTextItem.cxx               \
  SchemaComponentItem.hxx         \
  SchemaComponentItem.cxx         \
  SchemaComposedNodeItem.hxx      \
  SchemaComposedNodeItem.cxx      \
  SchemaContainerItem.hxx         \
  SchemaContainerItem.cxx         \
  SchemaDataTypeItem.hxx          \
  SchemaDataTypeItem.cxx          \
  SchemaDirContainersItem.hxx     \
  SchemaDirContainersItem.cxx     \
  SchemaDirLinksItem.hxx          \
  SchemaDirLinksItem.cxx          \
  SchemaDirTypesItem.hxx          \
  SchemaDirTypesItem.cxx          \
  SchemaInPortItem.hxx            \
  SchemaInPortItem.cxx            \
  SchemaItem.hxx                  \
  SchemaItem.cxx                  \
  SchemaLinkItem.hxx              \
  SchemaLinkItem.cxx              \
  SchemaModel.hxx                 \
  SchemaModel.cxx                 \
  SchemaNodeItem.hxx              \
  SchemaNodeItem.cxx              \
  SchemaOutPortItem.hxx           \
  SchemaOutPortItem.cxx           \
  SchemaProcItem.hxx              \
  SchemaProcItem.cxx              \
  SchemaReferenceItem.hxx         \
  SchemaReferenceItem.cxx         \
  TablePortsEdition.hxx           \
  TablePortsEdition.cxx           \
  TableSwitch.hxx                 \
  TableSwitch.cxx                 \
  TreeView.hxx                    \
  TreeView.cxx                    \
  ValueDelegate.hxx               \
  ValueDelegate.cxx               \
  VisitorSaveGuiSchema.hxx        \
  VisitorSaveGuiSchema.cxx        \
  YACSGuiLoader.hxx               \
  YACSGuiLoader.cxx               \
  SuitWrapper.hxx                 \
  WrapGraphicsView.hxx

nodist_libGenericGui_la_SOURCES = \
  CaseSwitch_moc.cxx              \
  EditionBloc_moc.cxx             \
  EditionComponent_moc.cxx        \
  EditionContainer_moc.cxx        \
  EditionControlLink_moc.cxx      \
  EditionDataLink_moc.cxx         \
  EditionDataType_moc.cxx         \
  EditionElementaryNode_moc.cxx   \
  EditionForEachLoop_moc.cxx      \
  EditionInputPort_moc.cxx        \
  EditionLoop_moc.cxx             \
  EditionNode_moc.cxx             \
  EditionOutNode_moc.cxx          \
  EditionOutputPort_moc.cxx       \
  EditionPresetNode_moc.cxx       \
  EditionProc_moc.cxx             \
  EditionPyFunc_moc.cxx           \
  EditionSalomeNode_moc.cxx       \
  EditionScript_moc.cxx           \
  EditionStudyInNode_moc.cxx      \
  EditionStudyOutNode_moc.cxx     \
  EditionSwitch_moc.cxx           \
  EditionWhile_moc.cxx            \
  FormComponent_moc.cxx           \
  FormContainer_moc.cxx           \
  FormEachLoop_moc.cxx            \
  FormEditItem_moc.cxx            \
  FormEditTree_moc.cxx            \
  FormLoop_moc.cxx                \
  FormSchemaView_moc.cxx          \
  GenericGui_moc.cxx              \
  GraphicsView_moc.cxx            \
  ItemEdition_moc.cxx             \
  LogViewer_moc.cxx               \
  Menus_moc.cxx                   \
  SchemaComponentItem_moc.cxx     \
  SchemaComposedNodeItem_moc.cxx  \
  SchemaContainerItem_moc.cxx     \
  SchemaDataTypeItem_moc.cxx      \
  SchemaDirContainersItem_moc.cxx \
  SchemaDirLinksItem_moc.cxx      \
  SchemaDirTypesItem_moc.cxx      \
  SchemaInPortItem_moc.cxx        \
  SchemaItem_moc.cxx              \
  SchemaLinkItem_moc.cxx          \
  SchemaModel_moc.cxx             \
  SchemaNodeItem_moc.cxx          \
  SchemaOutPortItem_moc.cxx       \
  SchemaProcItem_moc.cxx          \
  SchemaReferenceItem_moc.cxx     \
  TablePortsEdition_moc.cxx       \
  TableSwitch_moc.cxx             \
  TreeView_moc.cxx                \
  ValueDelegate_moc.cxx           \
  WrapGraphicsView_moc.cxx

libGenericGui_la_CXXFLAGS  = \
	$(qt4_cppflags) \
	$(qsci4_cppflags) \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(GRAPHVIZ_CPPFLAGS) \
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

libGenericGui_la_LDFLAGS   = $(qt4_ldflags) $(qsci4_ldflags) $(GRAPHVIZ_LDFLAGS)

libGenericGui_la_LIBADD    = $(qt4_libs) $(qsci4_libs)\
	../runtime/libYACSRuntimeSALOME.la \
	../yacsloader/libYACSloader.la \
	../salomewrap/libSalomeWrap.la \
	../hmi/libHMI.la \
	$(EXPAT_LIBS) $(LIBXML_LIBS) $(GRAPHVIZ_LIBADD)



LIBICONS = \
	add_in_study.png \
	add_node.png \
	arrange_nodes.png \
	autoComputeLink.png \
	block_node.png \
	breakpoints_active.png \
	breakpoints.png \
	change_informations.png \
	component.png \
	connect_to_batch_session.png \
	container.png \
	control_link.png \
	control_view.png \
	copy.png \
	cut.png \
	dataflow_view.png \
	data_link.png \
	datastream_view.png \
	delete.png \
	execute_in_batch.png \
	export_dataflow.png \
	filter_next_steps.png \
	filter_notification.png \
	folder_cyan.png \
	folder_cyan_open.png \
	force2nodeLink.png \
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
	paste.png \
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
	simplifyLink.png \
	step_by_step_active.png \
	step_by_step.png \
	stream_link.png \
	suspend_resume.png \
	switch_node.png \
	table_view.png \
	toggle_stop_on_error.png \
	toggleVisibility.png \
	whatsThis.png

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
	rm -f *_moc.cxx *_qrc.cxx ui_*.h


clean-local: clean-local-qt