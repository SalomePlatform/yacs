//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <RuntimeSALOME.hxx>

#include <yacs.hh>

#include <YACSGui_Module.h>
#include <YACSGui_Graph.h>
#include <YACSGui_DataModel.h>
#include <YACSGui_Selection.h>
//#include <YACSGui_DataObject.h>
#include <YACSGui_Executor.h>
#include <YACSGui_XMLDriver.h>
#include <YACSGui_Observer.h>
#include "YACSGui_RunMode.h"
#include <YACSGui_InputPanel.h>
#include <YACSGui_TreeView.h>
#include <YACSGui_TreeViewItem.h>

#include <YACSPrs_ElementaryNode.h>
#include <YACSPrs_BlocNode.h>
#include <YACSPrs_Link.h>

#include <OB_Browser.h>
#include <OB_ListItem.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_FileDlg.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ToolButton.h>
#include <QtxToolBar.h>
#include <CAM_Application.h>
#include <LightApp_WidgetContainer.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_DataModel.h>
#include <SalomeApp_DataObject.h>
#include <QxGraph_ViewModel.h>
#include <QxGraph_ViewWindow.h>
#include <QxGraph_ViewManager.h>
#include <QxGraph_Canvas.h> // for test presentations
#include <QxGraph_Prs.h> // for test presentations

#include <commandsProc.hxx>

#include <Catalog.hxx>
#include <OutputPort.hxx> // for a test function
#include <OutputDataStreamPort.hxx> // for a test function
#include <Node.hxx>
#include <PythonNode.hxx>
#include <SalomePythonNode.hxx>
#include <CppNode.hxx>
#include <CORBANode.hxx>
#include <XMLNode.hxx>
#include <Switch.hxx>
#include <ForLoop.hxx>
#include <WhileLoop.hxx>
#include <ForEachLoop.hxx>
#include <SalomeComponent.hxx>

#include <SALOME_LifeCycleCORBA.hxx>

#include <SALOME_ModuleCatalog.hh>

#include "SALOMEDS_Study.hxx"
#include "SALOMEDS_Tool.hxx"

#include <qapplication.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qtextstream.h>

#include <iostream> // for debug only
#include "utilities.h"

using namespace YACS;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

using namespace std;


extern "C"
{
  CAM_Module* createModule()
  {
    return new YACSGui_Module();
  }
}

//! Constructor.
/*!
 */
YACSGui_Module::YACSGui_Module() : SalomeApp_Module( "YACS" )
{
  MESSAGE("YACSGui_Module::YACSGui_Module()");
  _myRunMode = 0;
  myInputPanel = 0;
  myCatalog = 0;
  myGuiMode = YACSGui_Module::EmptyMode;
  myBlockWindowActivateSignal = false;
}

//! Destructor.
/*!
 */
YACSGui_Module::~YACSGui_Module()
{
  MESSAGE("YACSGui_Module::~YACSGui_Module()");
}

//! Initialize module.
/*!
 */
void YACSGui_Module::initialize( CAM_Application* theApp )
{
  MESSAGE("YACSGui_Module::initialize");
  SalomeApp_Module::initialize(theApp);
  //InitYACSGuiGen( dynamic_cast<SalomeApp_Application*>( theApp ) );
  createSComponent();
  updateObjBrowser();

  createActions();
  createMenus();
  createPopups();

  RuntimeSALOME::setRuntime();
  fillNodeTypesMap();

  if ( theApp && theApp->desktop() )
    connect( theApp->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
             this, SLOT(onWindowActivated( SUIT_ViewWindow* )) );
    
}

//! Creates module actions.
/*!
 */
void YACSGui_Module::createActions()
{
  MESSAGE("YACSGui_Module::createActions()");
  QPixmap aPixmap;
  QWidget* aDesktop = application()->desktop();
  SUIT_ResourceMgr* aResourceMgr =  SUIT_Session::session()->resourceMgr();
  
  if (!aDesktop || !aResourceMgr)
    return;
  
  // Menu "File"
  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_IMPORT_YACS_SCHEMA"));
  createAction( ImportSchemaId, tr("TOP_IMPORT_YACS_SCHEMA"), QIconSet(aPixmap),
                tr("MEN_IMPORT_YACS_SCHEMA"), tr("STB_IMPORT_YACS_SCHEMA"), 
		0, aDesktop, false, this, SLOT(onImportSchema()));
  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_IMPORT_SUPERV_SCHEMA"));
  createAction( ImportSupervSchemaId, tr("TOP_IMPORT_SUPERV_SCHEMA"), QIconSet(aPixmap),
                tr("MEN_IMPORT_SUPERV_SCHEMA"), tr("STB_IMPORT_SUPERV_SCHEMA"), 
		0, aDesktop, false, this, SLOT(onImportSupervSchema()));
  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_EXPORT_SCHEMA"));
  createAction( ExportSchemaId, tr("TOP_EXPORT_SCHEMA"), QIconSet(aPixmap),
                tr("MEN_EXPORT_SCHEMA"), tr("STB_EXPORT_SCHEMA"), 
		0, aDesktop, false, this, SLOT(onExportSchema()));
  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_SCHEMA"));
  createAction( NewSchemaId, tr("TOP_NEW_SCHEMA"), QIconSet(aPixmap),
                tr("MEN_NEW_SCHEMA"), tr("STB_NEW_SCHEMA"), 
		0, aDesktop, false, this, SLOT(onNewSchema()));
  
  // Menu "Create"
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_CONTAINER"));
  createAction( NewContainerId, tr("TOP_NEW_CONTAINER"), QIconSet(aPixmap),
                tr("MEN_NEW_CONTAINER"), tr("STB_NEW_CONTAINER"), 
		0, aDesktop, false, this, SLOT(onNewContainer()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_SALOME_COMPONENT"));
  createAction( NewSalomeComponentId, tr("TOP_NEW_SALOME_COMPONENT"), QIconSet(aPixmap),
                tr("MEN_NEW_SALOME_COMPONENT"), tr("STB_NEW_SALOME_COMPONENT"), 
		0, aDesktop, false, this, SLOT(onNewSalomeComponent()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_SALOMEPYTHON_COMPONENT"));
  createAction( NewSalomePythonComponentId, tr("TOP_NEW_SALOMEPYTHON_COMPONENT"), QIconSet(aPixmap),
                tr("MEN_NEW_SALOMEPYTHON_COMPONENT"), tr("STB_NEW_SALOMEPYTHON_COMPONENT"), 
		0, aDesktop, false, this, SLOT(onNewSalomePythonComponent()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_CORBA_COMPONENT"));
  createAction( NewCorbaComponentId, tr("TOP_NEW_CORBA_COMPONENT"), QIconSet(aPixmap),
                tr("MEN_NEW_CORBA_COMPONENT"), tr("STB_NEW_CORBA_COMPONENT"), 
		0, aDesktop, false, this, SLOT(onNewCorbaComponent()));
  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_SALOME_SERVICE_NODE"));
  createAction( NewSalomeServiceNodeId, tr("TOP_NEW_SALOME_SERVICE_NODE"), QIconSet(aPixmap),
                tr("MEN_NEW_SALOME_SERVICE_NODE"), tr("STB_NEW_SALOME_SERVICE_NODE"), 
		0, aDesktop, false, this, SLOT(onSalomeServiceNode()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_CORBA_SERVICE_NODE"));
  createAction( NewCorbaServiceNodeId, tr("TOP_NEW_CORBA_SERVICE_NODE"), QIconSet(aPixmap),
                tr("MEN_NEW_CORBA_SERVICE_NODE"), tr("STB_NEW_CORBA_SERVICE_NODE"), 
		0, aDesktop, false, this, SLOT(onCorbaServiceNode()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_NODENODE_SERVICE_NODE"));
  createAction( NewNodeNodeServiceNodeId, tr("TOP_NEW_NODENODE_SERVICE_NODE"), QIconSet(aPixmap),
                tr("MEN_NEW_NODENODE_SERVICE_NODE"), tr("STB_NEW_NODENODE_SERVICE_NODE"), 
		0, aDesktop, false, this, SLOT(onNodeNodeServiceNode()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_CPP_NODE"));
  createAction( NewCppNodeId, tr("TOP_NEW_CPP_NODE"), QIconSet(aPixmap),
                tr("MEN_NEW_CPP_NODE"), tr("STB_NEW_CPP_NODE"), 
		0, aDesktop, false, this, SLOT(onCppNode()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_SERVICE_INLINE_NODE"));
  createAction( NewServiceInlineNodeId, tr("TOP_NEW_SERVICE_INLINE_NODE"), QIconSet(aPixmap),
                tr("MEN_NEW_SERVICE_INLINE_NODE"), tr("STB_NEW_SERVICE_INLINE_NODE"), 
		0, aDesktop, false, this, SLOT(onServiceInlineNode()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_XML_NODE"));
  createAction( NewXmlNodeId, tr("TOP_NEW_XML_NODE"), QIconSet(aPixmap),
                tr("MEN_NEW_XML_NODE"), tr("STB_NEW_XML_NODE"), 
		0, aDesktop, false, this, SLOT(onXMLNode()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_INLINE_SCRIPT_NODE"));
  createAction( NewInlineScriptNodeId, tr("TOP_NEW_INLINE_SCRIPT_NODE"), QIconSet(aPixmap),
                tr("MEN_NEW_INLINE_SCRIPT_NODE"), tr("STB_NEW_INLINE_SCRIPT_NODE"), 
		0, aDesktop, false, this, SLOT(onInlineScriptNode()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_INLINE_FUNCTION_NODE"));
  createAction( NewInlineFunctionNodeId, tr("TOP_NEW_INLINE_FUNCTION_NODE"), QIconSet(aPixmap),
                tr("MEN_NEW_INLINE_FUNCTION_NODE"), tr("STB_NEW_INLINE_FUNCTION_NODE"), 
		0, aDesktop, false, this, SLOT(onInlineFunctionNode()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_BLOCK_NODE"));
  createAction( NewBlockNodeId, tr("TOP_NEW_BLOCK_NODE"), QIconSet(aPixmap),
                tr("MEN_NEW_BLOCK_NODE"), tr("STB_NEW_BLOCK_NODE"), 
		0, aDesktop, false, this, SLOT(onBlockNode()));
  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_FOR_LOOP_NODE"));
  createAction( NewForLoopNodeId, tr("TOP_NEW_FOR_LOOP_NODE"), QIconSet(aPixmap),
                tr("MEN_NEW_FOR_LOOP_NODE"), tr("STB_NEW_FOR_LOOP_NODE"), 
		0, aDesktop, false, this, SLOT(onFORNode()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_FOREACH_LOOP_NODE"));
  createAction( NewForeachLoopNodeId, tr("TOP_NEW_FOREACH_LOOP_NODE"), QIconSet(aPixmap),
                tr("MEN_NEW_FOREACH_LOOP_NODE"), tr("STB_NEW_FOREACH_LOOP_NODE"), 
		0, aDesktop, false, this, SLOT(onFOREACHNode()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_WHILE_LOOP_NODE"));
  createAction( NewWhileLoopNodeId, tr("TOP_NEW_WHILE_LOOP_NODE"), QIconSet(aPixmap),
                tr("MEN_NEW_WHILE_LOOP_NODE"), tr("STB_NEW_WHILE_LOOP_NODE"), 
		0, aDesktop, false, this, SLOT(onWHILENode()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_SWITCH_LOOP_NODE"));
  createAction( NewSwitchLoopNodeId, tr("TOP_NEW_SWITCH_LOOP_NODE"), QIconSet(aPixmap),
                tr("MEN_NEW_SWITCH_LOOP_NODE"), tr("STB_NEW_SWITCH_LOOP_NODE"), 
		0, aDesktop, false, this, SLOT(onSWITCHNode()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_FROM_LIBRARY_NODE"));
  createAction( NewFromLibraryNodeId, tr("TOP_NEW_FROM_LIBRARY_NODE"), QIconSet(aPixmap),
                tr("MEN_NEW_FROM_LIBRARY_NODE"), tr("STB_NEW_FROM_LIBRARY_NODE"), 
		0, aDesktop, false, this, SLOT(onNodeFromLibrary()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_LINK"));
  createAction( NewLinkId, tr("TOP_NEW_LINK"), QIconSet(aPixmap),
                tr("MEN_NEW_LINK"), tr("STB_NEW_LINK"), 
		0, aDesktop, false, this, SLOT(onNewLink()));

  // Menu "Supervisor"
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_SUSPEND_RESUME"));
  createAction( SuspendResumeDataflowId, tr("TOP_SUSPEND_RESUME_DATAFLOW"), QIconSet(aPixmap),
                tr("MEN_SUSPEND_RESUME_DATAFLOW"), tr("STB_SUSPEND_RESUME_DATAFLOW"), 
		0, aDesktop, false, this, SLOT(onSuspendResumeDataflow()));

  /*  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_RUN_DATAFLOW"));
  createAction( RunDataflowId, tr("TOP_RUN_DATAFLOW"), QIconSet(aPixmap),
                tr("MEN_RUN_DATAFLOW"), tr("STB_RUN_DATAFLOW"), 
		0, aDesktop, false, this, SLOT(onRunDataflow()));
  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_REMOTE_RUN_DATAFLOW"));
  createAction( RemoteRunDataflowId, tr("TOP_REMOTE_RUN_DATAFLOW"), QIconSet(aPixmap),
                tr("MEN_REMOTE_RUN_DATAFLOW"), tr("STB_REMOTE_RUN_DATAFLOW"), 
		0, aDesktop, false, this, SLOT(onRemoteRunDataflow()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_KILL_DATAFLOW"));
  createAction( KillDataflowId, tr("TOP_KILL_DATAFLOW"), QIconSet(aPixmap),
                tr("MEN_KILL_DATAFLOW"), tr("STB_KILL_DATAFLOW"), 
		0, aDesktop, false, this, SLOT(onKillDataflow()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_TOGGLESTOPONERROR"));
  createAction( ToggleStopOnErrorId, tr("TOP_TOGGLESTOPONERROR"), QIconSet(aPixmap),
                tr("MEN_TOGGLESTOPONERROR"), tr("STB_TOGGLESTOPONERROR"), 
		0, aDesktop, false, this, SLOT(onToggleStopOnError()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_FILTERNEXTSTEPS"));
  createAction( FilterNextStepsId, tr("TOP_FILTERNEXTSTEPS"), QIconSet(aPixmap),
                tr("MEN_FILTERNEXTSTEPS"), tr("STB_FILTERNEXTSTEPS"), 
		0, aDesktop, false, this, SLOT(onFilterNextSteps()));
  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_CHANGE_INFORMATIONS"));
  createAction( ChangeInformationsId, tr("TOP_CHANGE_INFORMATIONS"), QIconSet(aPixmap),
                tr("MEN_CHANGE_INFORMATIONS"), tr("STB_CHANGE_INFORMATIONS"), 
		0, aDesktop, false, this, SLOT(onChangeInformations()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_SAVEDATAFLOWSTATE"));
  createAction( SaveDataflowStateId, tr("TOP_SAVEDATAFLOWSTATE"), QIconSet(aPixmap),
                tr("MEN_SAVEDATAFLOWSTATE"), tr("STB_SAVEDATAFLOWSTATE"), 
		0, aDesktop, false, this, SLOT(onSaveDataflowState()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_RELOAD_DATAFLOW"));
  createAction( ReloadDataflowId, tr("TOP_RELOAD_DATAFLOW"), QIconSet(aPixmap),
                tr("MEN_RELOAD_DATAFLOW"), tr("STB_RELOAD_DATAFLOW"), 
		0, aDesktop, false, this, SLOT(onReloadDataflow()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_ARRANGE_NODES"));
  createAction( ArrangeNodesId, tr("TOP_ARRANGE_NODES"), QIconSet(aPixmap),
                tr("MEN_ARRANGE_NODES"), tr("STB_ARRANGE_NODES"), 
		0, aDesktop, false, this, SLOT(onArrangeNodes()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_REBUILD_LINKS"));
  createAction( RebuildLinksId, tr("TOP_REBUILD_LINKS"), QIconSet(aPixmap),
                tr("MEN_REBUILD_LINKS"), tr("STB_REBUILD_LINKS"), 
		0, aDesktop, false, this, SLOT(onRebuildLinks()));

  // Menu "Execute"
  createAction( NewExecutionId, tr("TOP_NEW_EXECUTION"), QIconSet(),
                tr("MEN_NEW_EXECUTION"), tr("STB_NEW_EXECUTION"), 
		0, aDesktop, false, this, SLOT(onCreateExecution()));

  createAction( ReloadExecutionId, tr("TOP_RELOAD_EXECUTION"), QIconSet(),
                tr("MEN_RELOAD_EXECUTION"), tr("STB_RELOAD_EXECUTION"), 
		0, aDesktop, false, this, SLOT(onReloadExecution()));

   // Popup "Schema"
  createAction( SetActiveId, tr("TOP_SET_ACTIVE"), QIconSet(),
                tr("MEN_SET_ACTIVE"), tr("STB_SET_ACTIVE"), 
		0, aDesktop, false, this, SLOT(onSetActive()));


   // View mode section of "Dataflow" toolbar  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_FULL_VIEW"));
  createAction( FullViewId, tr("TOP_FULL_VIEW"), QIconSet(aPixmap),
                tr("MEN_FULL_VIEW"), tr("STB_FULL_VIEW"), 
		0, aDesktop, false, this, SLOT(onFullView()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_CONTROL_VIEW"));
  createAction( ControlViewId, tr("TOP_CONTROL_VIEW"), QIconSet(aPixmap),
                tr("MEN_CONTROL_VIEW"), tr("STB_CONTROL_VIEW"), 
		0, aDesktop, false, this, SLOT(onControlView()));
  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_TABLE_VIEW"));
  createAction( TableViewId, tr("TOP_TABLE_VIEW"), QIconSet(aPixmap),
		tr("MEN_TABLE_VIEW"), tr("STB_TABLE_VIEW"), 
		0, aDesktop, false, this, SLOT(onTableView()));
  */
  
  // Menu "Execute" (edit gui mode)
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_EXECUTION"));
  createAction( NewExecutionId, tr("TOP_NEW_EXECUTION"), QIconSet(aPixmap),
                tr("MEN_NEW_EXECUTION"), tr("STB_NEW_EXECUTION"), 
		0, aDesktop, false, this, SLOT(onCreateExecution()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_BATCH_EXECUTION"));
  createAction( NewBatchExecutionId, tr("TOP_NEW_BATCH_EXECUTION"), QIconSet(aPixmap),
                tr("MEN_NEW_BATCH_EXECUTION"), tr("STB_NEW_BATCH_EXECUTION"), 
		0, aDesktop, false, this, SLOT(onCreateBatchExecution()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_LOAD_EXECUTION_STATE"));
  createAction( ReloadExecutionId, tr("TOP_LOAD_EXECUTION_STATE"), QIconSet(aPixmap),
                tr("MEN_LOAD_EXECUTION_STATE"), tr("STB_LOAD_EXECUTION_STATE"), 
		0, aDesktop, false, this, SLOT(onReloadExecution()));
  
    //Menu "Execute" (run gui mode)
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_EXECUTE"));
  createAction( ExecuteId, tr("TOP_EXECUTE"), QIconSet(aPixmap),
                tr("MEN_EXECUTE"), tr("STB_EXECUTE"), 
		0, aDesktop, false, this, SLOT(onExecute()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_EXECUTE_STBY"));
  createAction( ExecuteStById, tr("TOP_EXECUTE_STBY"), QIconSet(aPixmap),
                tr("MEN_EXECUTE_STBY"), tr("STB_EXECUTE_STBY"), 
		0, aDesktop, false, this, SLOT(onExecuteStBy()));
  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_EXECUTE_BATCH"));
  createAction( ExecuteInBatchId, tr("TOP_EXECUTE_BATCH"), QIconSet(aPixmap),
                tr("MEN_EXECUTE_BATCH"), tr("STB_EXECUTE_BATCH"), 
		0, aDesktop, false, this, SLOT(onExecuteInBatch()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_PAUSE"));
  createAction( PauseId, tr("TOP_PAUSE"), QIconSet(aPixmap),
                tr("MEN_PAUSE"), tr("STB_PAUSE"), 
		0, aDesktop, false, this, SLOT(onPause()));
  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_RESUME"));
  createAction( ResumeId, tr("TOP_RESUME"), QIconSet(aPixmap),
                tr("MEN_RESUME"), tr("STB_RESUME"), 
		0, aDesktop, false, this, SLOT(onResume()));

  /*aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_SUSPEND_RESUME"));
  createAction( ResumeId, tr("TOP_SUSPEND_RESUME"), QIconSet(aPixmap),
                tr("MEN_SUSPEND_RESUME"), tr("STB_SUSPEND_RESUME"), 
		0, aDesktop, false, this, SLOT(onSuspendResume()));*/

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_ABORT"));
  createAction( AbortId, tr("TOP_ABORT"), QIconSet(aPixmap),
                tr("MEN_ABORT"), tr("STB_ABORT"), 
		0, aDesktop, false, this, SLOT(onAbort()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_RESET"));
  createAction( ResetId, tr("TOP_RESET"), QIconSet(aPixmap),
                tr("MEN_RESET"), tr("STB_RESET"), 
		0, aDesktop, false, this, SLOT(onReset()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_SAVE_EXECUTION_STATE"));
  createAction( SaveExecutionStateId, tr("TOP_SAVE_EXECUTION_STATE"), QIconSet(aPixmap),
                tr("MEN_SAVE_EXECUTION_STATE"), tr("STB_SAVE_EXECUTION_STATE"), 
		0, aDesktop, false, this, SLOT(onSaveExecutionState()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_CONNECT_TO_RUNNING"));
  createAction( ConnectToRunningId, tr("TOP_CONNECT_TO_RUNNING"), QIconSet(aPixmap),
                tr("MEN_CONNECT_TO_RUNNING"), tr("STB_CONNECT_TO_RUNNING"), 
		0, aDesktop, false, this, SLOT(onConnectToRunning()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_TOGGLESTOPONERROR"));
  createAction( ToggleStopOnErrorId, tr("TOP_TOGGLESTOPONERROR"), QIconSet(aPixmap),
                tr("MEN_TOGGLESTOPONERROR"), tr("STB_TOGGLESTOPONERROR"), 
		0, aDesktop, true, this, SLOT(onToggleStopOnError()));

   // Popup "Schema"
  createAction( SetActiveId, tr("TOP_SET_ACTIVE"), QIconSet(),
                tr("MEN_SET_ACTIVE"), tr("STB_SET_ACTIVE"), 
		0, aDesktop, false, this, SLOT(onSetActive()));


  // View mode section of "Dataflow" toolbar  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_FULL_VIEW"));
  createAction( FullViewId, tr("TOP_FULL_VIEW"), QIconSet(aPixmap),
                tr("MEN_FULL_VIEW"), tr("STB_FULL_VIEW"), 
		0, aDesktop, false, this, SLOT(onFullView()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_CONTROL_VIEW"));
  createAction( ControlViewId, tr("TOP_CONTROL_VIEW"), QIconSet(aPixmap),
                tr("MEN_CONTROL_VIEW"), tr("STB_CONTROL_VIEW"), 
		0, aDesktop, false, this, SLOT(onControlView()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_DATAFLOW_VIEW"));
  createAction( DataFlowViewId, tr("TOP_DATAFLOW_VIEW"), QIconSet(aPixmap),
                tr("MEN_DATAFLOW_VIEW"), tr("STB_DATAFLOW_VIEW"),
		0, aDesktop, false, this, SLOT(onDataFlowView()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_DATASTREAM_VIEW"));
  createAction( DataStreamViewId, tr("TOP_DATASTREAM_VIEW"), QIconSet(aPixmap),
                tr("MEN_DATASTREAM_VIEW"), tr("STB_DATASTREAM_VIEW"),
		0, aDesktop, false, this, SLOT(onDataStreamView()));
  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_ARRANGE_NODES"));
  createAction( ArrangeNodesId, tr("TOP_ARRANGE_NODES"), QIconSet(aPixmap),
                tr("MEN_ARRANGE_NODES"), tr("STB_ARRANGE_NODES"), 
		0, aDesktop, false, this, SLOT(onArrangeNodes()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_REBUILD_LINKS"));
  createAction( RebuildLinksId, tr("TOP_REBUILD_LINKS"), QIconSet(aPixmap),
                tr("MEN_REBUILD_LINKS"), tr("STB_REBUILD_LINKS"), 
		0, aDesktop, false, this, SLOT(onRebuildLinks()));
}

//! Creates module menus.
/*!
 */
void YACSGui_Module::createMenus()
{
  MESSAGE("YACSGui_Module::createMenus");
  int fileId = createMenu( tr( "MEN_FILE" ), -1, -1 );
  //@ createMenu( NewDataflowId,    fileId, 10 );
  //@ createMenu( ModifyDataflowId, fileId, 10 );
  //@ createMenu( ModifySupervDataflowId, fileId, 10 );
  int importSchemaId = createMenu( tr( "MEN_IMPORT_SCHEMA" ), fileId, 11, 10 );
  createMenu( ImportSchemaId, importSchemaId, -1 );
  createMenu( ImportSupervSchemaId, importSchemaId, -1 );
  createMenu( ExportSchemaId, fileId, 10 );
  createMenu( NewSchemaId, fileId, 10 );
  
  int createId = createMenu( tr( "MEN_CREATE" ), -1, -1, 10 );
  createMenu( NewContainerId, createId, -1 );
  int newComponentId = createMenu( tr("MEN_NEW_COMPONENT"), createId );
  createMenu( NewSalomeComponentId, newComponentId, -1 );
  createMenu( NewSalomePythonComponentId, newComponentId, -1 );
  createMenu( NewCorbaComponentId, newComponentId, -1 );
  int newNodeId = createMenu( tr("MEN_NEW_NODE"), createId );
  createMenu( NewSalomeServiceNodeId, newNodeId, -1 );
  createMenu( NewCorbaServiceNodeId, newNodeId, -1 );
  createMenu( NewNodeNodeServiceNodeId, newNodeId, -1 );
  createMenu( NewCppNodeId, newNodeId, -1 );
  createMenu( NewServiceInlineNodeId, newNodeId, -1 );
  createMenu( NewXmlNodeId, newNodeId, -1 );
  createMenu( NewInlineScriptNodeId, newNodeId, -1 );
  createMenu( NewInlineFunctionNodeId, newNodeId, -1 );
  createMenu( NewBlockNodeId, newNodeId, -1 );
  createMenu( NewForLoopNodeId, newNodeId, -1 );
  createMenu( NewForeachLoopNodeId, newNodeId, -1 );
  createMenu( NewWhileLoopNodeId, newNodeId, -1 );
  createMenu( NewSwitchLoopNodeId, newNodeId, -1 );
  createMenu( NewFromLibraryNodeId, newNodeId, -1 );
  createMenu( NewLinkId, createId, -1 );

  //Menu "Execute" (edit gui mode)
  int execId = createMenu( tr( "MEN_EXECUTE" ), -1, -1, 10 );
  createMenu( NewExecutionId, execId, -1 );
  createMenu( ReloadExecutionId, execId, -1 );
  createMenu( NewBatchExecutionId, execId, -1 );

  //Menu "Execute" (run gui mode)
  createMenu( ExecuteId, execId, -1 );
  createMenu( ExecuteStById, execId, -1 );
  createMenu( ExecuteInBatchId, execId, -1 );
  createMenu( PauseId, execId, -1 );
  createMenu( ResumeId, execId, -1 );
  createMenu( AbortId, execId, -1 );
  createMenu( ResetId, execId, -1 );
  //createMenu( StopOnErrorId, execId, -1 );
  createMenu( separator(),execId, -1 );
  createMenu( SaveExecutionStateId, execId, -1 );
  createMenu( separator(),execId, -1 );
  createMenu( ConnectToRunningId, execId, -1 );
  createMenu( ToggleStopOnErrorId, execId, -1 );

  
  /*  int supervisorId = createMenu( tr( "MEN_SUPERVISOR" ), -1, -1, 10 );
  createMenu( separator(),             supervisorId, -1 );
  createMenu( SuspendResumeDataflowId, supervisorId, -1 );
  createMenu( RemoteRunDataflowId,     supervisorId, -1 );
  createMenu( RunDataflowId,           supervisorId, -1 );
  createMenu( KillDataflowId,          supervisorId, -1 );
  createMenu( separator(),             supervisorId, -1 );
  createMenu( ToggleStopOnErrorId,     supervisorId, -1 );
  createMenu( separator(),             supervisorId, -1 );
  createMenu( AddDataflowInStudyId,    supervisorId, -1 );
  createMenu( ChangeInformationsId,    supervisorId, -1 );
  createMenu( separator(),             supervisorId, -1 );
  createMenu( SaveDataflowStateId,     supervisorId, -1 );
  createMenu( ReloadDataflowId,        supervisorId, -1 );
  createMenu( ArrangeNodesId,          supervisorId, -1 );
  createMenu( RebuildLinksId,          supervisorId, -1 );
  */

      //Toolbar "Schema"

  myToolBars[SchemaTbId] = new QtxToolBar(true ,application()->desktop());
  action(NewSchemaId)->addTo(myToolBars[SchemaTbId]);
  myToolButtons[ImportBtnId] = new SUIT_ToolButton(myToolBars[SchemaTbId],"import_btn");
  myToolButtons[ImportBtnId]->AddAction(action(ImportSchemaId));
  myToolButtons[ImportBtnId]->AddAction(action(ImportSupervSchemaId));
  myToolBars[SchemaTbId]->setLabel(tr("TOOLBAR_SCHEMA"));
  action(ExportSchemaId)->addTo(myToolBars[SchemaTbId]);


  //Toolbar "Creation"
  myToolBars[CreationTbId]= new QtxToolBar(true ,application()->desktop());
  myToolBars[CreationTbId]->setLabel(tr("TOOLBAR_CREATION"));
  action(NewContainerId)->addTo(myToolBars[CreationTbId]);
  myToolButtons[NewContainerBtnId] = new SUIT_ToolButton(myToolBars[CreationTbId], "new_container_btn");
  myToolButtons[NewContainerBtnId]->AddAction(action(NewSalomeComponentId));
  myToolButtons[NewContainerBtnId]->AddAction(action(NewSalomePythonComponentId));
  myToolButtons[NewContainerBtnId]->AddAction(action(NewCorbaComponentId));
  
  myToolButtons[NewNodeBtn] = new SUIT_ToolButton(myToolBars[CreationTbId], "new_node_btn");
  myToolButtons[NewNodeBtn]->AddAction(action(NewSalomeServiceNodeId));
  myToolButtons[NewNodeBtn]->AddAction(action(NewCorbaServiceNodeId));
  myToolButtons[NewNodeBtn]->AddAction(action(NewNodeNodeServiceNodeId));
  myToolButtons[NewNodeBtn]->AddAction(action(NewCppNodeId));
  myToolButtons[NewNodeBtn]->AddAction(action(NewServiceInlineNodeId));
  myToolButtons[NewNodeBtn]->AddAction(action(NewXmlNodeId));
  myToolButtons[NewNodeBtn]->AddAction(action(NewInlineScriptNodeId));
  myToolButtons[NewNodeBtn]->AddAction(action(NewInlineFunctionNodeId));
  myToolButtons[NewNodeBtn]->AddAction(action(NewBlockNodeId));
  myToolButtons[NewNodeBtn]->AddAction(action(NewForLoopNodeId));
  myToolButtons[NewNodeBtn]->AddAction(action(NewForeachLoopNodeId));
  myToolButtons[NewNodeBtn]->AddAction(action(NewWhileLoopNodeId));
  myToolButtons[NewNodeBtn]->AddAction(action(NewSwitchLoopNodeId));
  myToolButtons[NewNodeBtn]->AddAction(action(NewFromLibraryNodeId));

  action(NewLinkId)->addTo(myToolBars[CreationTbId]);
  
  //Toolbar "Execute" (edit gui mode)
  int executionTbId = createTool( tr("TOOLBAR_EXECUTION") );
  createTool( NewExecutionId,  executionTbId);
  createTool( NewBatchExecutionId,  executionTbId);
  createTool( ReloadExecutionId,  executionTbId);

  //Toolbar "Execute" (run gui mode)
  createTool( ExecuteId, executionTbId);
  createTool( ExecuteStById,  executionTbId);
  createTool( ExecuteInBatchId,  executionTbId);
  createTool( PauseId,  executionTbId);
  createTool( ResumeId,  executionTbId);
  createTool( AbortId,  executionTbId);
  createTool( ResetId,  executionTbId);
  //createTool( StopOnErrorId,  executionTbId);
  createTool( SaveExecutionStateId,  executionTbId);
  createTool( ConnectToRunningId, executionTbId);
  createTool( ToggleStopOnErrorId,  executionTbId);
  createTool( SuspendResumeDataflowId, executionTbId); //For testing

  //Toolbar "Visualization"
  int visualizationTbId = createTool( tr("TOOLBAR_VISUALIZATION") );
  createTool( FullViewId, visualizationTbId);
  createTool( ControlViewId, visualizationTbId);
  createTool( DataFlowViewId, visualizationTbId);
  createTool( DataStreamViewId, visualizationTbId);
  createTool( ArrangeNodesId, visualizationTbId);
  createTool( RebuildLinksId, visualizationTbId);

  /*int dataflowTbId = createTool( tr("TOOLBAR_DATAFLOW") );
  createTool( NewSchemaId,          dataflowTbId );
  createTool( ModifyDataflowId,       dataflowTbId );
  createTool( ModifySupervDataflowId, dataflowTbId );
  createTool( ImportSchemaId,       dataflowTbId );
  createTool( ImportSupervSchemaId, dataflowTbId );
  createTool( ExportSchemaId,       dataflowTbId );
  createTool( separator(),            dataflowTbId );
  createTool( AddDataflowInStudyId,   dataflowTbId );
  createTool( separator(),            dataflowTbId );
  createTool( ReloadDataflowId,       dataflowTbId );
  createTool( ArrangeNodesId,         dataflowTbId );
  createTool( RebuildLinksId,         dataflowTbId );
  createTool( separator(),            dataflowTbId );
  createTool( FullViewId,             dataflowTbId );
  createTool( ControlViewId,          dataflowTbId );
  createTool( TableViewId,            dataflowTbId );
  createTool( separator(),            dataflowTbId );
  createTool( SaveDataflowStateId,    dataflowTbId );

  int executionTbId = createTool( tr("TOOLBAR_EXECUTION") );
  createTool( SuspendResumeDataflowId, executionTbId );
  createTool( RemoteRunDataflowId,     executionTbId );
  createTool( RunDataflowId,           executionTbId );
  createTool( separator(),             executionTbId );
  createTool( KillDataflowId,          executionTbId );
  createTool( ToggleStopOnErrorId,     executionTbId );*/
}

//! Creates module popups.
/*!
 */
void YACSGui_Module::createPopups()
{
  QString rule = "client='ObjectBrowser' and selcount = 1 and $ObjectType = { 'SchemaObject' }";

  //Execute menu
  popupMgr()->insert( action(NewExecutionId), -1, 0 );
  popupMgr()->insert( action(ReloadExecutionId), -1, 0 );
  popupMgr()->setRule( action(NewExecutionId), rule, true );
  popupMgr()->setRule( action(ReloadExecutionId), rule, true );

  rule = "client='ObjectBrowser' and selcount = 1 and $ObjectType in { 'SchemaObject' 'RunObject' }";

  // Schema popup
  popupMgr()->insert( action(SetActiveId), -1, 1 );
  popupMgr()->setRule( action(SetActiveId), rule, true );  
}

//! Creates module Input Page.
/*!
 */
void YACSGui_Module::createInputPanel()
{
  // create input panel
  if (!myInputPanel) {
    myInputPanel = new YACSGui_InputPanel( this );
    
    int aWidth = myInputPanel->getPrefferedSize().width();
    
    myInputPanel->setFixedExtentWidth( aWidth );
    application()->desktop()->moveDockWindow( myInputPanel, Qt::DockRight );
    myInputPanel->setResizeEnabled( true );
    myInputPanel->setVerticallyStretchable( true );
    myInputPanel->setCloseMode( QDockWindow::Always );
    myInputPanel->setCaption( tr( "INPUT_PANEL" ) );
    myInputPanel->widget()->show();
    myInputPanel->hide();

    getApp()->desktop()->setAppropriate( myInputPanel, false );

    connect( myInputPanel, SIGNAL( Apply( const int ) ), this, SLOT( onApplyInputPanel( const int ) ) );
  }
}

//! Creates tree view for the given schema.
/*!
 */
void YACSGui_Module::createTreeView( YACS::HMI::SubjectProc* theSProc, YACSGui_DataModel::ObjectType theType )
{
  if ( theSProc )
  {
    SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(getApp()->activeStudy() );
    if ( !study ) return;
    
    int aStudyId = study->id();
    if ( Proc* aProc = dynamic_cast<Proc*>(theSProc->getNode()) )
    {
      int aVMId = viewManagerId( aProc ); // id of view manager for the given proc
      YACSGui_TreeView* aTV = treeView( aStudyId, aVMId );
      if ( !aTV )
	if ( theType == YACSGui_DataModel::SchemaObject )
	  // here the aVMId is index of the 2D schema view in edition mode
	  addTreeView( createTreeView( theSProc, WT_EditTreeView ), aStudyId, aVMId );
	else if ( theType == YACSGui_DataModel::RunObject )
	  // activate 2D schema view in execution mode, and get its id, use it as aVMId
	  addTreeView( createTreeView( theSProc, WT_RunTreeView ), aStudyId, aVMId );
      activateTreeView( aStudyId, aVMId );
    }
  }
}

//! Defines which windows should be created.
/*!
 */
void YACSGui_Module::windows( QMap<int, int>& mappa ) const
{
  MESSAGE("YACSGui_Module::windows");
  mappa.insert( LightApp_Application::WT_ObjectBrowser, Qt::DockLeft );
  mappa.insert( LightApp_Application::WT_PyConsole, Qt::DockBottom );
  mappa.insert( LightApp_Application::WT_LogWindow, Qt::DockBottom );
}

//! Defines viewer type.
/*!
 */
void YACSGui_Module::viewManagers( QStringList& lst ) const
{
  MESSAGE("YACSGui_Module::viewManagers");
  lst.append( QxGraph_Viewer::Type() );
}


//! Inherited public slot. Called on a module activating.
/*!
 */
bool YACSGui_Module::activateModule( SUIT_Study* theStudy )
{
  MESSAGE("YACSGui_Module::activateModule");
  bool bOk = SalomeApp_Module::activateModule( theStudy );
  setMenuShown( true );
  setToolShown( true );
  setGuiMode(YACSGui_Module::InitMode);

  //RuntimeSALOME::setRuntime();

  connect( getApp()->objectBrowser()->listView(), SIGNAL( doubleClicked( QListViewItem* ) ), 
	   this,                                  SLOT  ( onDblClick( QListViewItem* ) ) );

  connect( getApp()->objectBrowser()->listView(), SIGNAL( expanded( QListViewItem* ) ), 
	   this,                                  SLOT  ( onExpanded( QListViewItem* ) ) );
  connect( getApp()->objectBrowser()->listView(), SIGNAL( collapsed( QListViewItem* ) ), 
	   this,                                  SLOT  ( onCollapsed( QListViewItem* ) ) );

  return bOk;
}

//! Inherited public slot. Called on a module deactivating.
/*!
 */
bool YACSGui_Module::deactivateModule( SUIT_Study* theStudy )
{
  MESSAGE("YACSGui_Module::deactivateModule");
  setGuiMode(YACSGui_Module::EmptyMode);
  setMenuShown( false );
  setToolShown( false );

  // Hide input panel
  if ( myInputPanel )
    myInputPanel->hide();

  // Hide an active tree view
  if ( getApp()->activeStudy() )
  {
    int sId = getApp()->activeStudy()->id();
    if ( myTreeViews.find( sId ) != myTreeViews.end() )
      myTreeViews[sId]->hide();
  }

  disconnect( getApp()->objectBrowser()->listView(), SIGNAL( doubleClicked( QListViewItem* ) ), 
	      this,                                  SLOT  ( onDblClick( QListViewItem* ) ) );

  return SalomeApp_Module::deactivateModule( theStudy );
}

//! Module's engine IOR
/*!
 */
QString YACSGui_Module::engineIOR() const
{
  MESSAGE("YACSGui_Module::engineIOR");
  CORBA::String_var anIOR = getApp()->orb()->object_to_string( InitYACSGuiGen( getApp() ) );
  return QString( anIOR.in() );
  
  // returns default IOR
  //return getApp()->defaultEngineIOR();
}

//! Gets a reference to the module's engine
/*!
 */
YACSGui_ORB::YACSGui_Gen_ptr YACSGui_Module::InitYACSGuiGen( SalomeApp_Application* app )
{
  MESSAGE("YACSGui_Module::InitYACSGuiGen");
  Engines::Component_var comp = app->lcc()->FindOrLoad_Component( "YACSContainer", "YACSGui" );
  YACSGui_ORB::YACSGui_Gen_ptr clr = YACSGui_ORB::YACSGui_Gen::_narrow(comp);
  ASSERT(!CORBA::is_nil(clr));
  return clr;
}


void YACSGui_Module::createGraph( SUIT_ViewManager* theVM )
{
  MESSAGE("YACSGui_Module::createGraph");
  QxGraph_ViewWindow* aVW = dynamic_cast<QxGraph_ViewWindow*>( theVM->getActiveView() );
  if ( aVW )
  {
    // create graph with null Proc : waiting for import operation!
    YACS::HMI::GuiContext* aCProc = 0;
    YACSGui_Loader::PrsDataMap aPrsData;
    YACSGui_Loader::PortLinkDataMap aPortLinkData;
    YACSGui_Loader::LabelLinkDataMap aLabelLinkData;
    displayGraph( aCProc, aPrsData, aPortLinkData, aLabelLinkData );
  }
}

/*!
  Creates tree view by flag.
  \param theFlag - identificator of tree view type
*/
YACSGui_TreeView* YACSGui_Module::createTreeView( YACS::HMI::SubjectProc* theSProc, const WindowTypes theFlag )
{
  YACSGui_TreeView* wid = 0;

  QxGraph_ViewWindow* aVW = 0;

  if ( !theSProc ) return wid;

  if ( Proc* aProc = dynamic_cast<Proc*>(theSProc->getNode()) )
  {
    WindowsMap::iterator anIterator = myWindowsMap.find(aProc);
    if (anIterator != myWindowsMap.end()) aVW = (*anIterator).second;
    if ( !aVW ) return wid;
    
    if ( theFlag == WT_EditTreeView )
      wid = new YACSGui_EditionTreeView( this, theSProc, aVW/*getApp()->desktop()*/ );
    else if ( theFlag == WT_RunTreeView )
      wid = new YACSGui_RunTreeView( this, theSProc, aVW/*getApp()->desktop()*/ );
  }
  
  return wid;
}

/*!
  Returns the index of the view manager, which shows the given schema.
  \param theProc - a schema engine object
*/
int YACSGui_Module::viewManagerId( YACS::ENGINE::Proc* theProc )
{
  // get active study id
  if ( !getApp()->activeStudy() )
    return -1;
  int aStudyId = getApp()->activeStudy()->id();

  ViewManagerList aVMs;
  getApp()->viewManagers( QxGraph_Viewer::Type(), aVMs );
  
  for ( QPtrListIterator<SUIT_ViewManager> it( aVMs ); it.current(); ++it )
  {
    if ( it.current()->study()->id() == aStudyId )
      if ( QxGraph_Viewer* aV = dynamic_cast<QxGraph_Viewer*>( it.current()->getViewModel() ) )
      {
	if ( YACSGui_Graph* aG = dynamic_cast<YACSGui_Graph*>( aV->getCanvas()->getPrs() ) )
	  if ( aG->getProc() == theProc )
	    return it.current()->getId();
      }
  }

  return -1;
}

//! Private slot. Creates a new empty dataflow.
/*!
 */
void YACSGui_Module::onNewSchema()
{
  MESSAGE("YACSGui_Module::onNewSchema");
  printf("YACSGui_Module::newSchema\n");

  // TODO: create an empty Proc*
  RuntimeSALOME* aRuntimeSALOME = YACS::ENGINE::getSALOMERuntime();
  Proc* aProc = aRuntimeSALOME->createProc("newSchema");//new Proc("");
  
  // create GuiContext for Proc*
  GuiContext* aCProc = new GuiContext();
  GuiContext::setCurrent( aCProc );
  aCProc->setProc( aProc );

  // create schema object in the Object Browser
  QString name = "";
  CreateNewSchema( name, aProc );
  aProc->setName( name );

  // create new ViewManager for the new empty schema
  createNewViewManager( EditMode, aProc );
  
  // create and activate the edition tree view for the new schema
  createTreeView( aCProc->getSubjectProc(), YACSGui_DataModel::SchemaObject );
}

//! Private slot. Creates a new container.
/*!
 */
void YACSGui_Module::onNewContainer()
{
  MESSAGE("YACSGui_Module::onNewContainer()");

  if ( SubjectProc* aSchema = GuiContext::getCurrent()->getSubjectProc() )
  {
    stringstream aName;
    aName << "container" << GuiContext::getCurrent()->getNewId(CONTAINER);

    bool aRet = aSchema->addContainer(aName.str());
    if ( !aRet )
    {
      SUIT_MessageBox::warn1(getApp()->desktop(), 
			     tr("WARNING"), 
			     GuiContext::getCurrent()->_lastErrorMessage,
			     tr("BUT_OK"));
      return;
    }
    temporaryExport();
  }
}

//! Private slot. Creates a new SALOME component.
/*!
 */
void YACSGui_Module::onNewSalomeComponent()
{
  MESSAGE("YACSGui_Module::onNewSalomeComponent()");
  
  YACSGui_EditionTreeView* anETV = dynamic_cast<YACSGui_EditionTreeView*>( activeTreeView() );
  if ( !anETV )
  {
    SUIT_MessageBox::warn1(getApp()->desktop(), 
			   tr("WARNING"), 
			   tr("MSG_NO_ANY_CONTAINER_SELECTED_IN_TREE_VIEW"),
			   tr("BUT_OK"));
    return;
  }

  Subject* aSub = anETV->getSelectedSubject();
  if ( !aSub )
  {
    SUIT_MessageBox::warn1(getApp()->desktop(), 
			   tr("WARNING"), 
			   tr("MSG_NO_ANY_CONTAINER_SELECTED_IN_TREE_VIEW"),
			   tr("BUT_OK"));
    return;
  }

  if ( SubjectContainer* aCont = dynamic_cast<SubjectContainer*>(aSub) )
  {
    stringstream aName;
    aName << "component" << GuiContext::getCurrent()->getNewId(COMPONENT);
    
    SubjectComponent* aRet = GuiContext::getCurrent()->getSubjectProc()->addComponent(aName.str());
    if ( !aRet )
    {
      SUIT_MessageBox::warn1(getApp()->desktop(), 
			     tr("WARNING"), 
			     GuiContext::getCurrent()->_lastErrorMessage,
			     tr("BUT_OK"));
      return;
    }
    
    aRet->associateToContainer(aCont);
    aCont->update(ADD, COMPONENT, aRet);
    temporaryExport();
  }
}

//! Private slot. Creates a new SALOME Python component.
/*!
 */
void YACSGui_Module::onNewSalomePythonComponent()
{
  MESSAGE("YACSGui_Module::onNewSalomePythonComponent() --- NOT YET IMPLEMENTED!");
}

//! Private slot. Creates a new CORBA component.
/*!
 */
void YACSGui_Module::onNewCorbaComponent()
{
  MESSAGE("YACSGui_Module::onNewCorbaComponent() --- NOT YET IMPLEMENTED!");
}

//! Private slot. Creates a new SALOME service node.
/*!
 */
void YACSGui_Module::onSalomeServiceNode()
{
  createNode(SALOMENODE);
}

//! Private slot. Creates a new service inline node.
/*!
 */
void YACSGui_Module::onServiceInlineNode()
{
  createNode(SALOMEPYTHONNODE);
}

//! Private slot. Creates a new CORBA service node.
/*!
 */
void YACSGui_Module::onCorbaServiceNode()
{
  createNode(CORBANODE);
}

//! Private slot. Creates a new "node-node" service node.
/*!
 */
void YACSGui_Module::onNodeNodeServiceNode()
{
  MESSAGE("YACSGui_Module::onNodeNodeServiceNode --- NOT YET IMPLEMENTED!");
  //createNode(NODENODE);
}

//! Private slot. Creates a new cpp node.
/*!
 */
void YACSGui_Module::onCppNode()
{
  createNode(CPPNODE);
}

//! Private slot. Creates a new XML node.
/*!
 */
void YACSGui_Module::onXMLNode()
{
  createNode(XMLNODE);
}

//! Private slot. Creates a new inline script node.
/*!
 */
void YACSGui_Module::onInlineScriptNode()
{
  createNode(PYTHONNODE);
}

//! Private slot. Creates a new inline function node.
/*!
 */
void YACSGui_Module::onInlineFunctionNode()
{
  createNode(PYFUNCNODE);
}

//! Private slot. Creates a new block node.
/*!
 */
void YACSGui_Module::onBlockNode()
{
  createNode(BLOC);
}

//! Private slot. Creates a new FOR loop node.
/*!
 */
void YACSGui_Module::onFORNode()
{
  createNode(FORLOOP);
}

//! Private slot. Creates a new FOREACH loop node.
/*!
 */
void YACSGui_Module::onFOREACHNode()
{
  createNode(FOREACHLOOP);
}

//! Private slot. Creates a new WHILE loop node.
/*!
 */
void YACSGui_Module::onWHILENode()
{
  createNode(WHILELOOP);
}

//! Private slot. Creates a new SWITCH node.
/*!
 */
void YACSGui_Module::onSWITCHNode()
{
  createNode(SWITCH);
}

//! Private slot. Loads a new from the library.
/*!
 */
void YACSGui_Module::onNodeFromLibrary()
{
  MESSAGE("YACSGui_Module::onNodeFromLibrary --- NOT YET IMPLEMENTED!");
}

//! Private slot. Creates a new link.
/*!
 */
void YACSGui_Module::onNewLink()
{
  MESSAGE("YACSGui_Module::onNewLink()");
}

//! Private slot. Imports uneditable dataflow from the XML file(choosing via a file dialog)
/*!
 */
void YACSGui_Module::onImportSchema()
{
  MESSAGE("YACSGui_Module::onImportSchema");

  QString aFileName = SUIT_FileDlg::getFileName( application()->desktop(), "", "*.xml", tr("TLT_IMPORT_SCHEMA"), true );
  if ( GuiContext* aCProc = ImportProcFromFile(aFileName, EditMode,  false) )
    if ( Proc* aProc = aCProc->getProc() )
    { 
      // create schema object in the Object Browser
      CreateNewSchema(aFileName,aProc);
    
      // create and activate the edition tree view for the imported schema
      createTreeView( aCProc->getSubjectProc(), YACSGui_DataModel::SchemaObject );
    }
}

//! Private slot. Imports uneditable SUPERV dataflow from the XML file(choosing via a file dialog)
/*!
 */
void YACSGui_Module::onImportSupervSchema()
{
  MESSAGE("YACSGui_Module::onImportSupervSchema");

  // switch to edition mode
  //setGuiMode(YACSGui_Module::EditMode);

  QString aFileName = SUIT_FileDlg::getFileName( application()->desktop(), "", "*.xml", tr("TLT_IMPORT_SUPERV_SCHEMA"), true );
  if ( GuiContext* aCProc = ImportProcFromFile(aFileName, EditMode, false, true) )
    if ( Proc* aProc  = aCProc->getProc() )
    {
      // create schema object in the Object Browser
      CreateNewSchema(aFileName,aProc);

      // create and activate the edition tree view for the imported schema
      createTreeView( aCProc->getSubjectProc(), YACSGui_DataModel::SchemaObject );
    }
}

//! Private slot. Exports current dataflow to XML file.
/*!
 */
void YACSGui_Module::onExportSchema()
{
  MESSAGE("YACSGui_Module::onExportSchema");
  if ( !activeGraph() )
    return;
  Proc* aProc = activeGraph()->getProc();
  if ( !aProc )
    return;

  QString aFileName = SUIT_FileDlg::getFileName( application()->desktop(), aProc->getName(), "*.xml", tr("TLT_EXPORT_SCHEMA"), false );
  if (aFileName.isEmpty())
    return;

  YACSGui_VisitorSaveSchema aWriter( this, aProc );
  aWriter.openFileSchema( aFileName );
  aWriter.visitProc( aProc );
  aWriter.closeFileSchema();
}
   
//! Private slot. Reloads current dataflow.
/*!
 */
void YACSGui_Module::onReloadDataflow()
{
  MESSAGE("YACSGui_Module::onReloadDataflow");
  YACSGui_Graph* aGraph = activeGraph();
  if (aGraph)
  {
    aGraph->setToUpdate(true);
    aGraph->update();
  }
}

//! Private slot. Auto-arrange dataflow nodes.
/*!
 */
void YACSGui_Module::onArrangeNodes()
{
  YACSGui_Graph* aGraph = activeGraph();
  if (aGraph)
  {
    int anErr = 0;

    anErr = aGraph->arrangeNodesAlgo(aGraph->getProc());
    if ( anErr )
      SUIT_MessageBox::error1(getApp()->desktop(), 
			      tr("ERROR"), 
			      tr("MSG_ARRANGE_NODES_ERROR"), 
			      tr("BUT_OK"));
  }
}

//! Private slot. Rebuilds dataflow links.
/*!
 */
void YACSGui_Module::onRebuildLinks()
{
  MESSAGE("YACSGui_Module::onRebuildLinks");
  YACSGui_Graph* aGraph = activeGraph();
  if (aGraph) aGraph->rebuildLinks();
}
 
//! Private slot. Starts local dataflow execution.
/*!
 */ 
void YACSGui_Module::onRunDataflow()
{
  MESSAGE("YACSGui_Module::onRunDataflow");
  YACSGui_Executor* anExecutor = findExecutor();
  if (anExecutor)
    anExecutor->runDataflow();
}

//! Private slot. Starts remote dataflow execution.
/*!
 */ 
void YACSGui_Module::onRemoteRunDataflow()
{
  MESSAGE("YACSGui_Module::onRemoteRunDataflow");
  YACSGui_Executor* anExecutor = findExecutor();
  if (anExecutor)
    anExecutor->runDataflow(true);
}

//! Private slot. Kills dataflow execution.
/*!
 */
void YACSGui_Module::onKillDataflow()
{
  MESSAGE("YACSGui_Module::onKillDataflow");
  YACSGui_Executor* anExecutor = findExecutor();
  if (anExecutor)
  {
    if (!anExecutor->isRunning())
    {
      SUIT_MessageBox::warn1(getApp()->desktop(), 
			     tr("WARNING"), 
			     tr("MSG_DATAFLOW_IS_NOT_RUNNING"), 
			     tr("BUT_OK"));
      return;
    }
    anExecutor->stopDataflow();
    //myExecutors[aProc] = 0; //@
  }
}

//! Private slot. Suspends or resumes dataflow execution.
/*!
 */
void YACSGui_Module::onSuspendResumeDataflow()
{
  MESSAGE("YACSGui_Module::onSuspendResumeDataflow");
  YACSGui_Executor* anExecutor = findExecutor();
  if (anExecutor)
    {
      RunModeMap::iterator anIterator = _runModeMap.find(anExecutor);
      YACSGui_RunMode* aRunMode = 0;
      if (anIterator != _runModeMap.end())
        aRunMode = (*anIterator).second;
      else
        {
          aRunMode = new YACSGui_RunMode(anExecutor, application()->desktop());
          _runModeMap[anExecutor] = aRunMode;
        }
      if (!aRunMode->isShown())
        aRunMode->show();
    }
}

void YACSGui_Module::onFilterNextSteps()
{
  MESSAGE("YACSGui_Module::onFilterNextSteps --- NOT YET IMPLEMENTED!");
  YACSGui_Executor* anExecutor = findExecutor();
  if (anExecutor)
    {
    }
}

void YACSGui_Module::onSaveDataflowState()
{
  MESSAGE("YACSGui_Module::onSaveDataflowState --- NOT YET IMPLEMENTED!");
  YACSGui_Executor* anExecutor = findExecutor();
  if (anExecutor)
    {
    }
}

//! A test function: put values of all ouput ports of the schema into the study
/*!
 */
void YACSGui_Module::publishInStudy(YACSGui_Graph* theGraph)
{
  /*abd
  if (aModel)
  {
    YACSGui_ModuleObject* aRoot = dynamic_cast<YACSGui_ModuleObject*>( aModel->root() );
    if( aRoot )
    {
      // get SComponent from root data object
      _PTR(SComponent) aSComp( aRoot->object() );
      
      _PTR(Study)            aStudy = (( SalomeApp_Study* )(getApp()->activeStudy()))->studyDS();
      _PTR(StudyBuilder)     aBuilder ( aStudy->NewBuilder() );
      _PTR(GenericAttribute) anAttr;
      _PTR(AttributeName)    aName;

      Proc* aProc = theGraph->getProc();
      _PTR(SObject) aSObj = aStudy->FindObject(aProc->getName());
      if ( !aSObj )
      {	// create a new schema SObject
	printf(">> create a new schema SObject\n");
	aSObj = aBuilder->NewObject(aSComp);
	anAttr =  aBuilder->FindOrCreateAttribute(aSObj, "AttributeName");
	aName = _PTR(AttributeName) ( anAttr );
	aName->SetValue(aProc->getName());
      }

      if ( aProc->getState() == YACS::DONE )
      {
	_PTR(SObject) aResult;
	_PTR(ChildIterator) anIterator ( aStudy->NewChildIterator(aSObj) );
	for (; anIterator->More(); anIterator->Next()) {
	  if (anIterator->Value()->FindAttribute(anAttr, "AttributeName")) {
	    aName = _PTR(AttributeName) ( anAttr );
	    //if (strcmp(aName->Value().c_str(), myStartTimeString) == 0) {
	      aResult = anIterator->Value();
	      break;
	    //}
	  }
	}
	
	if ( !aResult )
	{
	  // create a new "Run ... " SObject
	  printf("create a new Run ...  SObject\n");
	  aResult = aBuilder->NewObject( aSObj );
	  anAttr = aBuilder->FindOrCreateAttribute(aResult, "AttributeName");
	  aName = _PTR(AttributeName) ( anAttr );
	  aName->SetValue("Run ...");

	  // iterates on all schema nodes and create a new node SObject for each node,
	  // which has any output port
	  set<Node*> aNodeSet = aProc->getAllRecursiveConstituents();
	  for ( set<Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ )
	  {
	    YACSPrs_ElementaryNode* aNodePrs = theGraph->getItem(*it);
	    if ( !aNodePrs ) return;
	
	    list<OutputPort*> aDataPorts = (*it)->getSetOfOutputPort();
	    list<OutputDataStreamPort *> aDataStreamPorts = (*it)->getSetOfOutputDataStreamPort();
	    
	    if ( !aDataPorts.empty() || !aDataStreamPorts.empty() )
	    {
	      // create a new node SObject
	      printf(">> create a new node SObject\n");
	      aSObj = aBuilder->NewObject( aResult );
	      anAttr = aBuilder->FindOrCreateAttribute(aSObj, "AttributeName");
	      aName = _PTR(AttributeName) ( anAttr );
	      aName->SetValue( (*it)->getRootNode()->getChildName(*it) );

	      _PTR(SObject) aPSObj;	      

	      // create output data port SObjects
	      for ( list<OutputPort*>::iterator itDP = aDataPorts.begin(); itDP != aDataPorts.end(); itDP++ )
	      {
		printf(">> create output data port SObjects\n");
		aPSObj = aBuilder->NewObject( aSObj );
		anAttr = aBuilder->FindOrCreateAttribute(aPSObj, "AttributeName");
		aName = _PTR(AttributeName) ( anAttr );
		aName->SetValue( (*itDP)->getName() );
		anAttr = aBuilder->FindOrCreateAttribute(aPSObj, "AttributeIOR");
		_PTR(AttributeIOR) anIORAttr  ( anAttr );
		anIORAttr->SetValue(aNodePrs->getPortPrs(*itDP)->getValue());
	      }

	      // create output data stream port SObjects
	      for ( list<OutputDataStreamPort*>::iterator itDSP = aDataStreamPorts.begin(); itDSP != aDataStreamPorts.end(); itDSP++ )
	      {
		printf(">> create output data stream port SObjects\n");
		aPSObj = aBuilder->NewObject( aSObj );
		anAttr = aBuilder->FindOrCreateAttribute(aPSObj, "AttributeName");
		aName = _PTR(AttributeName) ( anAttr );
		aName->SetValue( (*itDSP)->getName() );
		anAttr = aBuilder->FindOrCreateAttribute(aPSObj, "AttributeIOR");
		_PTR(AttributeIOR) anIORAttr  ( anAttr );
		anIORAttr->SetValue(aNodePrs->getPortPrs(*itDSP)->getValue());
	      }
	    }

	  } // end loop on nodes

	}
      }
    }
  }*/
}

//! Private slot. TD: what's this ?
/*!
 */
void YACSGui_Module::onChangeInformations()
{
  MESSAGE("YACSGui_Module::onChangeInformations --- NOT YET IMPLEMENTED!");
}

//! Private slot. Switch current schema to full view mode.
/*!
 */
void YACSGui_Module::onFullView()
{
  MESSAGE("YACSGui_Module::onFullView");

  YACSGui_Graph* aGraph = activeGraph();
  if (aGraph)
  {
    int aPrevMode = aGraph->getDMode();
    if ( aPrevMode != YACSGui_Graph::FullId )
    {
      QCanvas* aCanvas = aGraph->getCanvas();
      if ( !aCanvas ) return;

      aGraph->hide(); // canvas is 0 for all presentations
      aCanvas->setChanged( QRect(0,0,aCanvas->width(),aCanvas->height()) );
      aCanvas->update();

      aGraph->setDMode(YACSGui_Graph::FullId);
      aGraph->setToUpdate(true);
      aGraph->show(); // set canvas for full view presentations
      aCanvas->update();
      
      //aGraph->viewModesConsistency(aPrevMode,YACSGui_Graph::FullId);
      
      //if ( QCanvas* aCanvas = aGraph->getCanvas() )
      //{
      //	aCanvas->setChanged( QRect(0,0,aCanvas->width(),aCanvas->height()) );
      //	aCanvas->update();
      //}
    }
  }
}

//! Private slot.  Switch current schema to control view mode.
/*!
 */
void YACSGui_Module::onControlView()
{
  MESSAGE("YACSGui_Module::onControlView");

  YACSGui_Graph* aGraph = activeGraph();
  if (aGraph)
  {
    int aPrevMode = aGraph->getDMode();
    if ( aPrevMode != YACSGui_Graph::ControlId )
    {
      aGraph->hide();

      aGraph->setDMode(YACSGui_Graph::ControlId);
      aGraph->setToUpdate(true);
      aGraph->show();

      //aGraph->viewModesConsistency(aPrevMode,YACSGui_Graph::ControlId);
      
      if ( QCanvas* aCanvas = aGraph->getCanvas() )
      {
	aCanvas->setChanged( QRect(0,0,aCanvas->width(),aCanvas->height()) );
	aCanvas->update();
      }
    }
  }
}

//! Private slot.  Switch current schema to data flow view mode.
/*!
 */
void YACSGui_Module::onDataFlowView()
{
  MESSAGE("YACSGui_Module::onDataFlowView --- NOT YET IMPLEMENTED!");
}

//! Private slot.  Switch current schema to data stream view mode.
/*!
 */
void YACSGui_Module::onDataStreamView()
{
  MESSAGE("YACSGui_Module::onDataStreamView --- NOT YET IMPLEMENTED!");
}

//! Private slot.  Switch current dataflow to table view mode.
/*!
 */
void YACSGui_Module::onTableView()
{
  MESSAGE("YACSGui_Module::onTableView --- NOT YET IMPLEMENTED!");
}

//! Private slot.  Selects the original object in the object browser if a reference object is double clicked.
/*!
 */
void YACSGui_Module::onDblClick( QListViewItem* theItem )
{
  OB_ListItem* item = dynamic_cast<OB_ListItem*>( theItem );
  if ( !item ) return;

  if ( SalomeApp_DataObject* aDataObj = dynamic_cast<SalomeApp_DataObject*>( item->dataObject() ) )
  {
    if ( aDataObj && getApp()->checkDataObject(aDataObj) )
    {
      QString anEntry = aDataObj->entry();

      YACSGui_DataModel::ObjectType anObjType = getDataModel()->objectType( anEntry.latin1() );
      if ( anObjType == YACSGui_DataModel::SchemaObject || anObjType == YACSGui_DataModel::RunObject )
      {
	SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(getApp()->activeStudy() );
	if ( !study ) return;
	
	_PTR(SObject) aObj = aDataObj->object();
	if ( Proc* aProc = getDataModel()->getProc( aObj ) )
	{ //switch to edition or run mode (depends on anObjType)
          if(anObjType == YACSGui_DataModel::SchemaObject)
            setGuiMode(YACSGui_Module::EditMode);
          else if(anObjType == YACSGui_DataModel::RunObject)
            setGuiMode(YACSGui_Module::RunMode);
          
	  //                                        show edition tree view
	  int aStudyId = study->id();
	  int aVMId = viewManagerId( aProc ); // id of view manager for the given proc
	  printf(">> aVMId = %d\n",aVMId);
	  if ( aVMId == -1 )
	  { // create a new one view manager and view window
	    GuiContext* aCProc = 0;
	    
	    QxGraph_ViewWindow* aVW = createNewViewManager( myGuiMode, aProc );
	    if ( aVW )
	    {
	      int w, h;
	      YACSGui_Loader aLoader;
	      const YACSGui_Loader::PrsDataMap& aPrsData = aLoader.getPrsData( aProc, w, h );
	      const YACSGui_Loader::PortLinkDataMap& aPortLinkData = aLoader.getPortLinkData( aProc );
	      const YACSGui_Loader::LabelLinkDataMap& aLabelLinkData = aLoader.getLabelLinkData( aProc );
	      
	      // create GuiContext for Proc*
	      aCProc = new GuiContext();
	      GuiContext::setCurrent( aCProc );
	      aCProc->setProc( aProc );
	      	      
	      displayGraph( aCProc, aPrsData, aPortLinkData, aLabelLinkData );
	    }

	    // create and activate the run tree view for the create run object
	    createTreeView( aCProc ? aCProc->getSubjectProc() : 0, anObjType );
	  }
	  else
	  {
	    // activate correspondiong view window
	    activateViewWindow( aProc );
	    // activate correspondiong tree view
	    activateTreeView( aStudyId, aVMId );
	  }
          
	}
      }
    }
  }
}

//! Private slot.  Set an expanded folder icon for the expanded data object (YACSGui_DataObject) in the Object Browser.
/*!
 */
void YACSGui_Module::onExpanded( QListViewItem* theItem )
{
  OB_ListItem* item = dynamic_cast<OB_ListItem*>( theItem );
  if ( !item ) return;

  if ( SalomeApp_DataObject* aDataObj = dynamic_cast<SalomeApp_DataObject*>( item->dataObject() ) )
  {
    aDataObj->setOpen( true );
    theItem->setPixmap( 0, aDataObj->icon() ); 
  }
}

//! Private slot.  Set a collapsed folder icon for the collapsed data object (YACSGui_DataObject) in the Object Browser.
/*!
 */
void YACSGui_Module::onCollapsed( QListViewItem* theItem )
{
  OB_ListItem* item = dynamic_cast<OB_ListItem*>( theItem );
  if ( !item ) return;

  if ( SalomeApp_DataObject* aDataObj = dynamic_cast<SalomeApp_DataObject*>( item->dataObject() ) )
  {
    aDataObj->setOpen( false );
    theItem->setPixmap( 0, aDataObj->icon() ); 
  }
}

//! Private slot.  Update tree in the Object Browser and 2D schema view according to applying changes in the Input Panel.
/*!
 */
void YACSGui_Module::onApplyInputPanel( const int theId )
{
  YACSGui_EditionTreeView* anETV = dynamic_cast<YACSGui_EditionTreeView*>( activeTreeView() );
  if ( !anETV ) return;

  // get the list of items selected in the edition tree view
  list<QListViewItem*> aSelList = anETV->getSelected();
  
  // check if the current selection is a single selection
  if (  aSelList.size() == 1 )
  {
    if ( QListViewItem* anItem = aSelList.front() )
    {
      switch ( theId )
      {
      case YACSGui_InputPanel::ContainerId:
	break;
      case YACSGui_InputPanel::ComponentId:
	break;
      case YACSGui_InputPanel::SchemaId:
      	if ( YACSGui_SchemaViewItem* aSchemaItem = dynamic_cast<YACSGui_SchemaViewItem*>( anItem ) )
	{ // this is a schema view item
	  if ( Proc* aSchema = aSchemaItem->getProc() )
	  { 
	    SalomeApp_DataObject* aDataObj = dynamic_cast<SalomeApp_DataObject*>( getDataModel()->getDataObject(aSchema) );
	    if ( !aDataObj ) return;

	    // update Object Browser:
	    getApp()->objectBrowser()->updateTree(aDataObj,true);

	    // update 2D schema view (if this schema is active):
	    // a.) update caption for active 2D view (edition mode)
	    if ( activeGraph()->getProc() == aSchema )
	    {
	      QxGraph_ViewWindow* aVW = dynamic_cast<QxGraph_ViewWindow*>( getApp()->activeViewManager()->getActiveView() );
	      if ( aVW ) aVW->setCaption( QString(aSchema->getName()) );
	    }
	    
	    _PTR(SObject) aSObj = aDataObj->object();
	    if ( aSObj )
	    {
	      _PTR(Study) aStudy = (( SalomeApp_Study* )(getApp()->activeStudy()))->studyDS();
	      _PTR(ChildIterator) anIterator ( aStudy->NewChildIterator(aSObj) );
	      for (; anIterator->More(); anIterator->Next())
	      { // iterates on all run sub-objects of the given schema
		Proc* aCorrSchema = getDataModel()->getProc( anIterator->Value() );
		
		// b.) update caption for corresponding 2D views in run mode
		WindowsMap::iterator anItWM = myWindowsMap.find( aCorrSchema );
		if (anItWM != myWindowsMap.end())
		  if (QxGraph_ViewWindow* aVW = (*anItWM).second)
		    aVW->setCaption( QString(aCorrSchema->getName()) );

		// b.) update all corresponding run tree views if any
		int aVMId = viewManagerId( aCorrSchema );
		if (YACSGui_RunTreeView* aRTV = dynamic_cast<YACSGui_RunTreeView*>( treeView( (( SalomeApp_Study* )(getApp()->activeStudy()))->id(),
											      aVMId ) ) )
		  aRTV->update();
	      }
	    }

	    // 2) update presentation according to new view mode
	    if ( activeGraph()->getProc() == aSchema )
	    {
	      // ...
	    }
	  }
	}
	break;
      case YACSGui_InputPanel::InlineNodeId:
	break;
      case YACSGui_InputPanel::LinkId:
	break;
      default:
	break;
      }
    }
  }
}

//! Creates and displays presentation of a calculation graph.
/*!
 *  \param theGraph - graph to be displayed
 *  \param thePrsData - map of node coordinates and dimensions
 */
YACSGui_Graph* YACSGui_Module::displayGraph( YACS::HMI::GuiContext* theCGraph, 
					     const YACSGui_Loader::PrsDataMap& thePrsData,
					     const YACSGui_Loader::PortLinkDataMap& thePortLinkData,
					     const YACSGui_Loader::LabelLinkDataMap& theLabelLinkData)
{
  MESSAGE("YACSGui_Module::displayGraph");
  YACSGui_Graph* aGraph = 0;
  
  if ( !theCGraph ) return aGraph;

  QxGraph_Canvas* aCanvas = getCanvas();
  if ( !aCanvas )
    return aGraph;

  aGraph = new YACSGui_Graph( this, aCanvas, theCGraph );
  aGraph->show();

  // Update node geometry
  // TODO: not optimal, graph is redrawn twice - 
  // before and after node geometry is applied - to be improved...
  bool needToUpdate = false;
  if ( thePrsData.size() )
  {
    for ( YACSGui_Loader::PrsDataMap::const_iterator it = thePrsData.begin(); it != thePrsData.end(); it++ )
    {
      YACSPrs_ElementaryNode* anItem = aGraph->getItem( (*it).first );
      YACSGui_Loader::PrsData aData  = (*it).second;

      if ( !anItem )
	continue;
      
      YACSPrs_BlocNode* aBloc = dynamic_cast<YACSPrs_BlocNode*>(anItem);
      if ( aBloc )
      {
	aBloc->resize( (int)aData.width, (int)aData.height );
      }

      anItem->setIsCheckAreaNeeded(false);
      anItem->move( aData.x, aData.y );
      anItem->setIsCheckAreaNeeded(true);
      anItem->setZ( aData.z );
    }
    needToUpdate = true;
  }

  // Update port links presentations
  if ( thePortLinkData.size() )
  {
    for ( YACSGui_Loader::PortLinkDataMap::const_iterator it = thePortLinkData.begin(); it != thePortLinkData.end(); it++ )
    {
      // (*it).first.first is an output port engine
      // (*it).first.second is an input port engine
      YACSPrs_ElementaryNode* aFromNodePrs = aGraph->getItem( (*it).first.first->getNode() );
      if ( !aFromNodePrs ) continue;

      list<YACSPrs_Link*> aLinksPrs = aFromNodePrs->getPortPrs( (*it).first.first )->getLinks();
      for ( list<YACSPrs_Link*>::iterator itL = aLinksPrs.begin(); itL != aLinksPrs.end(); itL++ )
      {
	if ( YACSPrs_PortLink* aPortLink = dynamic_cast<YACSPrs_PortLink*>( *itL ) )
	{
	  if ( aPortLink->getInputPort()->getEngine() == (*it).first.second )
	  {
	    aPortLink->setPoints((*it).second);
	    aPortLink->merge();
	  }
	}
      }
    }
    needToUpdate = true;
  }

  // Update label links presentations
  if ( theLabelLinkData.size() )
  {
    for ( YACSGui_Loader::LabelLinkDataMap::const_iterator it = theLabelLinkData.begin(); it != theLabelLinkData.end(); it++ )
    {
      YACSPrs_ElementaryNode* aSlaveNodePrs = aGraph->getItem( (*it).first );
      if ( !aSlaveNodePrs ) continue;

      if ( YACSPrs_LabelLink* aLinkPrs = aSlaveNodePrs->getLabelLink() )
      {
	aLinkPrs->setPoints((*it).second);
	aLinkPrs->merge();
      }
    }
    needToUpdate = true;
  }

  if ( needToUpdate ) aCanvas->update();
}

//! Returns the presentation of a given calculation graph.
/*!
 */
YACSGui_Graph* YACSGui_Module::getGraph( YACS::ENGINE::Proc* theProc )
{
  MESSAGE("YACSGui_Module::getGraph");
  YACSGui_Graph* aGraph = 0;

  // find a graph in all viewers:
  // get active study id
  if ( !getApp()->activeStudy() )
    return aGraph;
  int aStudyId = getApp()->activeStudy()->id();

  WindowsMap::iterator anIterator = myWindowsMap.find(theProc);
  if (anIterator != myWindowsMap.end())
    if ( QxGraph_ViewWindow* aVW = (*anIterator).second )
      if ( QxGraph_Viewer* aV = aVW->getViewModel() )
      aGraph = dynamic_cast<YACSGui_Graph*>( aV->getCanvas()->getPrs() );

  /*ViewManagerList aVMs;
  getApp()->viewManagers( aVMs );
  
  for ( QPtrListIterator<SUIT_ViewManager> it( aVMs ); it.current(); ++it )
  {
    if ( it.current()->study()->id() == aStudyId )
      if ( QxGraph_Viewer* aV = dynamic_cast<QxGraph_Viewer*>( it.current()->getViewModel() ) )
	if ( aGraph = dynamic_cast<YACSGui_Graph*>( aV->getCanvas()->getPrs() ) )
	  if ( aGraph->getProc() == theProc )
	    break;
  }*/

  return aGraph;
}

//! Returns the presentation of a the calculation graph corresponding to the active view.
/*!
 */
YACSGui_Graph* YACSGui_Module::activeGraph()
{
  MESSAGE("YACSGui_Module::activeGraph");
  YACSGui_Graph* aGraph = 0;

  // get active view manager
  SUIT_ViewManager* anActVM = getApp()->activeViewManager();
  
  // check its type
  if ( anActVM && anActVM->getType() == QxGraph_Viewer::Type() )
  {
    if ( QxGraph_ViewManager* aVM = dynamic_cast<QxGraph_ViewManager*>( anActVM ) )
    {
      if ( QxGraph_Viewer* aViewer = dynamic_cast<QxGraph_Viewer*>( aVM->getViewModel() ) )
      {
	if ( QxGraph_Canvas* aCanvas = aViewer->getCanvas() )
	  aGraph = dynamic_cast<YACSGui_Graph*>( aCanvas->getPrs() );
      }
    }
  }

  /*QxGraph_Canvas* aCanvas = getCanvas();
  if ( aCanvas )
    // Assume that there's the one and only prs per a canvas
    aGraph = dynamic_cast<YACSGui_Graph*>( aCanvas->getPrs() );*/

  return aGraph;
}

//! Returns the graph viewer instance
/*!
 */
QxGraph_Viewer* YACSGui_Module::getViewer()
{
  MESSAGE("YACSGui_Module::getViewer");
  QxGraph_Viewer* aViewer = 0;
  QxGraph_ViewManager* aVM = 
    dynamic_cast<QxGraph_ViewManager*>( getApp()->getViewManager( QxGraph_Viewer::Type(), true ) );
  if ( aVM )
    aViewer = dynamic_cast<QxGraph_Viewer*>( aVM->getViewModel() );
  return aViewer;
}

//! Returns the canvas for graph drawing
/*!
 */
QxGraph_Canvas* YACSGui_Module::getCanvas()
{
  MESSAGE("YACSGui_Module::getCanvas");
  QxGraph_Canvas* aCanvas = 0;
  QxGraph_Viewer* aViewer = getViewer();
  if ( aViewer )
    aCanvas = aViewer->getCanvas();
  return aCanvas;
}

void YACSGui_Module::createElementaryNodePrs()
{
  MESSAGE("YACSGui_Module::createElementaryNodePrs --- NOT YET IMPLEMENTED!");
  /*  if ( myGraph )
  {  
    YACS::ENGINE::Node* aNode = 0; // temporary solution for test
    YACSPrs_ElementaryNode* anElemNode = 
      new YACSPrs_ElementaryNode(SUIT_Session::session()->resourceMgr(), myGraph->getCanvas(), aNode);
    myGraph->getCanvas()->getPrs()->addItem(anElemNode); // add item for the current display mode
    anElemNode->show();
    myGraph->getCanvas()->update();
    }*/
}

//! Create SComponent for the YACS module.
/*!
 */
void YACSGui_Module::createSComponent()
{
  _PTR(Study)            aStudy = (( SalomeApp_Study* )(getApp()->activeStudy()))->studyDS();
  _PTR(StudyBuilder)     aBuilder ( aStudy->NewBuilder() );
  _PTR(GenericAttribute) anAttr;
  _PTR(AttributeName)    aName;

  // Find or create "YACS" SComponent in the study
  _PTR(SComponent) aComponent = aStudy->FindComponent("YACSGui");
  if ( !aComponent ) {
    aComponent = aBuilder->NewComponent("YACSGui");
    anAttr = aBuilder->FindOrCreateAttribute(aComponent, "AttributeName");
    aName = _PTR(AttributeName) ( anAttr );
    aName->SetValue( getApp()->moduleTitle( "YACSGui" ).latin1() );
	
    anAttr = aBuilder->FindOrCreateAttribute(aComponent, "AttributePixMap");
    _PTR(AttributePixMap) aPixmap ( anAttr );
    aPixmap->SetPixMap( "YACS_MODULE_ICON" );

    aBuilder->DefineComponentInstance( aComponent, engineIOR() );
  }
}

//! Load services from Module Catalog
/*!
 */
void YACSGui_Module::loadCatalog()
{
  RuntimeSALOME* aRuntimeSALOME = YACS::ENGINE::getSALOMERuntime();

  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  SALOME_NamingService* namingService = anApp->namingService();
  CORBA::Object_ptr obj = namingService->Resolve("/Kernel/ModulCatalog");
  SALOME_ModuleCatalog::ModuleCatalog_var aModuleCatalog = SALOME_ModuleCatalog::ModuleCatalog::_narrow(obj);
  if (CORBA::is_nil(aModuleCatalog)) {
    printf("ModuleCatalog is NULL\n");
    return;
  }

  CORBA::ORB_ptr orb = aRuntimeSALOME->getOrb();
  string anIOR = orb->object_to_string( aModuleCatalog );
  printf("ModuleCatalog - %s",anIOR.c_str());

  myCatalog = aRuntimeSALOME->loadCatalog( "session", anIOR );

  SALOME_ModuleCatalog::ListOfComponents_var aComponentList = aModuleCatalog->GetComponentList();
  long aNbComp = aComponentList->length();
  printf("Number of components - %d\n", aNbComp);
  for( int i = 0; i < aNbComp; i++ )
  {
    SALOME_ModuleCatalog::Acomponent_ptr aComponent = aModuleCatalog->GetComponent( (char*)aComponentList[i] );
    printf("Component N%d - %s ", i, (char*)aComponent->componentusername() );

    SALOME_ModuleCatalog::ListOfInterfaces* anInterfaceList = aComponent->GetInterfaceList();
    long aNbInterf = anInterfaceList->length();
    printf("(%d)\n", aNbInterf);
    for ( int j = 0; j < aNbInterf; j++ )
    {
      SALOME_ModuleCatalog::DefinitionInterface* anInterface = aComponent->GetInterface( (char*)(*anInterfaceList)[j] );
      printf("  Interface N%d - %s ", j, (char*)anInterface->interfacename );

      long aNbServices = anInterface->interfaceservicelist.length();
      printf("(%d)\n", aNbServices);
      for ( int k = 0; k < aNbServices; k++ )
      {
	SALOME_ModuleCatalog::Service* Service = &(anInterface->interfaceservicelist[k]);
	printf("    Service N%d - %s\n", k, (char*)Service->ServiceName );
      }
    }
  }
}

//! Create new instance of data model and return it.
/*!
 */
CAM_DataModel* YACSGui_Module::createDataModel()
{
  MESSAGE("YACSGui_Module::createDataModel");
  return new YACSGui_DataModel( this );
}

//! Retuns data model.
/*!
 */
YACSGui_DataModel* YACSGui_Module::getDataModel() const
{
  MESSAGE("YACSGui_Module::getDataModel");
  YACSGui_DataModel* aModel = dynamic_cast<YACSGui_DataModel*>( dataModel() );
  return aModel;
}

void YACSGui_Module::CreateNewSchema( QString& theName, YACS::ENGINE::Proc* theProc, const bool updateOB )
{
  YACSGui_DataModel* aModel = getDataModel();
  if (aModel)
  {
    aModel->createNewSchema( theName, theProc );
    if ( updateOB )
      updateObjBrowser();
  }
}

//! Load dataflow from file.
/*!
 *  \param setEditable - define if the dataflow will be editable.
 */
YACS::HMI::GuiContext* YACSGui_Module::ImportProcFromFile( const QString& theFilePath,
							   const YACSGui_Module::GuiMode theMode,
							   const bool setEditable,
							   bool fromSuperv )
{
  MESSAGE("YACSGui_Module::ImportProcFromFile");
  if (theFilePath.isEmpty())
    return 0;
  QString tmpFileName = theFilePath;

  if (fromSuperv)
    {
      YACSGui_ORB::YACSGui_Gen_ptr engineRef = InitYACSGuiGen(getApp());
      try
        {
          tmpFileName = engineRef->convertSupervFile(theFilePath);
        }
      catch(...)
        {
          cerr<<"Unexpected exception in convertSupervFile " <<endl;
          SUIT_MessageBox::warn1(getApp()->desktop(),
                                 tr("WARNING"),
                                 tr("Unexpected exception in convertSupervFile"),
                                 tr("BUT_OK"));
          return 0;
        }
    }
  
  //TD: Here check if the study already contains a graph imported from the same file
  
  YACSGui_Loader aLoader;
  GuiContext* aCProc = 0;
  
  try
    {
      Proc* aProc = 0;
      
      if (!tmpFileName.isEmpty())
        aProc = aLoader.load(tmpFileName);
      
      //TD: Check the result of file loading
      if (!aProc)
	{
	  SUIT_MessageBox::error1(getApp()->desktop(), 
				  tr("ERROR"), 
				  tr("MSG_IMPORT_FILE_ERROR"), 
				  tr("BUT_OK"));
	  return 0;
	}

      QString name = QFileInfo(theFilePath).fileName ();
      aProc->setName( name );

      QxGraph_ViewWindow* aVW = createNewViewManager( theMode, aProc );
      createInputPanel();
      
      if ( aVW )
      {
        int w, h;
        const YACSGui_Loader::PrsDataMap& aPrsData = aLoader.getPrsData( aProc, w, h );
        const YACSGui_Loader::PortLinkDataMap& aPortLinkData = aLoader.getPortLinkData( aProc );
        const YACSGui_Loader::LabelLinkDataMap& aLabelLinkData = aLoader.getLabelLinkData( aProc );
      
	// create GuiContext for Proc*
	aCProc = new GuiContext();
	GuiContext::setCurrent( aCProc );
	aCProc->setProc( aProc );
	// from now we have to use GuiContext* instead of Proc* in the YACSGui_Graph
	
	displayGraph( aCProc, aPrsData, aPortLinkData, aLabelLinkData );
      }
    }
  catch (YACS::Exception& ex)
    {
      cerr<<"YACSGui_Module::importSchema: " <<ex.what()<<endl;     
    }

  loadCatalog();

  return aCProc;
}

//! Returns or create executor for a given graph.
/*!
 */
YACSGui_Executor* YACSGui_Module::getExecutor( YACS::ENGINE::Proc* theProc )
{
  MESSAGE("YACSGui_Module::getExecutor");
  YACSGui_Executor* anExecutor = 0;
  
  ExecutorMap::iterator anIterator = myExecutors.find(theProc);
  if (anIterator != myExecutors.end())
    anExecutor = (*anIterator).second;
  
  if (!anExecutor)
    {
      anExecutor = new YACSGui_Executor(this, theProc);
      anExecutor->setGraph(activeGraph());
      myExecutors[theProc] = anExecutor;
    }
  
  return anExecutor;
}

//! Returns executor for active graph or nul if no active graph
/*!
 */
YACSGui_Executor* YACSGui_Module::findExecutor()
{
  MESSAGE("YACSGui_Module::findExecutor");
  YACSGui_Graph* aGraph = activeGraph();
  YACSGui_Executor* anExecutor = 0;
  Proc* aProc = 0;
  if (aGraph)
    aProc = aGraph->getProc();
  
  if (!aGraph || !aProc)
    {
      SUIT_MessageBox::warn1(getApp()->desktop(), 
			     tr("WARNING"), 
			     tr("MSG_NO_DATAFLOW_LOADED"), 
			     tr("BUT_OK"));
      return 0;
    }
  
  anExecutor = getExecutor(aProc);
  return anExecutor;
}

YACSGui_RunMode* YACSGui_Module::getRunMode(YACSGui_Executor* executor)
{
  RunModeMap::iterator anIterator = _runModeMap.find(executor);
  YACSGui_RunMode* aRunMode = 0;
  if (anIterator != _runModeMap.end())
    aRunMode = _runModeMap[executor];
  //SCRUTE(aRunMode);
  return aRunMode;
}

//! Fill the Input Panel with the property pages according to the Object Browser selection
/*!
 */
void YACSGui_Module::selectionChanged()
{
  DataObjectList objlist;
  getApp()->objectBrowser()->getSelected( objlist );

  // check if the current selection is a single selection
  // Template 1: it may be used in the future
  /*if ( objlist.count() == 1 )
  {
  }
  else if (...)
  {
  }*/

  // Template 2: it may be used in the future
  /*for ( DataObjectListIterator it( objlist ); it.current(); ++it )
  {
    LightApp_DataObject* obj = dynamic_cast<LightApp_DataObject*>( it.current() );
    if ( obj && getApp()->checkDataObject(obj) )
    {
      QString anEntry = obj->entry();
      printf("==> anEntry = %s\n",anEntry.latin1());
    }
  }*/
}

LightApp_Selection* YACSGui_Module::createSelection() const
{
  return new YACSGui_Selection();
}


void YACSGui_Module::onCreateExecution()
{
  DataObjectList objlist;
  getApp()->objectBrowser()->getSelected( objlist );

  // check if the current selection is a single selection
  if ( objlist.count() == 1 )
  {
    SalomeApp_DataObject* obj = dynamic_cast<SalomeApp_DataObject*>( objlist.getFirst() );
    if ( obj && getDataModel()->objectType( obj->entry() ) == YACSGui_DataModel::SchemaObject )
    {
      _PTR(SObject) sobj = obj->object();
      _PTR(GenericAttribute)   anAttr;
      _PTR(AttributeName)      aName;
      _PTR(AttributeParameter) aType;
      if ( sobj->FindAttribute( anAttr, "AttributeParameter" ) )
      {
        aType = _PTR(AttributeParameter) ( anAttr );
        QFile aSchemaFile( aType->GetString("FilePath") );
        
        if ( sobj->FindAttribute( anAttr, "AttributeName" )  )
        {
          aName = _PTR(AttributeName)( anAttr );
          QString aSchemaName = aName->Value();
	  QString anEditionName = "/tmp/YACSGui/" + aSchemaName + "-modified";

	  // check it is a first execution object created under the schema object
	  _PTR(Study) aStudy = (( SalomeApp_Study* )(getApp()->activeStudy()))->studyDS();
	  _PTR(ChildIterator) anIterator ( aStudy->NewChildIterator(sobj) );
	  bool noAnyRunObjects = true;
	  for (; anIterator->More(); anIterator->Next())
	  {
	    noAnyRunObjects = false;
	    break;
	  }

	  if ( !noAnyRunObjects )
	    // check if file for modified schema was created earlier
	    if ( QFile::exists(anEditionName) )
	      aSchemaFile.setName(anEditionName);
	  
          if ( aSchemaFile.open( IO_ReadOnly ) )
          {
            QString tmpDir = SALOMEDS_Tool::GetTmpDir();
            QDir aTmpDir( tmpDir );
            aTmpDir.mkdir( "YASCGui" );
	    aTmpDir.cd( "YASCGui", false );

            QDateTime curTime = QDateTime::currentDateTime();   
            QString aRunName = aSchemaName + "-" + curTime.toString( Qt::ISODate );
            QFile aRunFile( aTmpDir.absFilePath(aRunName) );
            if ( aRunFile.open( IO_WriteOnly ) )
            {
              QTextStream aSchemaTS( &aSchemaFile ),
                          aRunTS ( &aRunFile );
              while( !aSchemaTS.atEnd() )
                aRunTS << aSchemaTS.readLine() << "\n";
              aRunFile.close();
              aSchemaFile.close();

              //TODO: check for superv
              if ( GuiContext* aCProc = ImportProcFromFile( QFileInfo( aRunFile ).absFilePath(), RunMode,  false) )
		if ( Proc* aProc = aCProc->getProc() )
		{
		  getDataModel()->createNewRun( sobj, aProc );                
		  updateObjBrowser();
		  
		  // create  ViewManager for new the RunObject
		  //createNewViewManager( RunMode, aProc );
		  
		  // create and activate the run tree view for the create run object
		  createTreeView( aCProc->getSubjectProc(), YACSGui_DataModel::RunObject );
		}
            }
            else
            {
              printf( "Can't open temp run file: %s\n", QFileInfo( aRunFile ).absFilePath().latin1() );
            }
          }
          else
          {
            printf( "Can't open schema file: %s\n", QFileInfo( aSchemaFile ).absFilePath().latin1() );
          }
        }                 
      }
    }
  }
}

/*! Private slot. Create new execution in batch mode.
/*!
 */
void YACSGui_Module::onCreateBatchExecution()
{
  MESSAGE("YACSGui_Module::onCreateBatchExecution() --- NOT YET IMPLEMENTED!");
}

void YACSGui_Module::onReloadExecution()
{
  printf("YACSGui_Module::onReloadExecution - Not implemented yet");
  
  // switch to run mode
  setGuiMode(YACSGui_Module::RunMode);

  // TODO: create run object in the Object Browser
  // ... 

  // TODO: create 2D view presentation for schema run mode
  // ...
  
  // TODO: create and activate the run tree view for the imported schema
  // ...
}

/*! Private slot. Start execution.
/*!
 */
void YACSGui_Module::onExecute()
{
  MESSAGE("YACSGui_Module::onExecute()");
  YACSGui_Executor* anExecutor = findExecutor();
  if (anExecutor)
    if ( anExecutor->checkEndOfDataFlow(false) ) // finished or not started
    {
      if ( anExecutor->getCurrentExecMode() == YACSGui_ORB::STEPBYSTEP )
	anExecutor->setContinueMode();
      anExecutor->runDataflow(true); // remote run only
    }
    else // in progress (suspended or running)
    {
      SUIT_MessageBox::warn1(getApp()->desktop(), 
			     tr("WARNING"), 
			     tr("MSG_DATAFLOW_IS_CURRENTLY_RUNNING_ABORT"), 
			     tr("BUT_OK"));
      return;
    }
}

void YACSGui_Module::onExecuteStBy()
{
  MESSAGE("YACSGui_Module::onExecuteStBy()");
  YACSGui_Executor* anExecutor = findExecutor();
  if (anExecutor)
    if ( anExecutor->checkEndOfDataFlow(false) ) // finished or not started
    {
      anExecutor->setStepByStepMode();
      anExecutor->runDataflow(true); // remote run only
    }
    else // in progress (suspended or running)
    {
      SUIT_MessageBox::warn1(getApp()->desktop(), 
			     tr("WARNING"), 
			     tr("MSG_DATAFLOW_IS_CURRENTLY_RUNNING_ABORT"), 
			     tr("BUT_OK"));
      return;
    }
}

void YACSGui_Module::onExecuteInBatch()
{
  MESSAGE("YACSGui_Module::onExecuteInBatch() --- NOT YET IMPLEMENTED!");
}

void YACSGui_Module::onPause()
{
  MESSAGE("YACSGui_Module::onPause()");
  YACSGui_Executor* anExecutor = findExecutor();
  if (anExecutor)
    if ( !anExecutor->checkEndOfDataFlow(false) ) // in progress (suspended or running) 
      anExecutor->suspendDataflow();
}

void YACSGui_Module::onResume()
{
  MESSAGE("YACSGui_Module::onResume()");
  YACSGui_Executor* anExecutor = findExecutor();
  if (anExecutor)
    if ( !anExecutor->checkEndOfDataFlow(false) ) // in progress (suspended or running) 
      anExecutor->resumeDataflow();
}

/*void YACSGui_Module::onSuspendResume()
{
  MESSAGE("YACSGui_Module::onSuspendResume()");
  YACSGui_Executor* anExecutor = findExecutor();
  if (anExecutor)
    if ( !anExecutor->checkEndOfDataFlow(false) ) // in progress (suspended or running) 
      anExecutor->suspendResumeDataflow();
}*/

void YACSGui_Module::onAbort()
{
  MESSAGE("YACSGui_Module::onAbort()");
    YACSGui_Executor* anExecutor = findExecutor();
  if (anExecutor)
  {
    if (!anExecutor->isRunning())
    {
      SUIT_MessageBox::warn1(getApp()->desktop(), 
			     tr("WARNING"), 
			     tr("MSG_DATAFLOW_IS_NOT_RUNNING"), 
			     tr("BUT_OK"));
      return;
    }
    anExecutor->killDataflow();
  }
}

void YACSGui_Module::onReset()
{
  MESSAGE("YACSGui_Module::onReset()");
  YACSGui_Executor* anExecutor = findExecutor();
  if (anExecutor)
    if ( anExecutor->checkEndOfDataFlow(false) ) // finished or not started
    {
      YACS::ExecutorState aSchemaState = YACS::NOTYETINITIALIZED;
      YACS::StatesForNode aNodeState = YACS::INITED;
      
      YACSGui_RunTreeView* aRunTV = dynamic_cast<YACSGui_RunTreeView*>(activeTreeView());
      if ( !aRunTV ) return;

      if ( !myInputPanel ) return;

      // notify schema view item in the tree view
      aRunTV->onNotifyStatus(aSchemaState);
  
      set<Node*> aChildren = anExecutor->getProc()->getAllRecursiveConstituents();
      set<Node*>::iterator it = aChildren.begin();
      for ( ; it != aChildren.end(); it++ )
      {
	(*it)->setState( aNodeState );

	// notify node presentation in 2D view
	YACSGui_Graph* aGraph = anExecutor->getGraph();
	if ( aGraph )
	{
	  if ( YACSPrs_ElementaryNode* aNodePrs = aGraph->getItem( *it ) )
	    aNodePrs->update();
	  else
	  {
	    if ( dynamic_cast<ForEachLoop*>( (*it)->getFather() ) )
	    {
	      if ( YACSPrs_ElementaryNode* aFELNodePrs = aGraph->getItem( (*it)->getFather() ) )
	      {
		// transmit event to ForEachLoop node
		aFELNodePrs->update();

		// transmit event from the last clone node to original loop body node
		if ( (*it) == (dynamic_cast<ForEachLoop*>( (*it)->getFather() ))->getNodes().back() )
                {
                  set<Node *> aFELChildren = dynamic_cast<ForEachLoop*>( (*it)->getFather() )->edGetDirectDescendants();
                  for( set<Node *>::iterator iter = aFELChildren.begin(); iter != aFELChildren.end(); iter++ )
                    if ( YACSPrs_ElementaryNode* aFELBodyNodePrs = aGraph->getItem(*iter) )
		      aFELBodyNodePrs->updateForEachLoopBody(*it);
                }
	      }
	    }
	  }
	}

	// notify node view item in the tree view
	aRunTV->onNotifyNodeStatus( (*it)->getNumId(), aNodeState );

	// notify node property page
	myInputPanel->onNotifyNodeStatus( (*it)->getNumId(), aNodeState );
      }
    }
    else // in progress (suspended or running)
    {
      SUIT_MessageBox::warn1(getApp()->desktop(), 
			     tr("WARNING"), 
			     tr("MSG_DATAFLOW_IS_CURRENTLY_RUNNING"), 
			     tr("BUT_OK"));
      return;
    }
}

void YACSGui_Module::onSaveExecutionState()
{
  MESSAGE("YACSGui_Module:: onSaveExecutionState()");
  YACSGui_Executor* anExecutor = findExecutor();
  if (anExecutor)
  {
    printf(">> anExecutor->getExecutorState() = %d\n",anExecutor->getExecutorState());
    if ( anExecutor->getExecutorState() == YACS::NOTYETINITIALIZED
	 ||
	 anExecutor->getExecutorState() == YACS::PAUSED
	 ||
	 anExecutor->getExecutorState() == YACS::STOPPED
	 ||
	 anExecutor->getExecutorState() == YACS::FINISHED )
    {

      if ( !activeGraph() ) return;
      Proc* aProc = activeGraph()->getProc();
      if ( !aProc ) return;
      
      QString aDef = aProc->getName();
      aDef = aDef.left( aDef.findRev(".xml") ) + "_state.xml";
      QString aFileName = SUIT_FileDlg::getFileName( application()->desktop(), aDef, "*.xml", tr("TLT_SAVE_SCHEMA_STATE"), false );
      if (aFileName.isEmpty())
	return;
      
      anExecutor->saveState(aFileName.latin1());
    }
    else // in progress (suspended or running)
    {
      SUIT_MessageBox::warn1(getApp()->desktop(), 
			     tr("WARNING"), 
			     tr("MSG_DATAFLOW_IS_CURRENTLY_RUNNING"), 
			     tr("BUT_OK"));
      return;
    }
  }
}

void YACSGui_Module::onConnectToRunning()
{
  MESSAGE("YACSGui_Module:: onConnectToRunning()--- NOT YET IMPLEMENTED!");
}

void YACSGui_Module::onToggleStopOnError()
{
  MESSAGE("YACSGui_Module::onToggleStopOnError");
  YACSGui_Executor* anExecutor = findExecutor();
  if (anExecutor)
  {
    if ( !anExecutor->isStopOnError() )
      anExecutor->setStopOnError(true);
    else
      anExecutor->unsetStopOnError();
  }
}


/*!
  Adds tree view of a schema into the module internal structure.
  \param theTreeView - tree view to be added
  \param theStudyId - study id
  \param theId - key for window (i.e. the index of the corresponding view manager <=> 2D view presentation)
  theId used how identificator of tree view in the list.
*/
void YACSGui_Module::addTreeView( YACSGui_TreeView* theTreeView, 
				  const int theStudyId, 
				  const int theId )
{
  if ( !theTreeView )
    return;

  int sId = theStudyId;
  if ( theStudyId < 0 )
  {
    if ( !getApp()->activeStudy() )
      return;
    sId = getApp()->activeStudy()->id();
  }

  if ( myTreeViews.find( sId ) == myTreeViews.end() )
  { // there is no widget container for the given study => insert it
    LightApp_WidgetContainer* newWC = new LightApp_WidgetContainer( sId, getApp()->desktop() );
    //connect( newWC, SIGNAL(  destroyed ( QObject* ) ), this, SLOT( onWCDestroyed( QObject* ) ) );
    // the widget container is destroyed on study closing
    myTreeViews.insert( make_pair( sId, newWC ) );
    getApp()->desktop()->moveDockWindow( myTreeViews[sId], Qt::DockLeft );

    myTreeViews[sId]->setResizeEnabled( true );
    myTreeViews[sId]->setCloseMode( QDockWindow::Always );
    myTreeViews[sId]->setName( QString( "dock_window_%1_%2" ).arg( sId ).arg( theId ) );
    myTreeViews[sId]->setFixedExtentWidth( theTreeView->width() );
    myTreeViews[sId]->setFixedExtentHeight( theTreeView->height() );

    getApp()->desktop()->setAppropriate( myTreeViews[sId], false );
  }

  QFont f = theTreeView->font();

  myTreeViews[sId]->insert( theId, theTreeView );
  theTreeView->setFont(f);
}

/*!
  Returns tree view with the given index in the given study.
  \param theStudyId - study id
  \param theId - key for window
*/
YACSGui_TreeView* YACSGui_Module::treeView( const int theStudyId, const int theId ) const
{
  YACSGui_TreeView* aTV = 0;

  TreeViewMap::const_iterator anIt = myTreeViews.find( theStudyId );
  if ( anIt != myTreeViews.end() )
    aTV = dynamic_cast<YACSGui_TreeView*>( anIt->second->widget( theId ) );

  return aTV;
}


/*!
  Removes tree view with the given index in the given study (if, e.g., a schema was deleted from Object Browser).
  \param theStudyId - study id
  \param theId - key for window
*/
void YACSGui_Module::removeTreeView( const int theStudyId, const int theId )
{
  int sId = theStudyId;
  if ( theStudyId < 0 )
  {
    if ( !getApp()->activeStudy() )
      return;
    sId = getApp()->activeStudy()->id();
  }

  if ( myTreeViews.find( sId ) == myTreeViews.end() ) return;

  QWidget* wid = myTreeViews[sId]->widget( theId );
  if ( YACSGui_TreeView* aTV = dynamic_cast<YACSGui_TreeView*>(wid) )
    aTV->clearSelection();
  myTreeViews[sId]->remove( theId );
  delete wid;

  bool anIsEmpty = myTreeViews[sId]->isEmpty();
  QDockWindow* dw = myTreeViews[sId];
  if ( !anIsEmpty ) dw->show();
  else if ( dw->isShown() ) dw->hide();
}

/*!
  Returns true, if tree view with the given index in the given study is shown,
  and false - otherwise.
  \param theStudyId - study id
  \param theId - key for window
*/
bool YACSGui_Module::isTreeViewShown( const int theStudyId, const int theId )
{
  bool aRet = false;

  return aRet;
}

/*!
  Returns active tree view in the active study
*/
YACSGui_TreeView* YACSGui_Module::activeTreeView() const
{
  YACSGui_TreeView* aTV = 0;

  if ( !getApp()->activeStudy() )
    return aTV;
  
  int aStudyId = getApp()->activeStudy()->id();
  TreeViewMap::const_iterator anIt = myTreeViews.find( aStudyId );
  TreeViewActiveMap::const_iterator anActIt = myActiveTreeViews.find( aStudyId );
  if ( anIt != myTreeViews.end() && anActIt != myActiveTreeViews.end() )
    aTV = dynamic_cast<YACSGui_TreeView*>( anIt->second->widget( anActIt->second ) );
  
  return aTV;
}

/*!
  Returns active tree view in the given study
*/
YACSGui_TreeView* YACSGui_Module::activeTreeView( const int theStudyId ) const
{
  YACSGui_TreeView* aTV = 0;

  TreeViewMap::const_iterator anIt = myTreeViews.find( theStudyId );
  TreeViewActiveMap::const_iterator anActIt = myActiveTreeViews.find( theStudyId );
  if ( anIt != myTreeViews.end() && anActIt != myActiveTreeViews.end() )
    aTV = dynamic_cast<YACSGui_TreeView*>( anIt->second->widget( anActIt->second ) );
  
  return aTV;
}

/*!
  Set the given tree view as active
*/
void YACSGui_Module::activateTreeView( const int theStudyId, const int theId )
{
  // hide Input Panel of the selected item in the current active tree view
  setInputPanelVisibility(false);

  TreeViewMap::const_iterator anIt = myTreeViews.find( theStudyId );
  if ( anIt == myTreeViews.end() )
    return;

  anIt->second->activate( theId );
  anIt->second->show();

  // change the content of active tree view map
  if ( myActiveTreeViews.find(theStudyId) == myActiveTreeViews.end() )
    myActiveTreeViews.insert( make_pair( theStudyId, theId ) );
  else
    myActiveTreeViews[theStudyId] = theId;
  
  // show Input Panel of the selected item in the new active tree view
  setInputPanelVisibility(true);
}

/*!
  Show or hide Input Panel of the selected item in the current active tree view
*/
void YACSGui_Module::setInputPanelVisibility( const bool theOn )
{
  YACSGui_TreeView* anATV = activeTreeView();
  if ( !anATV ) return;

  // get the list of items selected in the current active edition tree view
  list<QListViewItem*> aSelList = anATV->getSelected();
  
  // check if the current selection is not empty (if it is true => the Input Panel is currently visible => hide it)
  if ( !aSelList.empty() && myInputPanel ) 
    if ( theOn ) anATV->syncPageTypeWithSelection();
    else myInputPanel->hide();
}

/*!
  Connects arrange nodes algorithm to the signal emitted when the size (the height in fact) of node presentation is changed
*/
//void YACSGui_Module::connectArrangeNodes( YACSPrs_ElementaryNode* theItem )
//{
//  connect( theItem, SIGNAL(portsChanged()), this, SLOT(onArrangeNodes()) );
//}

/*!
  Sets current selected schema as current
*/
void YACSGui_Module::onSetActive()
{
  printf( "YACSGui_Module::onSetActive() not implemented yet\n" );
}

//! Public. Set gui mode by ObjectType
/*!
 */
void YACSGui_Module::setGuiMode( YACSGui_DataModel::ObjectType theType )
{
  switch( theType )
  {
  case YACSGui_DataModel::SchemaObject:
    {
      setGuiMode( YACSGui_Module::EditMode );
      break;
    }
  case YACSGui_DataModel::RunObject:
    {
      setGuiMode( YACSGui_Module::RunMode );
      break;
    }
  default: //UnknownObject
    setGuiMode( YACSGui_Module::EmptyMode );
  }
}

//! Public. Set gui mode to theMode 
/*!
 */
void YACSGui_Module::setGuiMode(YACSGui_Module::GuiMode theMode)
{
  MESSAGE("YACSGui_Module::setGuiMode()");
  if (theMode == YACSGui_Module::EmptyMode)
  {
    setBaseActionShown(false);
    setModifyActionShown(false);
    setRunActionShown(false);
    setVisualizationActionShown(false);
    myGuiMode = theMode;
    return;
  }
  SCRUTE(theMode);
  SCRUTE(getGuiMode());
  
  if(theMode == getGuiMode())
    return;
  else
  {
    setBaseActionShown(true);
    switch(theMode)
    {
    case YACSGui_Module::InitMode:
      setModifyActionShown(false);
      setRunActionShown(false);
      setVisualizationActionShown(false);
      myGuiMode = theMode;
      break;
    case YACSGui_Module::EditMode:
      setModifyActionShown(true);
      setRunActionShown(false);
      setVisualizationActionShown(true);
      myGuiMode = theMode;
      break;
    case YACSGui_Module::RunMode:
      setModifyActionShown(false);
      setRunActionShown(true);
      setVisualizationActionShown(true);
      myGuiMode = theMode;
      break;
    default: printf("YACSGui_Module::setGuiMode(): incorrect gui mode\n");
      break;
    }
  }
}

YACSGui_Module::GuiMode YACSGui_Module::getGuiMode() const
{
  return myGuiMode;
}

//! Private. Show/Hide modification action in the toolbars and menus
/*!
 */
void YACSGui_Module::setModifyActionShown(const bool on)
{
  //For menus
    //Menu "File"
  setMenuShown( ExportSchemaId,on );

    //Menu "Create"
  setMenuShown( NewContainerId,on );
  setMenuShown( NewSalomeComponentId,on );
  setMenuShown( NewSalomePythonComponentId,on );
  setMenuShown( NewCorbaComponentId,on );
  setMenuShown( NewSalomeServiceNodeId, on );
  setMenuShown( NewCorbaServiceNodeId, on );
  setMenuShown( NewNodeNodeServiceNodeId,on );
  setMenuShown( NewServiceInlineNodeId,on );
  setMenuShown( NewXmlNodeId,on );
  setMenuShown( NewInlineScriptNodeId,on );
  setMenuShown( NewInlineFunctionNodeId,on );
  setMenuShown( NewBlockNodeId,on );
  setMenuShown( NewForLoopNodeId,on );
  setMenuShown( NewForeachLoopNodeId,on );
  setMenuShown( NewWhileLoopNodeId,on );
  setMenuShown( NewSwitchLoopNodeId,on );
  setMenuShown( NewFromLibraryNodeId,on );
  setMenuShown( NewLinkId, on );
  
    //Menu "Execute"
  setMenuShown( NewExecutionId, on );
  setMenuShown( NewBatchExecutionId, on );
  setMenuShown( ReloadExecutionId, on);

  //For toolbars
    //"Schema" and "Create" toolbars
  action(ExportSchemaId)->setVisible(on);
  action(NewContainerId)->setVisible(on);
  action(NewLinkId)->setVisible(on);
  
  if(on == true) {
    myToolButtons[NewContainerBtnId]->show();
    myToolButtons[NewNodeBtn]->show();
  }
  else {
    myToolButtons[NewContainerBtnId]->hide();
    myToolButtons[NewNodeBtn]->hide();
  }
  
    //"Execute" toolbar
  setToolShown( NewExecutionId, on );
  setToolShown( NewBatchExecutionId, on );
  setToolShown( ReloadExecutionId, on);
}

//! Private. Show/Hide visualization actions in the toolbar
/*!
 */
void YACSGui_Module::setVisualizationActionShown(const bool on)
{
    //"Visualization" toolbar
  setToolShown( FullViewId, on );
  setToolShown( ControlViewId, on );
  setToolShown( DataFlowViewId, on );
  setToolShown( DataStreamViewId, on );
  setToolShown( ArrangeNodesId, on );
  setToolShown( RebuildLinksId, on);
}

//! Private. Show/Hide Base actions(create schema and import dataflow) in the toolbars and menus
/*!
 */
void YACSGui_Module::setBaseActionShown(const bool on)
{
  action(NewSchemaId)->setVisible(on);
  action(ImportSchemaId)->setVisible(on);
  action(ImportSupervSchemaId)->setVisible(on);
  if(on)
    myToolButtons[ImportBtnId]->show();
  else
    myToolButtons[ImportBtnId]->hide();
}

//! Private. Show/Hide run actions in the toolbars and menus
/*!
 */
void YACSGui_Module::setRunActionShown(const bool on)
{
  //For menus
     //Menu "Execute"
  setMenuShown( ExecuteId, on );
  setMenuShown( ExecuteStById, on );
  setMenuShown( ExecuteInBatchId, on );
  setMenuShown( PauseId, on );
  setMenuShown( ResumeId, on );
  setMenuShown( AbortId, on);
  setMenuShown( ResetId, on );
  //setMenuShown( StopOnErrorId, on );
  setMenuShown( SaveExecutionStateId, on );
  setMenuShown( ConnectToRunningId, on );
  setMenuShown( ToggleStopOnErrorId, on );
  
  //For toolbars
     //"Visualization" toolbar
  setToolShown( FullViewId, on );
  setToolShown( ControlViewId, on );
  setToolShown( DataFlowViewId, on );
  setToolShown( DataStreamViewId, on );
  setToolShown( ArrangeNodesId, on );
  setToolShown( RebuildLinksId, on);
  
     //"Execute" toolbar
  setToolShown( ExecuteId, on );
  setToolShown( ExecuteStById, on );
  setToolShown( ExecuteInBatchId, on );
  setToolShown( PauseId, on );
  setToolShown( AbortId, on);
  setToolShown( ResumeId, on );
  setToolShown( ResetId, on );
  //setToolShown( StopOnErrorId, on );
  setToolShown( SaveExecutionStateId, on );
  setToolShown( ConnectToRunningId, on );
  setToolShown( ToggleStopOnErrorId, on );
  setToolShown( SuspendResumeDataflowId, on);
}

QxGraph_ViewWindow* YACSGui_Module::createNewViewManager(  YACSGui_Module::GuiMode theMode,
                                                           Proc* theProc,
                                                           int width,
                                                           int height )
{
  // switch to theMode
  setGuiMode( theMode );
      
  // create 2D view of a schema or a run obejct
  SUIT_ViewManager* aVM  = getApp()->createViewManager( QxGraph_Viewer::Type() );
  connect( aVM,  SIGNAL(deleteView(SUIT_ViewWindow*)),
	   this, SLOT(onWindowClosed(SUIT_ViewWindow*)) );

  // set caption for graph's view window
  QxGraph_ViewWindow* aVW = dynamic_cast<QxGraph_ViewWindow*>( aVM->getActiveView() );
  if ( aVW && theProc)
  {
    aVW->setCaption( theProc->getName() );
    myWindowsMap.insert( make_pair( theProc, aVW ) );
  }

  if ( width > 0 && height > 0 )
    aVW->getViewModel()->getCanvas()->resize( width, height );

  return aVW;
}

void YACSGui_Module::activateViewWindow( Proc* theProc )
{
  printf( "YACSGui_Module::activateViewWindow\n" );
  if ( theProc  )
  {
    WindowsMap::iterator anIterator = myWindowsMap.find(theProc);
    if (anIterator != myWindowsMap.end())
    {
      printf(">> Proc found!!!\n");
      QxGraph_ViewWindow* aVW = (*anIterator).second;
      if ( aVW )
      {
        myBlockWindowActivateSignal = true;
        aVW->setFocus();
        myBlockWindowActivateSignal = false;
	
	if ( YACSGui_Graph* aGraph = activeGraph() )
	  GuiContext::setCurrent( aGraph->getContext() );
      }
    }
  }
}

void YACSGui_Module::onWindowActivated( SUIT_ViewWindow* theVW )
{  
  printf( "YACSGui_Module::onWindowActivated( SUIT_ViewWindow* theVW )\n" );

  if ( myBlockWindowActivateSignal )
  {
    printf( "Blocked!!!\n" );
    return;
  }
  
  YACSGui_DataModel* model = getDataModel();
  
  if ( !model )
    return;

  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(getApp()->activeStudy() );
  if ( !study )
    return;

  if ( QxGraph_ViewWindow* aVW = dynamic_cast<QxGraph_ViewWindow*>( theVW ) )
  {
    WindowsMap::iterator anIt = myWindowsMap.begin();
    while( anIt != myWindowsMap.end()  ) // TODO: replace the cycle with the approach used in the getGraph(...) method (see also onNewSchema() method)
    {
      if( (*anIt).second == theVW )
      {
        //get GUI mode
        SalomeApp_DataObject* sobj = dynamic_cast<SalomeApp_DataObject*>( model->getDataObject( (*anIt).first ) );
        if ( sobj )
        {
          setGuiMode( model->objectType( sobj->entry() ) );
          getApp()->objectBrowser()->setSelected( sobj );
        }

	if ( YACSGui_Graph* aGraph = getGraph( (*anIt).first ) )
	  GuiContext::setCurrent( aGraph->getContext() );
	
        int aStudyId = study->id();
        int aVMId = viewManagerId( (*anIt).first );
        activateTreeView( aStudyId, aVMId );

        return;
      }
      anIt++;
    }
  }
}

void YACSGui_Module::onWindowClosed( SUIT_ViewWindow* theVW )
{
  if ( QxGraph_ViewWindow* aVW = dynamic_cast<QxGraph_ViewWindow*>( theVW ) )
  {
    WindowsMap::iterator anIt = myWindowsMap.begin();
    while( anIt != myWindowsMap.end()  ) // TODO: replace the cycle with the approach used in the getGraph(...) method (see also onNewSchema() method)
    {
      if( (*anIt).second == aVW )
      {
	if ( !getApp()->activeStudy() ) return;
	removeTreeView( getApp()->activeStudy()->id(), viewManagerId( (*anIt).first ) );
	myWindowsMap.erase( anIt );

	if ( YACSGui_Graph* aGraph = activeGraph() )
	  GuiContext::setCurrent( aGraph->getContext() );
	
	return;
      }
      anIt++;
    }
  }
}

//! Export the current active schema (modified schema) into the temporary XML file, which will be used for create execution objects
/*!
 */
void YACSGui_Module::temporaryExport()
{
  // get active schema
  YACSGui_Graph* aGraph = activeGraph();
  if ( !aGraph ) return;

  Proc* aSchema = aGraph->getProc();
  if ( !aSchema ) return;

  QString aSchemaName(aSchema->getName());

  QString tmpDir("/tmp/YACSGui");
  QDir aTmpDir( tmpDir );
  if ( !aTmpDir.exists() )
  {
    aTmpDir = QDir("/tmp");
    aTmpDir.mkdir( "YACSGui" );
    aTmpDir.cd( "YACSGui", false );
  }
  
  // the temporary file name for modified schema
  QString anEditionName = aSchemaName + "-modified";
  if ( aTmpDir.exists(anEditionName) )
    aTmpDir.remove(anEditionName);

  anEditionName = aTmpDir.absFilePath(anEditionName);

  YACSGui_VisitorSaveSchema aWriter( this, aSchema );
  aWriter.openFileSchema( anEditionName );
  aWriter.visitProc( aSchema );
  aWriter.closeFileSchema();
}

//! Returns a kind of a node as a string, which is using in the built-in catalog node maps
/*!
 */
std::string YACSGui_Module::getNodeType( YACS::ENGINE::Node* theNode )
{
  //if ( getApp()->activeModule() != this ) return "";    

  ServiceNode* aSNode = dynamic_cast<ServiceNode*>(theNode);
  if ( aSNode )
  {
    ComponentInstance* aComp = aSNode->getComponent();
    if ( aComp && !aComp->getName().empty() )
      return ""; // the given node is not an empty template
  }
  
  YACS::ENGINE::Catalog* aCatalog = YACS::ENGINE::getSALOMERuntime()->getBuiltinCatalog();
  if ( !aCatalog ) return "";

  map<string,Node*>::iterator it =  aCatalog->_nodeMap.begin();
  for ( ; it != aCatalog->_nodeMap.end(); it++ )
  {
    printf("==> _nodeMap : type : %d\n",ProcInvoc::getTypeOfNode((*it).second));
    if ( ProcInvoc::getTypeOfNode((*it).second) == ProcInvoc::getTypeOfNode(theNode) )
      return (*it).first;
  }
    
  map<string,ComposedNode*>::iterator itC =  aCatalog->_composednodeMap.begin();
  for ( ; itC != aCatalog->_composednodeMap.end(); itC++ )
  {
    printf("==> _composednodeMap : type : %d\n",ProcInvoc::getTypeOfNode((*itC).second));
    if ( ProcInvoc::getTypeOfNode((*itC).second) == ProcInvoc::getTypeOfNode(theNode) )
      return (*itC).first;
  }

  return "";
}

std::string YACSGui_Module::getNodeType( YACS::HMI::TypeOfElem theElemType )
{
  string aType = "";
  if ( myNodeTypeMap.find(theElemType) != myNodeTypeMap.end() )
    aType = myNodeTypeMap[theElemType];
  return aType;
}

void YACSGui_Module::fillNodeTypesMap()
{
  myNodeTypeMap[BLOC]             = getNodeType(new Bloc(""));
  myNodeTypeMap[FOREACHLOOP]      = getNodeType(new ForEachLoop("",Runtime::_tc_double));
  myNodeTypeMap[FORLOOP]          = getNodeType(new ForLoop(""));
  myNodeTypeMap[WHILELOOP]        = getNodeType(new WhileLoop(""));
  myNodeTypeMap[SWITCH]           = getNodeType(new Switch(""));
  myNodeTypeMap[PYTHONNODE]       = getNodeType(new PythonNode(""));
  myNodeTypeMap[PYFUNCNODE]       = getNodeType(new PyFuncNode(""));
  myNodeTypeMap[CORBANODE]        = getNodeType(new CORBANode(""));
  myNodeTypeMap[SALOMENODE]       = getNodeType(new SalomeNode(""));
  myNodeTypeMap[CPPNODE]          = getNodeType(new CppNode(""));
  myNodeTypeMap[SALOMEPYTHONNODE] = getNodeType(new SalomePythonNode(""));
  myNodeTypeMap[XMLNODE]          = getNodeType(new XmlNode(""));
}

void YACSGui_Module::createNode( YACS::HMI::TypeOfElem theElemType )
{
  YACS::ENGINE::Catalog* aCatalog = YACS::ENGINE::getSALOMERuntime()->getBuiltinCatalog();
  if ( !aCatalog ) return;

  if( YACSGui_EditionTreeView* anETV = dynamic_cast<YACSGui_EditionTreeView*>( activeTreeView() ) )
    if ( Subject* aSub = anETV->getSelectedSubject() )
    {
      if ( SubjectSwitch* aSwitch = dynamic_cast<SubjectSwitch*>(aSub) )
      {
	stringstream aName;
	string aType = getNodeType(theElemType);
	aName << aType << GuiContext::getCurrent()->getNewId(theElemType);
	
	map<int, SubjectNode*> bodyMap = aSwitch->getBodyMap();
	int aSwCase = 0;
	if (bodyMap.empty()) aSwCase = 1;
	else
	{
	  map<int, SubjectNode*>::reverse_iterator rit = bodyMap.rbegin();
	  aSwCase = (*rit).first + 1;
	}
	aSwitch->addNode(aCatalog, "", aType, aName.str(), aSwCase);
	temporaryExport();
      }
      //else if ( SubjectForLoop* aFor = dynamic_cast<SubjectForLoop*>(aSub) ... )
      //{ ... }
      else if ( SubjectComponent* aSComp = dynamic_cast<SubjectComponent*>(aSub) )
      {
	ComponentInstance* aComp = aSComp->getComponent();
	if ( !aComp || aComp->getKind() != SalomeComponent::KIND ) return;

	if ( SubjectProc* aSchema = GuiContext::getCurrent()->getSubjectProc() )
	  if ( theElemType == SALOMENODE || theElemType == SALOMEPYTHONNODE )
	  {
	    stringstream aName;
	    string aType = getNodeType(theElemType);
	    aName << aType << GuiContext::getCurrent()->getNewId(theElemType);
	    
	    if ( SubjectServiceNode* aSNode = dynamic_cast<SubjectServiceNode*>(aSchema->addNode(aCatalog, "", aType, aName.str())) )
	      aSNode->associateToComponent(aSComp);
	    
	    temporaryExport();
	  }
      }
      else if ( SubjectProc* aSchema = dynamic_cast<SubjectProc*>(aSub) )
      {
	stringstream aName;
	string aType = getNodeType(theElemType);
	aName << aType << GuiContext::getCurrent()->getNewId(theElemType);
	
	aSchema->addNode(aCatalog, "", aType, aName.str());
	temporaryExport();
      }
    }
    else if ( SubjectProc* aSchema = GuiContext::getCurrent()->getSubjectProc() )
    {  
      stringstream aName;
      string aType = getNodeType(theElemType);
      aName << aType << GuiContext::getCurrent()->getNewId(theElemType);
      
      aSchema->addNode(aCatalog, "", aType, aName.str());
      temporaryExport();
    }
}
