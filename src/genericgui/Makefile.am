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

UIC_FILES =       \
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

BUILT_SOURCES =       \
  $(UIC_FILES)

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
  GenericGuiExport.hxx            \
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
  EditionOptimizerLoop.hxx        \
  EditionOptimizerLoop.cxx        \
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
  PropertyEditor.hxx              \
  PropertyEditor.cxx              \
  QtGuiContext.hxx                \
  QtGuiContext.cxx                \
  Resource.hxx                    \
  Resource.cxx                    \
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
  SceneDSLinkItem.hxx             \
  SceneDSLinkItem.cxx             \
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

MOC_FILES = \
  CaseSwitch_moc.cxx              \
  EditionBloc_moc.cxx             \
  EditionComponent_moc.cxx        \
  EditionContainer_moc.cxx        \
  EditionControlLink_moc.cxx      \
  EditionDataLink_moc.cxx         \
  EditionDataType_moc.cxx         \
  EditionElementaryNode_moc.cxx   \
  EditionForEachLoop_moc.cxx      \
  EditionOptimizerLoop_moc.cxx    \
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
  PropertyEditor_moc.cxx          \
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

nodist_libGenericGui_la_SOURCES = \
  $(MOC_FILES)

libGenericGui_la_CXXFLAGS  = \
	$(qt4_cppflags) \
	$(qsci4_cppflags) \
	$(THREAD_DEF) \
	$(PYTHON_CPPFLAGS) \
	$(EXPAT_INCLUDES) \
	$(GRAPHVIZ_CPPFLAGS) \
	$(OMNIORB_CXXFLAGS) \
	$(OMNIORB_INCLUDES) \
	$(LIBXML_INCLUDES) \
	$(EXPAT_INCLUDES) \
	-I$(KERNEL_ROOT_DIR)/include/salome \
	-I$(builddir) \
	-I$(srcdir) \
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
	$(KERNEL_LDFLAGS) -lSalomeResourcesManager \
	$(EXPAT_LIBS) $(LIBXML_LIBS) $(GRAPHVIZ_LIBADD)



ICONS = \
	resources/add_in_study.png \
	resources/add_node.png \
	resources/addRowCols.png \
	resources/arrange_nodes.png \
	resources/autoComputeLink.png \
	resources/block_node.png \
	resources/breakpoints_active.png \
	resources/breakpoints.png \
	resources/centerOnNode.png \
	resources/change_informations.png \
	resources/component.png \
	resources/connect_to_batch_session.png \
	resources/container.png \
	resources/control_link.png \
	resources/control_view.png \
	resources/copy.png \
	resources/cut.png \
	resources/dataflow_view.png \
	resources/data_link.png \
	resources/datastream_view.png \
	resources/delete.png \
	resources/emphasisLink.png \
	resources/execute_in_batch.png \
	resources/export_dataflow.png \
	resources/filter_next_steps.png \
	resources/filter_notification.png \
	resources/folder_cyan.png \
	resources/folder_cyan_open.png \
	resources/force2nodeLink.png \
	resources/full_view.png \
	resources/hideLink.png \
	resources/icon_down.png \
	resources/icon_insert.png \
	resources/icon_minus.png \
	resources/icon_plus.png \
	resources/icon_select.png \
	resources/icon_text.png \
	resources/icon_up.png \
	resources/import_dataflow.png \
	resources/import_superv_dataflow.png \
	resources/in_port.png \
	resources/insert_file.png \
	resources/kill.png \
	resources/load_execution_state.png \
	resources/loop_node.png \
	resources/mode_continue.png \
	resources/modify_dataflow.png \
	resources/modify_superv_dataflow.png \
	resources/ModuleYacs.png \
	resources/new_batch_execution.png \
	resources/new_block_node.png \
	resources/new_corba_component.png \
	resources/new_corba_service_node.png \
	resources/new_cpp_node.png \
	resources/new_dataflow.png \
	resources/new_edition.png \
	resources/new_execution.png \
	resources/new_foreach_loop_node.png \
	resources/new_for_loop_node.png \
	resources/new_from_library_node.png \
	resources/new_inline_function_node.png \
	resources/new_inline_script_node.png \
	resources/new_link.png \
	resources/new_nodenode_service_node.png \
	resources/new_salome_component.png \
	resources/new_salomepy_component.png \
	resources/new_salome_service_node.png \
	resources/new_service_inline_node.png \
	resources/new_switch_loop_node.png \
	resources/new_while_loop_node.png \
	resources/new_xml_node.png \
	resources/node.png \
	resources/ob_service_node.png \
	resources/out_port.png \
	resources/paste.png \
	resources/pause.png \
	resources/rebuild_links.png \
	resources/reload.png \
	resources/remote_run.png \
	resources/reset.png \
	resources/resume.png \
	resources/run_active.png \
	resources/run_object.png \
	resources/run.png \
	resources/sample.png \
	resources/save_dataflow.png \
	resources/save_dataflow_state.png \
	resources/schema.png \
	resources/showLink.png \
	resources/simplifyLink.png \
	resources/step_by_step_active.png \
	resources/step_by_step.png \
	resources/stream_link.png \
	resources/suspend_resume.png \
	resources/switch_node.png \
	resources/table_view.png \
	resources/toggle_stop_on_error.png \
	resources/toggleVisibility.png \
	resources/whatsThis.png \
	resources/zoomToBloc.png

salomeresdir = $(prefix)/share/salome/resources/@MODULE_NAME@
dist_salomeres_DATA = $(QMFILES) ${ICONS}


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
