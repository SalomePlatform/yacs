# Copyright (C) 2012-2016  CEA/DEN, EDF R&D
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

INCLUDE(UseQtExt)

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${PYTHON_INCLUDE_DIR}
  ${OMNIORB_INCLUDE_DIR}
  ${LIBXML2_INCLUDE_DIR}
  ${GRAPHVIZ_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${KERNEL_INCLUDE_DIRS}
  ${GUI_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}/src/genericgui
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/src/bases
  ${PROJECT_SOURCE_DIR}/src/engine
  ${PROJECT_SOURCE_DIR}/src/runtime
  ${PROJECT_SOURCE_DIR}/src/yacsloader
  ${PROJECT_SOURCE_DIR}/src/hmi
  ${PROJECT_SOURCE_DIR}/src/salomewrap
  ${PROJECT_SOURCE_DIR}/src/genericgui
  ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_BINARY_DIR}/src/yacsorb
  )

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${QT_DEFINITIONS}
  ${PYTHON_DEFINITIONS}
  ${GRAPHVIZ_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  ${KERNEL_DEFINITIONS}
  ${GUI_DEFINITIONS}
  ${GRAPHVIZ_DEFINITIONS}
  )

# libraries to link to
SET(_link_LIBRARIES
  ${QT_LIBRARIES}
  YACSRuntimeSALOME
  YACSloader
  SalomeWrap
  HMI
  SalomeResourcesManager
  ${LIBXML2_LIBRARIES}
  ${GRAPHVIZ_LIBRARIES}
  )

# --- resources ---

# resource files / to be processed by uic
SET(_uic_files
  FormComponent.ui
  FormContainer.ui
  FormAdvParamContainer.ui
  FormParamContainer.ui
  FormHPContainer.ui
  FormEachLoop.ui
  FormEditItem.ui
  FormEditTree.ui
  FormLoop.ui
  FormSchemaView.ui
  FormUndoRedo.ui
  LogViewer.ui
  TablePorts.ui
  TableSwitch.ui
  CaseSwitch.ui
  FormOptimizerLoop.ui
  )

# --- headers ---

SET(GenericGui_HEADERS
  )

# header files / to be processed by moc
SET(_moc_HEADERS
  CaseSwitch.hxx
  EditionBloc.hxx
  EditionComponent.hxx
  EditionContainer.hxx
  EditionControlLink.hxx
  EditionDataLink.hxx
  EditionDataType.hxx
  EditionElementaryNode.hxx
  EditionForEachLoop.hxx
  EditionOptimizerLoop.hxx
  EditionInputPort.hxx
  EditionLoop.hxx
  EditionNode.hxx
  EditionOutNode.hxx
  EditionOutputPort.hxx
  EditionPresetNode.hxx
  EditionProc.hxx
  EditionPyFunc.hxx
  EditionSalomeNode.hxx
  EditionScript.hxx
  EditionStudyInNode.hxx
  EditionStudyOutNode.hxx
  EditionSwitch.hxx
  EditionWhile.hxx
  FormComponent.hxx
  FormContainerBase.hxx
  FormContainer.hxx
  FormHPContainer.hxx
  FormAdvParamContainer.hxx
  FormContainerDecorator.hxx
  FormEachLoop.hxx
  FormEditItem.hxx
  FormEditTree.hxx
  FormLoop.hxx
  FormOptimizerLoop.hxx
  FormSchemaView.hxx
  FormUndoRedo.hxx
  GenericGui.hxx
  GraphicsView.hxx
  ItemEdition.hxx
  ListJobs_GUI.hxx
  LogViewer.hxx
  Menus.hxx
  PropertyEditor.hxx
  SchemaComponentItem.hxx
  SchemaComposedNodeItem.hxx
  SchemaContainerItem.hxx
  SchemaDataTypeItem.hxx
  SchemaDirContainersItem.hxx
  SchemaDirLinksItem.hxx
  SchemaDirTypesItem.hxx
  SchemaInPortItem.hxx
  SchemaItem.hxx
  SchemaLinkItem.hxx
  SchemaModel.hxx
  SchemaNodeItem.hxx
  SchemaOutPortItem.hxx
  SchemaProcItem.hxx
  SchemaReferenceItem.hxx
  TablePortsEdition.hxx
  TableSwitch.hxx
  TreeView.hxx
  YACSWidgets.hxx
  ValueDelegate.hxx
  )

# header files / uic wrappings
QT_WRAP_UIC(_uic_HEADERS ${_uic_files})

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

SET(GenericGui_SOURCES
  GenericGuiExport.hxx
  CaseSwitch.hxx
  CaseSwitch.cxx
  CatalogWidget.hxx
  CatalogWidget.cxx
  EditionBloc.hxx
  EditionBloc.cxx
  EditionComponent.hxx
  EditionComponent.cxx            
  EditionContainer.hxx            
  EditionContainer.cxx
  EditionControlLink.hxx          
  EditionControlLink.cxx          
  EditionDataLink.hxx             
  EditionDataLink.cxx             
  EditionDataType.hxx             
  EditionDataType.cxx             
  EditionElementaryNode.hxx       
  EditionElementaryNode.cxx       
  EditionForEachLoop.hxx          
  EditionForEachLoop.cxx          
  EditionOptimizerLoop.hxx        
  EditionOptimizerLoop.cxx        
  EditionInputPort.hxx            
  EditionInputPort.cxx            
  EditionLoop.hxx                 
  EditionLoop.cxx                 
  EditionNode.hxx                 
  EditionNode.cxx                 
  EditionOutNode.hxx              
  EditionOutNode.cxx              
  EditionOutputPort.hxx           
  EditionOutputPort.cxx           
  EditionPresetNode.hxx           
  EditionPresetNode.cxx           
  EditionProc.hxx                 
  EditionProc.cxx                 
  EditionPyFunc.hxx               
  EditionPyFunc.cxx               
  EditionSalomeNode.hxx           
  EditionSalomeNode.cxx           
  EditionScript.hxx               
  EditionScript.cxx               
  EditionStudyInNode.hxx          
  EditionStudyInNode.cxx          
  EditionStudyOutNode.hxx         
  EditionStudyOutNode.cxx         
  EditionSwitch.hxx               
  EditionSwitch.cxx               
  EditionWhile.hxx                
  EditionWhile.cxx                
  FormComponent.hxx               
  FormComponent.cxx               
  FormContainer.hxx               
  FormContainer.cxx
  FormHPContainer.hxx 
  FormHPContainer.cxx
  FormContainerBase.hxx
  FormContainerBase.cxx
  FormAdvParamContainer.hxx 
  FormAdvParamContainer.cxx   
  FormContainerDecorator.hxx               
  FormContainerDecorator.cxx        
  FormEachLoop.hxx                
  FormEachLoop.cxx                
  FormEditItem.hxx                
  FormEditItem.cxx                
  FormEditTree.hxx                
  FormEditTree.cxx                
  FormLoop.hxx                    
  FormLoop.cxx                    
  FormOptimizerLoop.hxx           
  FormOptimizerLoop.cxx           
  FormSchemaView.hxx              
  FormSchemaView.cxx              
  FormUndoRedo.hxx                
  FormUndoRedo.cxx                
  GenericGui.hxx                  
  GenericGui.cxx                  
  GraphicsView.hxx                
  GraphicsView.cxx                
  GuiEditor.hxx                   
  GuiEditor.cxx                   
  GuiExecutor.hxx                 
  GuiExecutor.cxx                 
  GuiObserver_i.hxx               
  GuiObserver_i.cxx               
  ItemEdition.hxx                 
  ItemEdition.cxx                 
  ItemMimeData.hxx                
  ItemMimeData.cxx                
  LinkAStar.hxx                   
  LinkAStar.cxx                   
  LinkMatrix.hxx                  
  LinkMatrix.cxx                  
  ListJobs_GUI.hxx		  
  ListJobs_GUI.cxx                
  LogViewer.hxx                   
  LogViewer.cxx                   
  Menus.hxx                       
  Menus.cxx                       
  Message.hxx                     
  Message.cxx                     
  PropertyEditor.hxx              
  PropertyEditor.cxx              
  QtGuiContext.hxx                
  QtGuiContext.cxx                
  Resource.hxx                    
  Resource.cxx                    
  SceneComposedNodeItem.hxx       
  SceneComposedNodeItem.cxx       
  SceneCtrlInPortItem.hxx         
  SceneCtrlInPortItem.cxx         
  SceneCtrlLinkItem.hxx           
  SceneCtrlLinkItem.cxx           
  SceneCtrlOutPortItem.hxx        
  SceneCtrlOutPortItem.cxx        
  SceneCtrlPortItem.hxx           
  SceneCtrlPortItem.cxx           
  Scene.cxx                       
  Scene.hxx                       
  SceneBlocItem.hxx               
  SceneBlocItem.cxx               
  SceneDataPortItem.hxx           
  SceneDataPortItem.cxx           
  SceneDSLinkItem.hxx             
  SceneDSLinkItem.cxx             
  SceneElementaryNodeItem.hxx     
  SceneElementaryNodeItem.cxx     
  SceneHeaderItem.hxx             
  SceneHeaderItem.cxx             
  SceneHeaderNodeItem.hxx         
  SceneHeaderNodeItem.cxx         
  SceneInPortItem.hxx             
  SceneInPortItem.cxx             
  SceneItem.hxx                   
  SceneItem.cxx                   
  SceneLinkItem.hxx               
  SceneLinkItem.cxx               
  SceneNodeItem.hxx               
  SceneNodeItem.cxx               
  SceneObserverItem.hxx           
  SceneObserverItem.cxx           
  SceneOutPortItem.hxx            
  SceneOutPortItem.cxx            
  ScenePortItem.hxx               
  ScenePortItem.cxx               
  SceneProcItem.hxx               
  SceneProcItem.cxx               
  SceneProgressItem.hxx
  SceneProgressItem.cxx
  SceneTextItem.hxx               
  SceneTextItem.cxx               
  SchemaComponentItem.hxx         
  SchemaComponentItem.cxx         
  SchemaComposedNodeItem.hxx      
  SchemaComposedNodeItem.cxx      
  SchemaContainerItem.hxx         
  SchemaContainerItem.cxx         
  SchemaDataTypeItem.hxx          
  SchemaDataTypeItem.cxx          
  SchemaDirContainersItem.hxx     
  SchemaDirContainersItem.cxx     
  SchemaDirLinksItem.hxx          
  SchemaDirLinksItem.cxx          
  SchemaDirTypesItem.hxx          
  SchemaDirTypesItem.cxx          
  SchemaInPortItem.hxx            
  SchemaInPortItem.cxx            
  SchemaItem.hxx                  
  SchemaItem.cxx                  
  SchemaLinkItem.hxx              
  SchemaLinkItem.cxx              
  SchemaModel.hxx                 
  SchemaModel.cxx                 
  SchemaNodeItem.hxx              
  SchemaNodeItem.cxx              
  SchemaOutPortItem.hxx           
  SchemaOutPortItem.cxx           
  SchemaProcItem.hxx              
  SchemaProcItem.cxx              
  SchemaReferenceItem.hxx         
  SchemaReferenceItem.cxx         
  TablePortsEdition.hxx           
  TablePortsEdition.cxx           
  TableSwitch.hxx                 
  TableSwitch.cxx                 
  TreeView.hxx                    
  TreeView.cxx                    
  ValueDelegate.hxx               
  ValueDelegate.cxx               
  VisitorSaveGuiSchema.hxx        
  VisitorSaveGuiSchema.cxx        
  YACSWidgets.hxx                 
  YACSWidgets.cxx                 
  YACSGuiLoader.hxx               
  YACSGuiLoader.cxx
  ${_moc_SOURCES}
  ${_uic_files}
  )

# --- resources ---

# resource files / to be processed by lrelease
SET(_res_files
  resources/add_in_study.png
  resources/add_node.png
  resources/addRowCols.png
  resources/arrange_nodes.png
  resources/autoComputeLink.png
  resources/block_node.png
  resources/breakpoints_active.png
  resources/breakpoints.png
  resources/centerOnNode.png
  resources/shrinkExpand.png
  resources/change_informations.png
  resources/component.png
  resources/connect_to_batch_session.png
  resources/container.png
  resources/control_link.png
  resources/control_view.png
  resources/copy.png
  resources/cut.png
  resources/dataflow_view.png
  resources/data_link.png
  resources/datastream_view.png
  resources/delete.png
  resources/emphasisLink.png
  resources/execute_in_batch.png
  resources/export_dataflow.png
  resources/filter_next_steps.png
  resources/filter_notification.png
  resources/folder_cyan.png
  resources/folder_cyan_open.png
  resources/force2nodeLink.png
  resources/full_view.png
  resources/hideLink.png
  resources/icon_down.png
  resources/icon_insert.png
  resources/icon_minus.png
  resources/icon_plus.png
  resources/icon_select.png
  resources/icon_text.png
  resources/icon_up.png
  resources/import_dataflow.png
  resources/import_superv_dataflow.png
  resources/in_port.png
  resources/insert_file.png
  resources/kill.png
  resources/load_execution_state.png
  resources/loop_node.png
  resources/mode_continue.png
  resources/modify_dataflow.png
  resources/modify_superv_dataflow.png
  resources/ModuleYacs.png
  resources/new_batch_execution.png
  resources/new_block_node.png
  resources/new_corba_component.png
  resources/new_corba_service_node.png
  resources/new_cpp_node.png
  resources/new_dataflow.png
  resources/new_edition.png
  resources/new_execution.png
  resources/new_foreach_loop_node.png
  resources/new_for_loop_node.png
  resources/new_from_library_node.png
  resources/new_inline_function_node.png
  resources/new_inline_script_node.png
  resources/new_link.png
  resources/new_nodenode_service_node.png
  resources/new_salome_component.png
  resources/new_salomepy_component.png
  resources/new_salome_service_node.png
  resources/new_service_inline_node.png
  resources/new_switch_loop_node.png
  resources/new_while_loop_node.png
  resources/new_xml_node.png
  resources/node.png
  resources/ob_service_node.png
  resources/out_port.png
  resources/paste.png
  resources/pause.png
  resources/rebuild_links.png
  resources/redo.png
  resources/reload.png
  resources/remote_run.png
  resources/reset.png
  resources/resume.png
  resources/run_active.png
  resources/run_object.png
  resources/run.png
  resources/batch.png
  resources/sample.png
  resources/save_dataflow.png
  resources/save_dataflow_state.png
  resources/schema.png
  resources/showLink.png
  resources/simplifyLink.png
  resources/step_by_step_active.png
  resources/step_by_step.png
  resources/straightLink.png
  resources/stream_link.png
  resources/suspend_resume.png
  resources/switch_node.png
  resources/table_view.png
  resources/toggle_stop_on_error.png
  resources/toggleVisibility.png
  resources/undo.png
  resources/whatsThis.png
  resources/zoomToBloc.png
)
# --- rules ---

ADD_LIBRARY(GenericGui ${GenericGui_SOURCES})
TARGET_LINK_LIBRARIES(GenericGui ${_link_LIBRARIES})
INSTALL(TARGETS GenericGui EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${_res_files} DESTINATION ${SALOME_YACS_INSTALL_RES_DATA})
