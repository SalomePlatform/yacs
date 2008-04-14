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

#ifndef YACSGui_Module_HeaderFile
#define YACSGui_Module_HeaderFile

#include <SalomeApp_Application.h>
#include <SalomeApp_Module.h>
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(yacsgui)

#include <YACSGui_XMLDriver.h>
#include <YACSGui_DataModel.h>

#include <Proc.hxx>

#include <guiObservers.hxx>
#include <guiContext.hxx>

#include <map>

class SUIT_ViewWindow;
class SUIT_ViewManager;
class YACSGui_Graph;
class YACSGui_Executor;
class QxGraph_Viewer;
class QxGraph_ViewWindow;
class QxGraph_Canvas;
class YACSGui_RunMode;
class YACSGui_InputPanel;
class YACSGui_TreeView;
class LightApp_WidgetContainer;

class QListViewItem;
class SUIT_ToolButton;

class Catalog;

class YACSGui_Module : public SalomeApp_Module
{
  Q_OBJECT

public:

  typedef enum { WT_EditTreeView = LightApp_Application::WT_User, WT_RunTreeView } WindowTypes;

  enum { // Menu "File"
         ImportSchemaId, ExportSchemaId,
	 ImportSupervSchemaId, NewSchemaId,

	 // Menu "Create"
	 NewContainerId, 
	 NewSalomeComponentId, NewSalomePythonComponentId, NewCorbaComponentId, 
	 NewSalomeServiceNodeId, NewCorbaServiceNodeId, NewCppNodeId, 
	 NewServiceInlineNodeId, NewXmlNodeId, NewInlineScriptNodeId,
	 NewInlineFunctionNodeId, NewBlockNodeId, NewForLoopNodeId, NewForeachLoopNodeId, 
	 NewWhileLoopNodeId, NewSwitchLoopNodeId, NewFromLibraryNodeId,
         NewInDataNodeId, NewOutDataNodeId, NewInStudyNodeId, NewOutStudyNodeId,
	 	 	 
	 //Menu "Execute" (edit gui mode)
	 NewExecutionId, NewBatchExecutionId, ReloadExecutionId,

	 //Menu "Execute" (run gui mode)
	 StartResumeId,
	 /*ExecuteId, ExecuteStById,*/
	 PauseId, /*ResumeId,*/ AbortId, ResetId,
	 WithoutStopModeId, BreakpointsModeId, StepByStepModeId, ToggleStopOnErrorId,
	 ExecuteInBatchId, ConnectToRunningId,
	 SaveExecutionStateId, NewEditionId,
	 

	 // Menu "Supervisor"
         /*AddNodeId,
	 RunDataflowId, RemoteRunDataflowId, KillDataflowId, SuspendResumeDataflowId, 
	 ToggleStopOnErrorId, FilterNextStepsId,
	 AddDataflowInStudyId, ChangeInformationsId, SaveDataflowStateId, ReloadDataflowId, 
	 */
	 
	 // Popup menu
	 SetActiveId,
	 
	 // View mode section of "Dataflow" toolbar
	 FullViewId, ControlViewId, DataFlowViewId, DataStreamViewId,
	 ArrangeNodesId, RebuildLinksId
  };
  
  enum GuiMode {
    EmptyMode,  
    InitMode,
    EditMode,
    RunMode,
    NewMode
  };
  enum{ImportBtnId=1,NewContainerBtnId,NewNodeBtn};
  enum{SchemaTbId, CreationTbId};
  
public:
  YACSGui_Module();
  ~YACSGui_Module();
  
  virtual void initialize( CAM_Application* );

  QString engineIOR() const;
  static YACS_ORB::YACS_Gen_ptr InitYACSGuiGen( SalomeApp_Application* );

  YACSGui_DataModel* getDataModel() const;
  
  virtual void windows( QMap<int, int>& ) const;
  virtual void viewManagers( QStringList& ) const;
  
  YACSGui_Graph* displayGraph( YACS::HMI::GuiContext*,
			       const YACSGui_Loader::PrsDataMap&,
			       const YACSGui_Loader::PortLinkDataMap&,
			       const YACSGui_Loader::LabelLinkDataMap&,
			       const int& = 0, const int& = 0,
			       const double& = 1., const double& = 1. );
  YACSGui_Graph* getGraph( YACS::ENGINE::Proc* );
  YACSGui_Graph* activeGraph();

  QxGraph_ViewWindow* getViewWindow( YACS::ENGINE::Proc* );

  QxGraph_Viewer* getViewer();
  QxGraph_Canvas* getCanvas();
  void            updateViewer();

  QString iconName() const {return tr("YACS_MODULE_ICON");}

  YACSGui_RunMode* getRunMode(YACSGui_Executor* executor);

  YACSGui_InputPanel* getInputPanel() const { return myInputPanel; }

  virtual void            selectionChanged();
  void                    setGuiMode( GuiMode );
  void                    setGuiMode( YACSGui_DataModel::ObjectType );
  GuiMode                 getGuiMode() const;
  
  virtual LightApp_Selection* createSelection() const;
  void addTreeView( YACSGui_TreeView* theTreeView, const int theStudyId, const int theId );
  YACSGui_TreeView* treeView( const int theStudyId, const int theId ) const;
  void removeTreeView( const int theStudyId, const int theId );
  bool isTreeViewShown( const int theStudyId, const int theId );
  YACSGui_TreeView* activeTreeView() const;
  YACSGui_TreeView* activeTreeView( const int theStudyId ) const;
  void activateTreeView( const int theStudyId, const int theId );
  void setInputPanelVisibility( const bool theOn );

  //void connectArrangeNodes( YACSPrs_ElementaryNode* theItem );

  YACS::ENGINE::Catalog*  getCatalog() const { return myCatalog; }

  YACSGui_Executor* lookForExecutor();
  YACSGui_Executor* getExecutor( YACS::ENGINE::Proc* );
  YACSGui_Executor* findExecutor();

  std::string getNodeType( YACS::HMI::TypeOfElem theElemType );

  YACS::HMI::SubjectNode* createBody( YACS::HMI::TypeOfElem theElemType,
				      YACS::HMI::Subject* theSubject,
				      YACS::ENGINE::Catalog* theCatalog,
				      std::string theCompo = "",
				      std::string theService = "" );
  
public slots:
  virtual bool activateModule( SUIT_Study* );
  virtual bool deactivateModule( SUIT_Study* );
  void onArrangeNodes();

  void onFullView();
  void onControlView();
  void onDataFlowView();
  void onDataStreamView();

  void temporaryExport();

 protected:
  void createSComponent(); 
  void loadCatalog();
  void publishInStudy(YACSGui_Graph* theGraph); // a test function
  virtual CAM_DataModel* createDataModel();
  void createGraph( SUIT_ViewManager* );
  YACSGui_TreeView* createTreeView( YACS::HMI::SubjectProc*, const WindowTypes );
  int viewManagerId( YACS::ENGINE::Proc* theProc );
 
private slots:
  void onImportSchema();
  void onImportSupervSchema();
  void onExportSchema();
  
  void onEditDataTypes();
  void onCreateDataType();
  void onImportDataType();

  void onNewContainer();
  void onNewSalomeComponent();
  void onNewSalomePythonComponent();
  void onNewCorbaComponent();
  void onNewSchema();

  void onSalomeServiceNode();
  void onServiceInlineNode();
  void onCorbaServiceNode();
  void onNodeNodeServiceNode();
  void onCppNode();
  void onXMLNode();
  void onInDataNode();
  void onOutDataNode();
  void onInStudyNode();
  void onOutStudyNode();
  void onInlineScriptNode();
  void onInlineFunctionNode();
  void onBlockNode();
  void onFORNode();
  void onFOREACHNode();
  void onWHILENode();
  void onSWITCHNode();
  void onNodeFromLibrary();

  void onReloadDataflow();
  void onRebuildLinks();

  void onRunDataflow();
  void onKillDataflow();
  void onSuspendResumeDataflow();
  void onRemoteRunDataflow();
  void onFilterNextSteps();
  void onSaveDataflowState();
  void onCreateExecution();
  void onCreateBatchExecution();
  void onReloadExecution();
  void onStartResume();
  void onExecute();
  void onExecuteStBy();
  void onExecuteInBatch();
  void onPause();
  void onResume();
  //void onSuspendResume();
  void onAbort();
  void onReset();
  void onWithoutStopMode();
  void onBreakpointsMode();
  void onStepByStepMode();
  void onSaveExecutionState();
  void onCreateEdition();
  void onConnectToRunning();
  void onToggleStopOnError();
  
  void onChangeInformations();

  void onTableView();

  void onDblClick( QListViewItem* );
  void onExpanded( QListViewItem* theItem );
  void onCollapsed( QListViewItem* theItem );

  void onApplyInputPanel( const int theId );

  void onSetActive();
  
  void onWindowActivated( SUIT_ViewWindow* );
  void onWindowClosed( SUIT_ViewWindow* );

private:
  void createActions();
  void createMenus();
  void createPreferences();
  void createPopups();
  void createInputPanel();
  void createTreeView( YACS::HMI::SubjectProc*, YACSGui_DataModel::ObjectType );

  void setModifyActionShown(bool on);
  void setBaseActionShown(bool on);
  void setVisualizationActionShown(bool on);
  void setRunActionShown(bool on);

  void CreateNewSchema( QString&, YACS::ENGINE::Proc*, const  bool = true);

  YACS::HMI::GuiContext* ImportProcFromFile( const QString& filePath,
					     const GuiMode,
					     const bool setEditable,
					     bool fromSuperv = false);
  void createElementaryNodePrs(); // for test presentations

  QxGraph_ViewWindow* createNewViewManager( GuiMode, YACS::ENGINE::Proc*, int = 0, int = 0 );
  void activateViewWindow( YACS::ENGINE::Proc* );

  void fillNodeTypesMap();
  std::string getNodeType( YACS::ENGINE::Node* theNode );

  void createNode( YACS::HMI::TypeOfElem theElemType,
		   std::string theCompo = "",
		   std::string theService = "" );
  
private:
  typedef std::map<YACS::ENGINE::Proc*, YACSGui_Executor*> ExecutorMap;
  typedef std::map<YACS::ENGINE::Proc*, QxGraph_ViewWindow*> WindowsMap;
  typedef std::map<YACSGui_Executor*, YACSGui_RunMode*> RunModeMap;
  typedef std::map<int, LightApp_WidgetContainer*> TreeViewMap;
  typedef std::map<int, int> TreeViewActiveMap;
  typedef std::map<int, SUIT_ToolButton*> ToolButtonMap;
  typedef std::map<int, QToolBar*> ToolBarMap;
  typedef std::map<YACS::HMI::TypeOfElem, std::string> NodeTypeMap;
  
  YACS::ENGINE::Catalog*   myCatalog;

  ExecutorMap              myExecutors;
  RunModeMap               _runModeMap;
  YACSGui_RunMode*         _myRunMode;
  
  YACSGui_InputPanel*      myInputPanel;
  GuiMode                  myGuiMode;

  TreeViewMap              myTreeViews;
  TreeViewActiveMap        myActiveTreeViews;
  ToolButtonMap            myToolButtons;
  ToolBarMap               myToolBars;

  WindowsMap               myWindowsMap;
  bool                     myBlockWindowActivateSignal;

  NodeTypeMap              myNodeTypeMap;
};

#endif
