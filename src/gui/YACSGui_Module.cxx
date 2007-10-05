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
#include <YACSGui_DataObject.h>
#include <YACSGui_Executor.h>
#include <YACSGui_XMLDriver.h>
#include <YACSGui_Observer.h>
#include "YACSGui_RunMode.h"

#include <YACSPrs_ElementaryNode.h>
#include <YACSPrs_BlocNode.h>
#include <YACSPrs_Link.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_FileDlg.h>
#include <SUIT_MessageBox.h>
#include <CAM_Application.h>
#include <SalomeApp_Application.h>
#include <QxGraph_ViewModel.h>
#include <QxGraph_ViewWindow.h>
#include <QxGraph_ViewManager.h>
#include <QxGraph_Canvas.h> // for test presentations
#include <QxGraph_Prs.h> // for test presentations

#include <Node.hxx>
#include <SALOME_LifeCycleCORBA.hxx>

#include <qapplication.h>

#include <iostream> // for debug only
#include "utilities.h"

using namespace YACS;
using namespace YACS::ENGINE;


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
  InitYACSGuiGen( dynamic_cast<SalomeApp_Application*>( theApp ) );

  createActions();
  createMenus();
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
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_NEW_DATAFLOW"));
  createAction( NewDataflowId, tr("TOP_NEW_DATAFLOW"), QIconSet(aPixmap),
                tr("MEN_NEW_DATAFLOW"), tr("STB_NEW_DATAFLOW"), 
		0, aDesktop, false, this, SLOT(onNewDataflow()));
  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_MODIFY_DATAFLOW"));
  createAction( ModifyDataflowId, tr("TOP_MODIFY_DATAFLOW"), QIconSet(aPixmap),
                tr("MEN_MODIFY_DATAFLOW"), tr("STB_MODIFY_DATAFLOW"), 
		0, aDesktop, false, this, SLOT(onModifyDataflow()));
  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_MODIFY_SUPERV_DATAFLOW"));
  createAction( ModifySupervDataflowId, tr("TOP_MODIFY_SUPERV_DATAFLOW"), QIconSet(aPixmap),
                tr("MEN_MODIFY_SUPERV_DATAFLOW"), tr("STB_MODIFY_SUPERV_DATAFLOW"), 
		0, aDesktop, false, this, SLOT(onModifySupervDataflow()));
  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_IMPORT_DATAFLOW"));
  createAction( ImportDataflowId, tr("TOP_IMPORT_DATAFLOW"), QIconSet(aPixmap),
                tr("MEN_IMPORT_DATAFLOW"), tr("STB_IMPORT_DATAFLOW"), 
		0, aDesktop, false, this, SLOT(onImportDataflow()));
  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_IMPORT_SUPERV_DATAFLOW"));
  createAction( ImportSupervDataflowId, tr("TOP_IMPORT_SUPERV_DATAFLOW"), QIconSet(aPixmap),
                tr("MEN_IMPORT_SUPERV_DATAFLOW"), tr("STB_IMPORT_SUPERV_DATAFLOW"), 
		0, aDesktop, false, this, SLOT(onImportSupervDataflow()));
  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_EXPORT_DATAFLOW"));
  createAction( ExportDataflowId, tr("TOP_EXPORT_DATAFLOW"), QIconSet(aPixmap),
                tr("MEN_EXPORT_DATAFLOW"), tr("STB_EXPORT_DATAFLOW"), 
		0, aDesktop, false, this, SLOT(onExportDataflow()));
  
  // Menu "Supervisor"
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_ADD_NODE"));
  createAction( AddNodeId, tr("TOP_ADD_NODE"), QIconSet(aPixmap),
                tr("MEN_ADD_NODE"), tr("STB_ADD_NODE"), 
		0, aDesktop, false, this, SLOT(onAddNode()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_SUSPEND_RESUME_DATAFLOW"));
  createAction( SuspendResumeDataflowId, tr("TOP_SUSPEND_RESUME_DATAFLOW"), QIconSet(aPixmap),
                tr("MEN_SUSPEND_RESUME_DATAFLOW"), tr("STB_SUSPEND_RESUME_DATAFLOW"), 
		0, aDesktop, false, this, SLOT(onSuspendResumeDataflow()));

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_RUN_DATAFLOW"));
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
  
  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_ADD_DATAFLOW_IN_STUDY"));
  createAction( AddDataflowInStudyId, tr("TOP_ADD_DATAFLOW_IN_STUDY"), QIconSet(aPixmap),
                tr("MEN_ADD_DATAFLOW_IN_STUDY"), tr("STB_ADD_DATAFLOW_IN_STUDY"), 
		0, aDesktop, false, this, SLOT(onAddDataflowInStudy()));
  
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

  aPixmap = aResourceMgr->loadPixmap("YACSGui", tr("ICON_REBUILD_LINKS"));
  createAction( RebuildLinksId, tr("TOP_REBUILD_LINKS"), QIconSet(aPixmap),
                tr("MEN_REBUILD_LINKS"), tr("STB_REBUILD_LINKS"), 
		0, aDesktop, false, this, SLOT(onRebuildLinks()));

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
  
}

//! Creates module menus.
/*!
 */
void YACSGui_Module::createMenus()
{
  MESSAGE("YACSGui_Module::createMenus");
  int fileId = createMenu( tr( "MEN_FILE" ), -1, -1 );
  createMenu( NewDataflowId,    fileId, 10 );
  createMenu( ModifyDataflowId, fileId, 10 );
  createMenu( ModifySupervDataflowId, fileId, 10 );
  createMenu( ImportDataflowId, fileId, 10 );
  createMenu( ImportSupervDataflowId, fileId, 10 );
  createMenu( ExportDataflowId, fileId, 10 );

  int supervisorId = createMenu( tr( "MEN_SUPERVISOR" ), -1, -1, 10 );
  createMenu( AddNodeId,               supervisorId, -1 );
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
  createMenu( RebuildLinksId,          supervisorId, -1 );

  int dataflowTbId = createTool( tr("TOOLBAR_DATAFLOW") );
  createTool( NewDataflowId,          dataflowTbId );
  createTool( ModifyDataflowId,       dataflowTbId );
  createTool( ModifySupervDataflowId, dataflowTbId );
  createTool( ImportDataflowId,       dataflowTbId );
  createTool( ImportSupervDataflowId, dataflowTbId );
  createTool( ExportDataflowId,       dataflowTbId );
  createTool( separator(),            dataflowTbId );
  createTool( AddDataflowInStudyId,   dataflowTbId );
  createTool( separator(),            dataflowTbId );
  createTool( ReloadDataflowId,       dataflowTbId );
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
  createTool( ToggleStopOnErrorId,     executionTbId );
}

//! Defines which windows should be created.
/*!
 */
void YACSGui_Module::windows( QMap<int, int>& mappa ) const
{
  MESSAGE("YACSGui_Module::windows");
  mappa.insert( LightApp_Application::WT_ObjectBrowser, Qt::DockLeft );
  mappa.insert( LightApp_Application::WT_PyConsole, Qt::DockBottom );
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

  RuntimeSALOME::setRuntime();
  return bOk;
}

//! Inherited public slot. Called on a module deactivating.
/*!
 */
bool YACSGui_Module::deactivateModule( SUIT_Study* theStudy )
{
  MESSAGE("YACSGui_Module::deactivateModule");
  setMenuShown( false );
  setToolShown( false );
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
}

//! Gets a reference to the module's engine
/*!
 */
YACSGui_ORB::YACSGui_Gen_ptr YACSGui_Module::InitYACSGuiGen( SalomeApp_Application* app )
{
  MESSAGE("YACSGui_Module::InitYACSGuiGen");
  Engines::Component_var comp = app->lcc()->FindOrLoad_Component( "YACSContainer","YACSGui" );
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
    YACS::ENGINE::Proc* aProc = 0;
    YACSGui_Loader::PrsDataMap aPrsData;
    YACSGui_Loader::PortLinkDataMap aPortLinkData;
    YACSGui_Loader::LabelLinkDataMap aLabelLinkData;
    displayGraph( aProc, aPrsData, aPortLinkData, aLabelLinkData );
  }
}

//! Private slot. Creates a new empty dataflow.
/*!
 */
void YACSGui_Module::onNewDataflow()
{
  MESSAGE("YACSGui_Module::onNewDataflow");
  printf("YACSGui_Module::newDataflow\n");
  SUIT_ViewManager* aVM  = getApp()->createViewManager( QxGraph_Viewer::Type() );
  createGraph(aVM);
}

//! Private slot. Imports uneditable dataflow from the XML file(choosing via a file dialog)
/*!
 */
void YACSGui_Module::onImportDataflow()
{
  MESSAGE("YACSGui_Module::onImportDataflow");
  ImportDataflowFromFile(false);
}
 
//! Private slot. Imports editable dataflow from the XML file(choosing via a file dialog)
/*!
 */
void YACSGui_Module::onModifyDataflow()
{
  MESSAGE("YACSGui_Module::onModifyDataflow");
  ImportDataflowFromFile(true);
}

//! Private slot. Imports uneditable SUPERV dataflow from the XML file(choosing via a file dialog)
/*!
 */
void YACSGui_Module::onImportSupervDataflow()
{
  MESSAGE("YACSGui_Module::onImportSupervDataflow");
  ImportDataflowFromFile(false, true);
}
 
//! Private slot. Imports editable SUPERV dataflow from the XML file(choosing via a file dialog)
/*!
 */
void YACSGui_Module::onModifySupervDataflow()
{
  MESSAGE("YACSGui_Module::onModifySupervDataflow");
  ImportDataflowFromFile(true, true);
}

//! Private slot. Exports current dataflow to XML file.
/*!
 */
void YACSGui_Module::onExportDataflow()
{
  MESSAGE("YACSGui_Module::onExportDataflow");
  if ( !activeGraph() )
    return;
  Proc* aProc = activeGraph()->getProc();
  if ( !aProc )
    return;

  QString aFileName = SUIT_FileDlg::getFileName( application()->desktop(), aProc->getName(), "*.xml", tr("TLT_EXPORT_DATAFLOW"), false );
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

//! Private slot. Rebuilds dataflow links.
/*!
 */
void YACSGui_Module::onRebuildLinks()
{
  MESSAGE("YACSGui_Module::onRebuildLinks");
  YACSGui_Graph* aGraph = activeGraph();
  if (aGraph) aGraph->rebuildLinks();
}

//! Private slot. Open dialog for add node to current dataflow.
/*!
 */
void YACSGui_Module::onAddNode()
{
  MESSAGE("YACSGui_Module::onAddNode");
  YACSGui_Graph* aGraph = activeGraph();
  if (!aGraph)
    {
      SUIT_MessageBox::warn1(getApp()->desktop(), 
			     tr("WARNING"), 
			     tr("MSG_NO_DATAFLOW_SELECTED"), 
			     tr("BUT_OK"));
      return;
    }

  YACSGui_DataModel* aModel = getDataModel();
  if ( aModel && !aModel->isEditable(activeGraph()->getProc()) )
    {
      SUIT_MessageBox::warn1(getApp()->desktop(), 
			     tr("WARNING"), 
			     tr("MSG_DATAFLOW_NOT_EDITABLE"), 
			     tr("BUT_OK"));
      return;
    }
}
 
//! Private slot. Starts dataflow execution.
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
    }
  anExecutor->stopDataflow();
  //myExecutors[aProc] = 0; //@
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

void YACSGui_Module::onToggleStopOnError()
{
  MESSAGE("YACSGui_Module::onToggleStopOnError");
  YACSGui_Executor* anExecutor = findExecutor();
  if (anExecutor)
    {
      anExecutor->setStopOnError(true);
    }
}

//! Private slot. Add current dataflow to study.
/*!
 */
void YACSGui_Module::onAddDataflowInStudy()
{
  MESSAGE("YACSGui_Module::onAddDataflowInStudy");
  YACSGui_Graph* aGraph = activeGraph();

  if (aGraph) {
    Proc* aProc = aGraph->getProc();
    if (!aProc)
      aProc = new Proc("NewDataflow");
    YACSGui_DataModel* aModel = getDataModel();
    if (aModel)
      aModel->addData(aProc);
  }
}

//! Private slot. TD: what's this ?
/*!
 */
void YACSGui_Module::onChangeInformations()
{
  MESSAGE("YACSGui_Module::onChangeInformations --- NOT YET IMPLEMENTED!");
}

//! Private slot. Switch current dataflow to full view mode.
/*!
 */
void YACSGui_Module::onFullView()
{
  MESSAGE("YACSGui_Module::onFullView --- NOT YET IMPLEMENTED!");
}

//! Private slot.  Switch current dataflow to control view mode.
/*!
 */
void YACSGui_Module::onControlView()
{
  MESSAGE("YACSGui_Module::onControlView --- NOT YET IMPLEMENTED!");
}

//! Private slot.  Switch current dataflow to table view mode.
/*!
 */
void YACSGui_Module::onTableView()
{
  MESSAGE("YACSGui_Module::onTableView --- NOT YET IMPLEMENTED!");
}

//! Creates and displays presentation of a calculation graph.
/*!
 *  \param theGraph - graph to be displayed
 *  \param thePrsData - map of node coordinates and dimensions
 */
YACSGui_Graph* YACSGui_Module::displayGraph( YACS::ENGINE::Proc* theGraph, 
					     const YACSGui_Loader::PrsDataMap& thePrsData,
					     const YACSGui_Loader::PortLinkDataMap& thePortLinkData,
					     const YACSGui_Loader::LabelLinkDataMap& theLabelLinkData)
{
  MESSAGE("YACSGui_Module::displayGraph");
  YACSGui_Graph* aGraph = 0;

  QxGraph_Canvas* aCanvas = getCanvas();
  if ( !aCanvas )
    return aGraph;

  aGraph = new YACSGui_Graph( this, aCanvas, theGraph );
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

      std::list<YACSPrs_Link*> aLinksPrs = aFromNodePrs->getPortPrs( (*it).first.first )->getLinks();
      for ( std::list<YACSPrs_Link*>::iterator itL = aLinksPrs.begin(); itL != aLinksPrs.end(); itL++ )
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
YACSGui_Graph* YACSGui_Module::getGraph( YACS::ENGINE::Proc* theGraph )
{
  MESSAGE("YACSGui_Module::getGraph");
  YACSGui_Graph* aGraph = 0;
  // TODO - find a graph in all viewers
  // ...
  // TEMPORARY solution:
  if ( activeGraph() && activeGraph()->getProc() == theGraph )
  {
    aGraph = activeGraph();
  }

  return aGraph;
}

//! Returns the presentation of a the calculation graph corresponding to the active view.
/*!
 */
YACSGui_Graph* YACSGui_Module::activeGraph()
{
  MESSAGE("YACSGui_Module::activeGraph");
  YACSGui_Graph* aGraph = 0;
  QxGraph_Canvas* aCanvas = getCanvas();
  if ( aCanvas )
    // Assume that there's the one and only prs per a canvas
    aGraph = dynamic_cast<YACSGui_Graph*>( aCanvas->getPrs() );
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

//! Load dataflow from file.
/*!
 *  \param setEditable - define if the dataflow wiil be editable.
 */
void YACSGui_Module::ImportDataflowFromFile(const bool setEditable, bool fromSuperv)
{
  MESSAGE("YACSGui_Module::ImportDataflowFromFile");
  QString caption;
  if (fromSuperv)
    {
      if (setEditable)
        caption = QString(tr("TLT_MODIFY_SUPERV_DATAFLOW"));
      else
        caption = QString(tr("TLT_IMPORT_SUPERV_DATAFLOW"));
    }
  else
    {
      if (setEditable)
        caption = QString(tr("TLT_MODIFY_DATAFLOW"));
      else
        caption = QString(tr("TLT_IMPORT_DATAFLOW"));
    }
  QString aFileName = SUIT_FileDlg::getFileName( application()->desktop(), "", "*.xml", caption, true );

  if (aFileName.isEmpty())
    return;
  QString tmpFileName = aFileName;

  if (fromSuperv)
    {
      YACSGui_ORB::YACSGui_Gen_ptr engineRef = InitYACSGuiGen(getApp());
      try
        {
          tmpFileName = engineRef->convertSupervFile(aFileName);
        }
      catch(...)
        {
          std::cerr<<"Unexpected exception in convertSupervFile " <<std::endl;
          SUIT_MessageBox::warn1(getApp()->desktop(),
                                 tr("WARNING"),
                                 tr("Unexpected exception in convertSupervFile"),
                                 tr("BUT_OK"));
          return;
        }
    }
  
  //TD: Here check if the study already contains a graph imported from the same file
  
  YACSGui_Loader aLoader;
  
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
	  return;
	}

      aProc->setName(QFileInfo(aFileName).fileName ());
	 
      SUIT_ViewManager* aVM  = getApp()->createViewManager( QxGraph_Viewer::Type() );

      // set caption for graph's view window
      QxGraph_ViewWindow* aVW = dynamic_cast<QxGraph_ViewWindow*>( aVM->getActiveView() );
      if ( aVW ) aVW->setCaption(QFileInfo(aFileName).fileName ());

      YACSGui_DataModel* aModel = getDataModel();
      if (aModel)
	aModel->add(aProc, setEditable);
      // TD: add graph to study.

      int w, h;
      const YACSGui_Loader::PrsDataMap& aPrsData = aLoader.getPrsData( aProc, w, h );
      const YACSGui_Loader::PortLinkDataMap& aPortLinkData = aLoader.getPortLinkData( aProc );
      const YACSGui_Loader::LabelLinkDataMap& aLabelLinkData = aLoader.getLabelLinkData( aProc );
      if ( aVW )
      {
	if ( w > 0 && h > 0 )
	  aVW->getViewModel()->getCanvas()->resize( w, h );
	displayGraph( aProc, aPrsData, aPortLinkData, aLabelLinkData );
      }
    }
  catch (YACS::Exception& ex)
    {
      std::cerr<<"YACSGui_Module::importDataflow: " <<ex.what()<<std::endl;
    }
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
