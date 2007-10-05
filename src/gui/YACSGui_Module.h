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

#include <SalomeApp_Module.h>
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(yacsgui)

#include <YACSGui_XMLDriver.h>

#include <Proc.hxx>

#include <map>

class SUIT_ViewWindow;
class SUIT_ViewManager;
class YACSGui_DataModel;
class YACSGui_Graph;
class YACSGui_Executor;
class QxGraph_Viewer;
class QxGraph_Canvas;
class YACSGui_RunMode;

class SalomeApp_Application;
class YACSGui_Module : public SalomeApp_Module
{
  Q_OBJECT

public:
  enum { // Menu "File"
         NewDataflowId, ModifyDataflowId, ImportDataflowId, ExportDataflowId,
         ModifySupervDataflowId, ImportSupervDataflowId,

	 // Menu "Supervisor"
         AddNodeId,
         RunDataflowId, RemoteRunDataflowId, KillDataflowId, SuspendResumeDataflowId, 
         ToggleStopOnErrorId, FilterNextStepsId,
	 AddDataflowInStudyId, ChangeInformationsId, SaveDataflowStateId, ReloadDataflowId, 
	 RebuildLinksId,
	 
	 // View mode section of "Dataflow" toolbar
	 FullViewId, ControlViewId, TableViewId
  };

public:
  YACSGui_Module();
  ~YACSGui_Module();
  
  virtual void initialize( CAM_Application* );

  QString engineIOR() const;
  static YACSGui_ORB::YACSGui_Gen_ptr InitYACSGuiGen( SalomeApp_Application* );

  YACSGui_DataModel* getDataModel() const;

  virtual void windows( QMap<int, int>& ) const;
  virtual void viewManagers( QStringList& ) const;
  
  YACSGui_Graph* displayGraph( YACS::ENGINE::Proc*, 
			       const YACSGui_Loader::PrsDataMap&,
			       const YACSGui_Loader::PortLinkDataMap&,
			       const YACSGui_Loader::LabelLinkDataMap&);
  YACSGui_Graph* getGraph( YACS::ENGINE::Proc* );
  YACSGui_Graph* activeGraph();

  QxGraph_Viewer* getViewer();
  QxGraph_Canvas* getCanvas();
  void            updateViewer();

  QString iconName() const {return tr("YACS_MODULE_ICON");}

  YACSGui_RunMode* getRunMode(YACSGui_Executor* executor);

public slots:
  virtual bool activateModule( SUIT_Study* );
  virtual bool deactivateModule( SUIT_Study* );

protected:
  virtual CAM_DataModel* createDataModel();
  void createGraph( SUIT_ViewManager* );
 
private slots:
  void onNewDataflow();
  void onImportDataflow();
  void onModifyDataflow();
  void onImportSupervDataflow();
  void onModifySupervDataflow();
  void onExportDataflow();
   
  void onAddNode();
  
  void onReloadDataflow();
  void onRebuildLinks();

  void onRunDataflow();
  void onKillDataflow();
  void onSuspendResumeDataflow();
  void onRemoteRunDataflow();
  void onFilterNextSteps();
  void onSaveDataflowState();
  void onToggleStopOnError();

  void onAddDataflowInStudy();
  void onChangeInformations();

  void onFullView();
  void onControlView();
  void onTableView();

private:
  void createActions();
  void createMenus();

  void ImportDataflowFromFile(const bool setEditable, bool fromSuperv = false);

  void createElementaryNodePrs(); // for test presentations

  YACSGui_Executor* getExecutor( YACS::ENGINE::Proc* );
  YACSGui_Executor* findExecutor();

private:
  typedef std::map<YACS::ENGINE::Proc*, YACSGui_Executor*> ExecutorMap;
  typedef std::map<YACSGui_Executor*, YACSGui_RunMode*> RunModeMap;

  ExecutorMap myExecutors;
  RunModeMap _runModeMap;
  YACSGui_RunMode* _myRunMode;
};

#endif
