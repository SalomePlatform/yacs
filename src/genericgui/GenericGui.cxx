
#include <Python.h>
#include "SALOME_ResourcesManager.hxx"
#include "SALOME_LifeCycleCORBA.hxx"

#include "RuntimeSALOME.hxx"
#include "Proc.hxx"
#include "parsers.hxx"
#include "Logger.hxx"

#include "SALOME_NamingService.hxx"
#include "SALOME_ModuleCatalog.hxx"
#include "SALOME_ModuleCatalog.hh"
#include "SALOMEDS_Tool.hxx"

#include "QtGuiContext.hxx"

#include "GuiEditor.hxx"
#include "GraphicsView.hxx"
#include "Scene.hxx"
#include "GenericGui.hxx"
#include "SceneItem.hxx"
#include "ItemEdition.hxx"
#include "CatalogWidget.hxx"
#include "TreeView.hxx"
#include "VisitorSaveGuiSchema.hxx"
#include "TypeCode.hxx"
#include "LinkInfo.hxx"

#include <QFileDialog>
#include <sstream>
#include <QDir>
#include <QDateTime>

#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

GenericGui::GenericGui(YACS::HMI::SuitWrapper* wrapper, QMainWindow *parent)
{
  _wrapper = wrapper;
  _parent = parent;
  _dwTree = 0;
  _dwStacked = 0;
  _dwCatalogs = 0;
  _catalogsWidget = 0;
  _sessionCatalog = 0;
  _schemaCnt = 0;
  _mapViewContext.clear();
  _machineList.clear();
  _menuId = 190;

  QDir::addSearchPath("icons", "share/salome/resources/yacs");

  YACS::ENGINE::RuntimeSALOME::setRuntime();
  _loader = new YACS::YACSLoader();
  _loader->registerProcCataLoader();
  _builtinCatalog = YACS::ENGINE::getSALOMERuntime()->getBuiltinCatalog();

  try
    {
      YACS::ENGINE::RuntimeSALOME* runTime = YACS::ENGINE::getSALOMERuntime();
      CORBA::ORB_ptr orb = runTime->getOrb();
      if (orb)
        {
          SALOME_NamingService namingService(orb);
          CORBA::Object_ptr obj = namingService.Resolve("/Kernel/ModulCatalog");
          SALOME_ModuleCatalog::ModuleCatalog_var aModuleCatalog =
            SALOME_ModuleCatalog::ModuleCatalog::_narrow(obj);
          if (! CORBA::is_nil(aModuleCatalog))
            {
              DEBTRACE("SALOME_ModuleCatalog::ModuleCatalog found");
              std::string anIOR = orb->object_to_string( aModuleCatalog );
              _sessionCatalog = runTime->loadCatalog( "session", anIOR );
              {
                std::map< std::string, YACS::ENGINE::ComponentDefinition * >::iterator it;
                for (it = _sessionCatalog->_componentMap.begin();
                     it != _sessionCatalog->_componentMap.end(); ++it)
                  DEBTRACE("Component: " <<(*it).first);
              }
              {
                std::map< std::string, YACS::ENGINE::TypeCode*>::iterator it;
                for (it = _sessionCatalog->_typeMap.begin();
                     it != _sessionCatalog->_typeMap.end(); ++it)
                  DEBTRACE("Type : " <<(*it).first
                           << " " << (*it).second->getKindRepr()
                           << " " << (*it).second->name()
                           << " " << (*it).second->shortName()
                           << " " << (*it).second->id() );
              }
              DEBTRACE("_sessionCatalog " << _sessionCatalog);
            }
        }
    }
  catch(ServiceUnreachable& e)
    {
      DEBTRACE("Caught Exception. "<<e);
    }

  _dwTree = new QDockWidget(_parent);
  _dwTree->setWindowTitle("Tree View: edition mode");
  _parent->addDockWidget(Qt::LeftDockWidgetArea, _dwTree);
  _dwStacked = new QDockWidget(_parent);
  _dwStacked->setWindowTitle("Input Panel");
  _parent->addDockWidget(Qt::RightDockWidgetArea, _dwStacked);
  _dwCatalogs = new QDockWidget(_parent);
  _dwCatalogs->setWindowTitle("Catalogs");
  _parent->addDockWidget(Qt::RightDockWidgetArea, _dwCatalogs);
  _catalogsWidget = new CatalogWidget(_dwCatalogs,
                                      _builtinCatalog,
                                      _sessionCatalog);
  _catalogsWidget->setMinimumWidth(350); // --- force a minimum until display
  _dwCatalogs->setWidget(_catalogsWidget);
  _parent->tabifyDockWidget(_dwStacked, _dwCatalogs);
}

GenericGui::~GenericGui()
{
}

void GenericGui::createActions()
{
//       QAction* createAction(const int id,
//                             const QString& toolTip,
//                             const QIcon& icon,
//                             const QString& menu,
//                             const QString& status,
//                             const int shortCut,
//                             QObject* parent =0,
//                             bool checkable = false,
//                             QObject* receiver =0,
//                             const char* member =0);

  QPixmap pixmap;

  pixmap.load("icons:new_dataflow.png");
  _newSchemaAct = _wrapper->createAction(getMenuId(), tr("Create a new YACS Schema"), QIcon(pixmap),
                                         tr("New Schema"), tr("Create a new YACS Schema"),
                                         0, _parent, false, this,  SLOT(onNewSchema()));
  
  pixmap.load("icons:import_dataflow.png");
  _importSchemaAct = _wrapper->createAction(getMenuId(), tr("Import a YACS Schema for edition"), QIcon(pixmap),
                                            tr("Import Schema"), tr("Import a YACS Schema for edition"),
                                            0, _parent, false, this,  SLOT(onImportSchema()));
  
  pixmap.load("icons:import_superv_dataflow.png");
  _importSupervSchemaAct = _wrapper->createAction(getMenuId(), tr("Import a SUPERV Schema for edition"), QIcon(pixmap),
                                                  tr("Import SUPERV Schema"), tr("Import a SUPERV Schema for edition"),
                                                  0, _parent, false, this,  SLOT(onImportSupervSchema()));
  
  pixmap.load("icons:save_dataflow.png");
  _exportSchemaAct = _wrapper->createAction(getMenuId(), tr("Save the current YACS Schema"), QIcon(pixmap),
                                            tr("Save Schema"), tr("Save the current YACS Schema"),
                                            0, _parent, false, this,  SLOT(onExportSchema()));
  
  pixmap.load("icons:export_dataflow.png");
  _exportSchemaAsAct = _wrapper->createAction(getMenuId(), tr("Save the current YACS Schema As..."), QIcon(pixmap),
                                              tr("Save Schema As"), tr("Save the current YACS Schema As..."),
                                              0, _parent, false, this,  SLOT(onExportSchemaAs()));

  pixmap.load("icons:insert_file.png");
  _importCatalogAct = _wrapper->createAction(getMenuId(), tr("Import a Schema as a Catalog"), QIcon(pixmap),
                                              tr("Import Catalog"), tr("Import a Schema as a Catalog"),
                                              0, _parent, false, this,  SLOT(onImportCatalog()));


  pixmap.load("icons:run_active.png");
  _runLoadedSchemaAct = _wrapper->createAction(getMenuId(), tr("Prepare the current edited schema for run"), QIcon(pixmap),
                                              tr("Run Current Schema"), tr("Prepare the current edited schema for run"),
                                              0, _parent, false, this,  SLOT(onRunLoadedSchema()));

  pixmap.load("icons:run.png");
  _loadAndRunSchemaAct = _wrapper->createAction(getMenuId(), tr("Load a schema for run"), QIcon(pixmap),
                                              tr("Load Schema to run"), tr("Load a schema for run"),
                                              0, _parent, false, this,  SLOT(onLoadAndRunSchema()));


  pixmap.load("icons:remote_run.png");
  _executeAct = _wrapper->createAction(getMenuId(), tr("Execute the schema"), QIcon(pixmap),
                                       tr("Execute schema"), tr("Execute the schema"),
                                       0, _parent, false, this,  SLOT(onExecute()));

  pixmap.load("icons:kill.png");
  _abortAct = _wrapper->createAction(getMenuId(), tr("Abort the current execution"), QIcon(pixmap),
                                     tr("Abort execution"), tr("Abort the current execution"),
                                     0, _parent, false, this,  SLOT(onAbort()));
  pixmap.load("icons:suspend_resume.png");
  _suspendResumeAct = _wrapper->createAction(getMenuId(), tr("Suspend or Resume the current execution"), QIcon(pixmap),
                                             tr("Suspend/Resume execution"), tr("Suspend or Resume the current execution"),
                                             0, _parent, false, this,  SLOT(onSuspendResume()));

  pixmap.load("icons:pause.png");
  _pauseAct = _wrapper->createAction(getMenuId(), tr("Suspend the current execution"), QIcon(pixmap),
                                     tr("Suspend execution"), tr("Suspend the current execution"),
                                     0, _parent, false, this,  SLOT(onPause()));

  pixmap.load("icons:resume.png");
  _resumeAct = _wrapper->createAction(getMenuId(), tr("Resume the current execution"), QIcon(pixmap),
                                      tr("Resume execution"), tr("Resume the current execution"),
                                      0, _parent, false, this,  SLOT(onResume()));



  pixmap.load("icons:kill.png");
  _editDataTypesAct = _wrapper->createAction(getMenuId(), tr("Edit Data Types"), QIcon(pixmap),
                                              tr("Edit Data Types"), tr("Edit Data Types"),
                                              0, _parent, false, this,  SLOT(onEditDataTypes()));

  pixmap.load("icons:kill.png");
  _createDataTypeAct = _wrapper->createAction(getMenuId(), tr("Create Data Types"), QIcon(pixmap),
                                              tr("Create Data Types"), tr("Create Data Types"),
                                              0, _parent, false, this,  SLOT(onCreateDataType()));

  pixmap.load("icons:kill.png");
  _importDataTypeAct = _wrapper->createAction(getMenuId(), tr("Import Data Types"), QIcon(pixmap),
                                              tr("Import Data Types"), tr("Import Data Types"),
                                              0, _parent, false, this,  SLOT(onImportDataType()));

  pixmap.load("icons:container.png");
  _newContainerAct = _wrapper->createAction(getMenuId(), tr("Create a New Container"), QIcon(pixmap),
                                              tr("Create Container"), tr("Create a New Container"),
                                              0, _parent, false, this,  SLOT(onNewContainer()));

  pixmap.load("icons:new_salome_component.png");
  _newSalomeComponentAct = _wrapper->createAction(getMenuId(), tr("Create a New SALOME Component"), QIcon(pixmap),
                                              tr("SALOME Component"), tr("Create a New SALOME Component"),
                                              0, _parent, false, this,  SLOT(onNewSalomeComponent()));

  pixmap.load("icons:new_salomepy_component.png");
  _newSalomePythonComponentAct = _wrapper->createAction(getMenuId(), tr("Create a New SALOME Python Component"), QIcon(pixmap),
                                              tr("SALOME Python Component"), tr("Create a New SALOME Python Component"),
                                              0, _parent, false, this,  SLOT(onNewSalomePythonComponent()));

  pixmap.load("icons:new_corba_component.png");
  _newCorbaComponentAct = _wrapper->createAction(getMenuId(), tr("Create a New CORBA Component"), QIcon(pixmap),
                                              tr("CORBA Component"), tr("Create a New CORBA Component"),
                                              0, _parent, false, this,  SLOT(onNewCorbaComponent()));

  pixmap.load("icons:schema.png");
  _newSchemaAct = _wrapper->createAction(getMenuId(), tr("New Schema"), QIcon(pixmap),
                                              tr("New Schema"), tr("New Schema"),
                                              0, _parent, false, this,  SLOT(onNewSchema()));

  pixmap.load("icons:new_salome_service_node.png");
  _salomeServiceNodeAct = _wrapper->createAction(getMenuId(), tr("Create a New SALOME Service Node"), QIcon(pixmap),
                                              tr("SALOME Service Node"), tr("Create a New SALOME Service Node"),
                                              0, _parent, false, this,  SLOT(onSalomeServiceNode()));

  pixmap.load("icons:new_service_inline_node.png");
  _serviceInlineNodeAct = _wrapper->createAction(getMenuId(), tr("Create a New Inline Service Node"), QIcon(pixmap),
                                              tr("Inline Service Node"), tr("Create a New Inline Service Node"),
                                              0, _parent, false, this,  SLOT(onServiceInlineNode()));

  pixmap.load("icons:new_corba_service_node.png");
  _CORBAServiceNodeAct = _wrapper->createAction(getMenuId(), tr("Create a New CORBA Service Node"), QIcon(pixmap),
                                              tr("CORBA Node"), tr("Create a New CORBA Service Node"),
                                              0, _parent, false, this,  SLOT(onCORBAServiceNode()));

  pixmap.load("icons:new_nodenode_service_node.png");
  _nodeNodeServiceNodeAct = _wrapper->createAction(getMenuId(), tr("Create a New Node referencing a Node"), QIcon(pixmap),
                                              tr("Ref on Node"), tr("Create a New Node referencing a Node"),
                                              0, _parent, false, this,  SLOT(onNodeNodeServiceNode()));

  pixmap.load("icons:new_cpp_node.png");
  _cppNodeAct = _wrapper->createAction(getMenuId(), tr("Create a New C++ Node"), QIcon(pixmap),
                                              tr("Cpp Node"), tr("Create a New C++ Node"),
                                              0, _parent, false, this,  SLOT(onCppNode()));

  pixmap.load("icons:kill.png");
  _inDataNodeAct = _wrapper->createAction(getMenuId(), tr("Create a New Input data Node"), QIcon(pixmap),
                                              tr("Input Data Node"), tr("Create a New Input data Node"),
                                              0, _parent, false, this,  SLOT(onInDataNode()));

  pixmap.load("icons:kill.png");
  _outDataNodeAct = _wrapper->createAction(getMenuId(), tr("Create a New Output data Node"), QIcon(pixmap),
                                              tr("Output Data Node"), tr("Create a New Output data Node"),
                                              0, _parent, false, this,  SLOT(onOutDataNode()));

  pixmap.load("icons:kill.png");
  _inStudyNodeAct = _wrapper->createAction(getMenuId(), tr("Create a New Input Study Node"), QIcon(pixmap),
                                              tr("Input Study Node"), tr("Create a New Input Study Node"),
                                              0, _parent, false, this,  SLOT(onInStudyNode()));

  pixmap.load("icons:kill.png");
  _outStudyNodeAct = _wrapper->createAction(getMenuId(), tr("Create a New Output Study Node"), QIcon(pixmap),
                                              tr("Output Study Node"), tr("Create a New Output Study Node"),
                                              0, _parent, false, this,  SLOT(onOutStudyNode()));

  pixmap.load("icons:new_inline_script_node.png");
  _inlineScriptNodeAct = _wrapper->createAction(getMenuId(), tr("Create a New Inline Python Script Node"), QIcon(pixmap),
                                              tr("Inline Script Node"), tr("Create a New Inline Python Script Node"),
                                              0, _parent, false, this,  SLOT(onInlineScriptNode()));

  pixmap.load("icons:new_inline_function_node.png");
  _inlineFunctionNodeAct = _wrapper->createAction(getMenuId(), tr("Create a New Inline Python Function Node"), QIcon(pixmap),
                                              tr("Inline Function Node"), tr("Create a New Inline Python Function Node"),
                                              0, _parent, false, this,  SLOT(onInlineFunctionNode()));

  pixmap.load("icons:new_block_node.png");
  _blockNodeAct = _wrapper->createAction(getMenuId(), tr("Create a New Bloc Node"), QIcon(pixmap),
                                              tr("bloc Node"), tr("Create a New Bloc Node"),
                                              0, _parent, false, this,  SLOT(onBlockNode()));

  pixmap.load("icons:new_for_loop_node.png");
  _FORNodeAct = _wrapper->createAction(getMenuId(), tr("Create a New For Loop Node"), QIcon(pixmap),
                                              tr("For Loop Node"), tr("Create a New For Loop Node"),
                                              0, _parent, false, this,  SLOT(onFORNode()));

  pixmap.load("icons:new_foreach_loop_node.png");
  _FOREACHNodeAct = _wrapper->createAction(getMenuId(), tr("Create a New For Each Loop Node"), QIcon(pixmap),
                                              tr("For Each Loop Node"), tr("Create a New For Each Loop Node"),
                                              0, _parent, false, this,  SLOT(onFOREACHNode()));

  pixmap.load("icons:new_while_loop_node.png");
  _WHILENodeAct = _wrapper->createAction(getMenuId(), tr("Create a New While Loop Node"), QIcon(pixmap),
                                              tr("While Loop Node"), tr("Create a New While Loop Node"),
                                              0, _parent, false, this,  SLOT(onWHILENode()));

  pixmap.load("icons:new_switch_loop_node.png");
  _SWITCHNodeAct = _wrapper->createAction(getMenuId(), tr("Create a New Switch Node"), QIcon(pixmap),
                                              tr("Switch Node"), tr("Create a New Switch Node"),
                                              0, _parent, false, this,  SLOT(onSWITCHNode()));

  pixmap.load("icons:new_from_library_node.png");
  _nodeFromCatalogAct = _wrapper->createAction(getMenuId(), tr("Create a New Node from Catalog"), QIcon(pixmap),
                                              tr("Node from Catalog"), tr("Create a New Node from Catalog"),
                                              0, _parent, false, this,  SLOT(onNodeFromCatalog()));

  pixmap.load("icons:kill.png");
  _deleteItemAct = _wrapper->createAction(getMenuId(), tr("Delete a Schema Item"), QIcon(pixmap),
                                              tr("Delete Item"), tr("Delete a Schema Item"),
                                              0, _parent, false, this,  SLOT(onDeleteItem()));

  pixmap.load("icons:rebuild_links.png");
  _computeLinkAct = _wrapper->createAction(getMenuId(), tr("compute orthogonal links"), QIcon(pixmap),
                                              tr("compute links"), tr("compute orthogonal links"),
                                              0, _parent, false, this,  SLOT(onRebuildLinks()));

  pixmap.load("icons:ob_inline_node.png");
  _toggleAutomaticComputeLinkAct = _wrapper->createAction(getMenuId(), tr("compute othogonal links autoamtically when nodes move"), QIcon(pixmap),
                                              tr("automatic link"), tr("compute othogonal links autoamtically when nodes move"),
                                              0, _parent, true, this,  SLOT(onToggleAutomaticComputeLinks(bool)));

  pixmap.load("icons:ob_inline_node.png");
  _toggleSimplifyLinkAct = _wrapper->createAction(getMenuId(), tr("simplify links by removing unnecessary direction changes"), QIcon(pixmap),
                                              tr("simplify links"), tr("simplify links by removing unnecessary direction changes"),
                                              0, _parent, true, this,  SLOT(onToggleSimplifyLinks(bool)));

  pixmap.load("icons:ob_inline_node.png");
  _toggleForce2NodesLinkAct = _wrapper->createAction(getMenuId(), tr("force orthogonal links by adding an edge on simples links"), QIcon(pixmap),
                                              tr("force ortho links"), tr("force orthogonal links by adding an edge on simples links"),
                                              0, _parent, true, this,  SLOT(onToggleForce2NodesLinks(bool)));

  pixmap.load("icons:ob_service_node.png");
  _selectReferenceAct = _wrapper->createAction(getMenuId(), tr("select reference"), QIcon(pixmap),
                                              tr("select reference"), tr("select reference"),
                                              0, _parent, false, this,  SLOT(onSelectReference()));

  pixmap.load("icons:run_active.png");
  _withoutStopModeAct = _wrapper->createAction(getMenuId(), tr("set execution mode without stop"), QIcon(pixmap),
                                              tr("mode without stop"), tr("set execution mode without stop"),
                                               0, _parent, true, this,  SLOT(onWithoutStopMode(bool)));

  pixmap.load("icons:breakpoints_active.png");
  _breakpointsModeAct = _wrapper->createAction(getMenuId(), tr("set execution mode with stop on breakpoints"), QIcon(pixmap),
                                              tr("mode breakpoints"), tr("set execution mode with stop on breakpoints"),
                                               0, _parent, true, this,  SLOT(onBreakpointsMode(bool)));

  pixmap.load("icons:step_by_step_active.png");
  _stepByStepModeAct = _wrapper->createAction(getMenuId(), tr("set execution mode step by step"), QIcon(pixmap),
                                              tr("mode step by step"), tr("set execution mode step by step"),
                                              0, _parent, true, this,  SLOT(onStepByStepMode(bool)));


  _execModeGroup = new QActionGroup(this);
  _execModeGroup->addAction(_withoutStopModeAct);
  _execModeGroup->addAction(_breakpointsModeAct);
  _execModeGroup->addAction(_stepByStepModeAct);
  _withoutStopModeAct->setChecked(true);

  /*
  pixmap.load("icons:.png");
  _ = _wrapper->createAction(getMenuId(), tr(""), QIcon(pixmap),
                                              tr(""), tr(""),
                                              0, _parent, false, this,  SLOT());

  */


}

void GenericGui::createMenus()
{
  int aMenuId;
  aMenuId = _wrapper->createMenu( "File", -1, -1 );
  _wrapper->createMenu( _wrapper->separator(), aMenuId, -1, 10 );
  aMenuId = _wrapper->createMenu( "YACS" , aMenuId, -1, 10 );
  _wrapper->createMenu( _newSchemaAct, aMenuId );
  _wrapper->createMenu( _importSchemaAct, aMenuId );

  aMenuId = _wrapper->createMenu( tr( "YACS" ), -1, -1, 30 );
  _wrapper->createMenu( _newSchemaAct, aMenuId );//, 10
  _wrapper->createMenu( _importSchemaAct, aMenuId );
  _wrapper->createMenu( _importSupervSchemaAct, aMenuId );
  _wrapper->createMenu( _wrapper->separator(), aMenuId);
  _wrapper->createMenu( _exportSchemaAct, aMenuId );
  _wrapper->createMenu( _exportSchemaAsAct, aMenuId );
  _wrapper->createMenu( _wrapper->separator(), aMenuId);
  _wrapper->createMenu( _runLoadedSchemaAct, aMenuId );
  _wrapper->createMenu( _loadAndRunSchemaAct, aMenuId );
  _wrapper->createMenu( _wrapper->separator(), aMenuId);
  _wrapper->createMenu( _executeAct, aMenuId );
  _wrapper->createMenu( _abortAct, aMenuId );
  _wrapper->createMenu( _suspendResumeAct, aMenuId );
  _wrapper->createMenu( _pauseAct, aMenuId );
  _wrapper->createMenu( _resumeAct, aMenuId );
  _wrapper->createMenu( _wrapper->separator(), aMenuId);
  _wrapper->createMenu( _withoutStopModeAct, aMenuId );
  _wrapper->createMenu( _breakpointsModeAct, aMenuId );
  _wrapper->createMenu( _stepByStepModeAct, aMenuId );
  _wrapper->createMenu( _wrapper->separator(), aMenuId);
  _wrapper->createMenu( _importCatalogAct, aMenuId );
}

void GenericGui::createTools()
{
  int aToolId = _wrapper->createTool ( tr( "TOOL_YACS" ) );
  _wrapper->createTool( _newSchemaAct, aToolId );
  _wrapper->createTool( _importSchemaAct, aToolId );
  _wrapper->createTool( _importSupervSchemaAct, aToolId );
  _wrapper->createTool( _wrapper->separator(), aToolId );
  _wrapper->createTool( _exportSchemaAct, aToolId );
  _wrapper->createTool( _exportSchemaAsAct, aToolId );
  _wrapper->createTool( _wrapper->separator(), aToolId);
  _wrapper->createTool( _runLoadedSchemaAct, aToolId );
  _wrapper->createTool( _loadAndRunSchemaAct, aToolId );
  _wrapper->createTool( _wrapper->separator(), aToolId );
  _wrapper->createTool( _executeAct, aToolId );
  _wrapper->createTool( _abortAct, aToolId );
  _wrapper->createTool( _suspendResumeAct, aToolId );
  _wrapper->createTool( _pauseAct, aToolId );
  _wrapper->createTool( _resumeAct, aToolId );
  _wrapper->createTool( _wrapper->separator(), aToolId );
  _wrapper->createTool( _withoutStopModeAct, aToolId );
  _wrapper->createTool( _breakpointsModeAct, aToolId );
  _wrapper->createTool( _stepByStepModeAct, aToolId );
  _wrapper->createTool( _wrapper->separator(), aToolId );
  _wrapper->createTool( _importCatalogAct, aToolId );
}

void GenericGui::initialMenus()
{
  showBaseMenus(true);
  showEditionMenus(false);
  showExecMenus(false);
}

void GenericGui::showBaseMenus(bool show)
{
  DEBTRACE("GenericGui::showBaseMenus " << show);
  _wrapper->setMenuShown(_newSchemaAct, show);
  _wrapper->setToolShown(_newSchemaAct, show);
  _wrapper->setMenuShown(_importSchemaAct, show);
  _wrapper->setToolShown(_importSchemaAct, show);
  _wrapper->setMenuShown(_importSupervSchemaAct, show);
  _wrapper->setToolShown(_importSupervSchemaAct, show);
  _wrapper->setMenuShown(_loadAndRunSchemaAct, show);
  _wrapper->setToolShown(_loadAndRunSchemaAct, show);
}

void GenericGui::showEditionMenus(bool show)
{
  DEBTRACE("GenericGui::showEditionMenus " << show);
  _wrapper->setMenuShown(_exportSchemaAct, show);
  _wrapper->setToolShown(_exportSchemaAct, show);
  _wrapper->setMenuShown(_exportSchemaAsAct, show);
  _wrapper->setToolShown(_exportSchemaAsAct, show);
  _wrapper->setMenuShown(_runLoadedSchemaAct, show);
  _wrapper->setToolShown(_runLoadedSchemaAct, show);
  _wrapper->setMenuShown(_importCatalogAct, show);
  _wrapper->setToolShown(_importCatalogAct, show);
}

void GenericGui::showExecMenus(bool show)
{
  DEBTRACE("GenericGui::showExecMenus " << show);
  _wrapper->setMenuShown(_executeAct, show);
  _wrapper->setToolShown(_executeAct, show);
  _wrapper->setMenuShown(_abortAct, show);
  _wrapper->setToolShown(_abortAct, show);
  _wrapper->setMenuShown(_suspendResumeAct, show);
  _wrapper->setToolShown(_suspendResumeAct, show);
  _wrapper->setMenuShown(_pauseAct, show);
  _wrapper->setToolShown(_pauseAct, show);
  _wrapper->setMenuShown(_resumeAct, show);
  _wrapper->setToolShown(_resumeAct, show);
  _wrapper->setMenuShown(_withoutStopModeAct, show);
  _wrapper->setToolShown(_withoutStopModeAct, show);
  _wrapper->setMenuShown(_breakpointsModeAct, show);
  _wrapper->setToolShown(_breakpointsModeAct, show);
  _wrapper->setMenuShown(_stepByStepModeAct, show);
  _wrapper->setToolShown(_stepByStepModeAct, show);
}

void GenericGui::switchContext(QWidget *view)
{
  DEBTRACE("GenericGui::switchContext");
  if (! _mapViewContext.count(view))
    {
      initialMenus();
      return;
    }
  QtGuiContext* newContext = _mapViewContext[view];
  QtGuiContext* oldContext = QtGuiContext::getQtCurrent();

  _dwTree->setWidget(newContext->getEditTree());
  _dwStacked->setWidget(newContext->getStackedWidget());

  QtGuiContext::setQtCurrent(newContext);

  if (newContext->isEdition())
    {
      showBaseMenus(true);
      showEditionMenus(true);
      showExecMenus(false);
    }
  else
    {
      showBaseMenus(true);
      showEditionMenus(false);
      showExecMenus(true);
    }
}

void GenericGui::showDockWidgets(bool isVisible)
{
  DEBTRACE("GenericGui::showDockWidgets " << isVisible);
  if (_dwTree) _dwTree->setVisible(isVisible);
  if (_dwStacked) _dwStacked->setVisible(isVisible);
  if (_dwCatalogs) _dwCatalogs->setVisible(isVisible);
}

std::list<std::string> GenericGui::getMachineList()
{
  if (!_machineList.empty()) return _machineList;

  YACS::ENGINE::RuntimeSALOME* runTime = YACS::ENGINE::getSALOMERuntime();
  CORBA::ORB_ptr orb = runTime->getOrb();
  if (!orb) return _machineList;

  SALOME_NamingService namingService(orb);
  SALOME_LifeCycleCORBA lcc(&namingService);

  CORBA::Object_var obj =
    namingService.Resolve(SALOME_ResourcesManager::_ResourcesManagerNameInNS);
  if (CORBA::is_nil(obj)) return _machineList;

  Engines::ResourcesManager_var resManager = Engines::ResourcesManager::_narrow(obj);
  if(!resManager) return _machineList;

  Engines::CompoList compoList ;
  Engines::MachineParameters params;
  lcc.preSet(params);

  Engines::MachineList* machineList =
    resManager->GetFittingResources(params, compoList);

  for (int i = 0; i < machineList->length(); i++)
    {
      const char* aMachine = (*machineList)[i];
      _machineList.push_back(aMachine);
    }

  return _machineList;
}

// -----------------------------------------------------------------------------

/*! a QtGuiContext instance is created for each schema in edition or each execution or run
 *  of a schema. The context stores references of a lot of objects related to the edition
 *  or execution.
 *  \param proc       the schema to run or edit, loaded from file or just created empty.
 *  \param schemaName filename of the schema in edition, also used in execution to publish
 *                    a run under the edition name in Salome object browser.
 *  \param runName    filename of the schema in execution, not used in edition.
 *  \param forEdition if true, edition mode, if false, execution mode.
 */
void GenericGui::createContext(YACS::ENGINE::Proc* proc,
                               const QString& schemaName,
                               const QString& runName,
                               bool forEdition)
{
  DEBTRACE("GenericGui::createContext");

  QString fileName;
  QWidget* refWindow = 0; // --- used only on run to refer to the schema in edition
  if (forEdition)
    {
      fileName = schemaName;
    }
  else
    {
      fileName = runName;
      if (QtGuiContext::getQtCurrent())
        refWindow = QtGuiContext::getQtCurrent()->getWindow();
    }

  QtGuiContext* context = new QtGuiContext(this);
  QtGuiContext::setQtCurrent(context);

  // --- catalogs

  context->setEdition(forEdition);
  context->setSessionCatalog(_sessionCatalog);
  context->setFileName(fileName);
  context->setCurrentCatalog(YACS::ENGINE::getSALOMERuntime()->loadCatalog("proc", fileName.toStdString()));

  // --- scene, viewWindow & GraphicsView

  Scene *scene = new Scene();
  QWidget *viewWindow = _wrapper->getNewWindow(scene);
  _mapViewContext[viewWindow] = context;
  GraphicsView* gView = new GraphicsView(viewWindow);
  gView->setScene(scene);
  gView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  _wrapper->AssociateViewToWindow(gView, viewWindow);
  context->setScene(scene);
  context->setView(gView);
  context->setWindow(viewWindow);
  gView->show();

  // --- Schema Model

  SchemaModel *schemaModel = new SchemaModel(context, viewWindow);
  schemaModel->setEdition(forEdition);
  context->setSchemaModel(schemaModel);
  RootSceneItem *rootSceneItem = new RootSceneItem(context);

  // --- tree associated to model

  FormEditTree *editTree = new FormEditTree(_dwTree);
  editTree->setMinimumHeight(400);
  _dwTree->setWidget(editTree);
  context->setEditTree(editTree);
  editTree->tv_schema->setModel(schemaModel);
  context->setSelectionModel(editTree->tv_schema->selectionModel());

  QObject::connect(editTree->tv_schema->selectionModel(),
                   SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                   schemaModel,
                   SLOT(updateSelection(const QItemSelection &, const QItemSelection &)));

  // --- stacked widget

  QStackedWidget *stacked = new QStackedWidget(_dwStacked);
  _dwStacked->setWidget(stacked);
  context->setStackedWidget(stacked);
  YACS::HMI::ItemEdition* rootEdit = new YACS::HMI::ItemEdition(context,
                                                                stacked,
                                                                context->getName().c_str());

  QObject::connect(schemaModel,
                   SIGNAL(signalSelection(const QModelIndex &)),
                   editTree->tv_schema,
                   SLOT(viewSelection(const QModelIndex &)));

  // --- load schema

  proc->setEdition(forEdition);
  context->setProc(proc);
  if (forEdition && _wrapper)  // --- Edition mode
    {
      _wrapper->createNewSchema(fileName, viewWindow);
    }
  else if (_wrapper)           // --- Execution Mode
    {
      GuiExecutor *guiExec = new GuiExecutor(proc);
      context->setGuiExecutor(guiExec);
      _wrapper->createNewRun(schemaName, fileName, refWindow, viewWindow);
    }


  QtGuiContext::getQtCurrent()->getSubjectProc()->update(UPDATE,
                                                         ProcInvoc::getTypeOfNode(proc),
                                                         0); // --- force validity check

  // --- adjust widgets

  TreeView *vtree = dynamic_cast<TreeView*>(editTree->tv_schema);
  assert(vtree);
  vtree->resizeColumns();
  _catalogsWidget->setMinimumWidth(0); // --- reset the constraint on width
  editTree->setMinimumHeight(0);

  // --- show menus

  if (forEdition)
    {
      showEditionMenus(true);
      showExecMenus(false);
    }
  else
    {
      showEditionMenus(false);
      showExecMenus(true);
    }
}

// -----------------------------------------------------------------------------

void GenericGui::onNewSchema()
{
  DEBTRACE("GenericGui::onNewSchema");

  std::stringstream name;
  name << "newSchema_" << ++_schemaCnt;

  YACS::ENGINE::RuntimeSALOME* runTime = YACS::ENGINE::getSALOMERuntime();
  YACS::ENGINE::Proc *proc = runTime->createProc(name.str());

  QString fileName = name.str().c_str();
  createContext(proc, fileName, "", true);
}

void GenericGui::onImportSchema()
{
  DEBTRACE("GenericGui::onImportSchema");
  QString fn = QFileDialog::getOpenFileName( _parent,
                                             "Choose a filename to load" ,
                                             QString::null,
                                             tr( "XML-Files (*.xml);;All Files (*)" ));
  if ( !fn.isEmpty() )
    {
      DEBTRACE("***************************************************************************");
      DEBTRACE("file loaded : " <<fn.toStdString());
      DEBTRACE("***************************************************************************");
      YACS::ENGINE::Proc *proc = _loader->load(fn.toLatin1());
      YACS::ENGINE::Logger* logger= proc->getLogger("parser");
      if(!logger->isEmpty())
        {
          DEBTRACE(logger->getStr());
        }
      createContext(proc, fn, "", true);
    }
}

void GenericGui::onImportSupervSchema()
{
  DEBTRACE("GenericGui::onImportSupervSchema");
}

//! bug confirmOverwrite : correction Qt 4.3.5
QString GenericGui::getSaveFileName(const QString& fileName)
{
  QFileDialog dialog(_parent, "Save schema", fileName);
  QStringList filters;
  filters << "XML files (*.xml)"
          << "Any files (*)";
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setFilters(filters);
  dialog.selectFilter("(*.xml)");
  dialog.setDefaultSuffix("xml");
  //dialog.setConfirmOverwrite(true); // bug Qt4.3.3
  dialog.setConfirmOverwrite(false);  // bug Qt4.3.3
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  QString selectedFile;
  QStringList fileNames;
  fileNames.clear();
  if (bool ret = dialog.exec())
    {
      //DEBTRACE(ret << " " << dialog.confirmOverwrite());
      fileNames = dialog.selectedFiles();
      if (!fileNames.isEmpty())
        selectedFile = fileNames.first();
    }
  return selectedFile;
}

void GenericGui::onExportSchema()
{
  DEBTRACE("GenericGui::onExportSchema");
  if (!QtGuiContext::getQtCurrent()) return;
  YACS::ENGINE::Proc* proc = QtGuiContext::getQtCurrent()->getProc();
  QString fo = QtGuiContext::getQtCurrent()->getFileName();
  QString foo = fo;
  QString fn = fo;
  if (fo.startsWith("newSchema_"))
    {
      fo.clear();
      fn = getSaveFileName(fo);
    }
  if (fn.isEmpty()) return;

  DEBTRACE("GenericGui::onExportSchema: " << fn.toStdString());
  QtGuiContext::getQtCurrent()->setFileName(fn);
  VisitorSaveGuiSchema aWriter(proc);
  aWriter.openFileSchema( fn.toStdString() );
  aWriter.visitProc(proc);
  aWriter.closeFileSchema();

  if (fn.compare(foo) && _wrapper)
    _wrapper->renameSchema(foo, fn, QtGuiContext::getQtCurrent()->getWindow());
}

void GenericGui::onExportSchemaAs()
{
  DEBTRACE("GenericGui::onExportSchemaAs");
  if (!QtGuiContext::getQtCurrent()) return;
  YACS::ENGINE::Proc* proc = QtGuiContext::getQtCurrent()->getProc();
  QString fo = QtGuiContext::getQtCurrent()->getFileName();
  QString foo = fo;
  if (fo.startsWith("newShema_")) fo.clear();
  QString fn = getSaveFileName(fo);
  if (fn.isEmpty()) return;

  DEBTRACE("GenericGui::onExportSchemaAs: " << fn.toStdString());
  QtGuiContext::getQtCurrent()->setFileName(fn);
  VisitorSaveGuiSchema aWriter(proc);
  aWriter.openFileSchema(fn.toStdString());
  aWriter.visitProc(proc);
  aWriter.closeFileSchema();

  if (fn.compare(foo) && _wrapper)
    _wrapper->renameSchema(foo, fn, QtGuiContext::getQtCurrent()->getWindow());
}

void GenericGui::onImportCatalog()
{
  DEBTRACE("GenericGui::onImportCatalog");
  QString fn = QFileDialog::getOpenFileName( _parent,
                                             "Choose a YACS Schema to load as a Catalog" ,
                                             QString::null,
                                             tr( "XML-Files (*.xml);;All Files (*)" ));
  if ( !fn.isEmpty() )
    _catalogsWidget->addCatalogFromFile(fn.toStdString());
}
  
void GenericGui::onRunLoadedSchema()
{
  DEBTRACE("GenericGui::onRunLoadedSchema");
  if (!QtGuiContext::getQtCurrent()) return;

  // --- check proc state (cf. editionProc)

  if (!QtGuiContext::getQtCurrent()->_setOfModifiedSubjects.empty()) return;

  YACS::ENGINE::Proc* proc = QtGuiContext::getQtCurrent()->getProc();
  if (!proc->isValid()) return;

  YACS::ENGINE::LinkInfo info(YACS::ENGINE::LinkInfo::ALL_STOP_ASAP);
  proc->checkConsistency(info);
  if (info.areWarningsOrErrors()) return;
 
  // --- save proc under a run name

  QFileInfo fo = QtGuiContext::getQtCurrent()->getFileName();
  QString procName = fo.baseName();
  QString tmpDir = SALOMEDS_Tool::GetTmpDir().c_str();
  QDir aTmpDir(tmpDir);
  aTmpDir.mkdir(QString("YACS_") + getenv("USER"));
  assert(aTmpDir.cd(QString("YACS_") + getenv("USER")));
  QDateTime curTime = QDateTime::currentDateTime();   
  QString aRunName = procName + "_" + curTime.toString("yyyyMMdd_hhmmss");
  aRunName = aTmpDir.absoluteFilePath(aRunName);
  DEBTRACE(aRunName.toStdString());

  VisitorSaveGuiSchema aWriter(proc);
  aWriter.openFileSchema(aRunName.toStdString());
  aWriter.visitProc(proc);
  aWriter.closeFileSchema();

  // --- create a run context

  YACS::ENGINE::Proc *procrun = _loader->load(aRunName.toLatin1());
  createContext(procrun, QtGuiContext::getQtCurrent()->getFileName(), aRunName, false);

}

void GenericGui::onLoadAndRunSchema()
{
  DEBTRACE("GenericGui::onLoadAndRunSchema");
  QString fn = QFileDialog::getOpenFileName( _parent,
                                             "Choose a filename to load" ,
                                             QString::null,
                                             tr( "XML-Files (*.xml);;All Files (*)" ));
  if ( !fn.isEmpty() )
    {
      DEBTRACE("***************************************************************************");
      DEBTRACE("file loaded : " <<fn.toStdString());
      DEBTRACE("***************************************************************************");
      YACS::ENGINE::Proc *proc = _loader->load(fn.toLatin1());
      YACS::ENGINE::Logger* logger= proc->getLogger("parser");
      if(!logger->isEmpty())
        {
          DEBTRACE(logger->getStr());
        }
      createContext(proc, fn, fn, false);
    }

}

void GenericGui::onExecute()
{
  DEBTRACE("GenericGui::onExecute");
  if (!QtGuiContext::getQtCurrent()) return;
  if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
  QtGuiContext::getQtCurrent()->getGuiExecutor()->runDataflow();
}

void GenericGui::onAbort()
{
  DEBTRACE("GenericGui::onAbort");
  if (!QtGuiContext::getQtCurrent()) return;
  if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
  QtGuiContext::getQtCurrent()->getGuiExecutor()->killDataflow();
}

void GenericGui::onSuspendResume()
{
  DEBTRACE("GenericGui::onSuspendResume");
  if (!QtGuiContext::getQtCurrent()) return;
  if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
  QtGuiContext::getQtCurrent()->getGuiExecutor()->suspendResumeDataflow();
}

void GenericGui::onPause()
{
  DEBTRACE("GenericGui::onPause");
  if (!QtGuiContext::getQtCurrent()) return;
  if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
  QtGuiContext::getQtCurrent()->getGuiExecutor()->suspendDataflow();
}

void GenericGui::onResume()
{
  DEBTRACE("GenericGui::onResume");
  if (!QtGuiContext::getQtCurrent()) return;
  if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
  QtGuiContext::getQtCurrent()->getGuiExecutor()->resumeDataflow();
}

void GenericGui::onEditDataTypes()
{
  DEBTRACE("GenericGui::onEditDataTypes");
}

void GenericGui::onCreateDataType()
{
  DEBTRACE("GenericGui::onCreateDataType");
}

void GenericGui::onImportDataType()
{
  DEBTRACE("GenericGui::onImportDataType");
}


void GenericGui::onNewContainer()
{
  DEBTRACE("GenericGui::onNewContainer");
  _guiEditor->CreateContainer();
}

void GenericGui::onNewSalomeComponent()
{
  DEBTRACE("GenericGui::onNewSalomeComponent");
}

void GenericGui::onNewSalomePythonComponent()
{
  DEBTRACE("GenericGui::onNewSalomePythonComponent");
}

void GenericGui::onNewCorbaComponent()
{
  DEBTRACE("GenericGui::onNewCorbaComponent");
}


void GenericGui::onSalomeServiceNode()
{
  DEBTRACE("GenericGui::onSalomeServiceNode");
  _guiEditor->CreateNode("SalomeNode");
}

void GenericGui::onServiceInlineNode()
{
  DEBTRACE("GenericGui::onServiceInlineNode");
  _guiEditor->CreateNode("SalomePythonNode");
}

void GenericGui::onCORBAServiceNode()
{
  DEBTRACE("GenericGui::onCORBAServiceNode");
  _guiEditor->CreateNode("CORBANode");
}

void GenericGui::onNodeNodeServiceNode()
{
  DEBTRACE("GenericGui::onNodeNodeServiceNode");
}

void GenericGui::onCppNode()
{
  DEBTRACE("GenericGui::onCppNode");
  _guiEditor->CreateNode("CppNode");
}

void GenericGui::onXMLNode()
{
  DEBTRACE("GenericGui::onXMLNode");
  _guiEditor->CreateNode("XmlNode");
}

void GenericGui::onInDataNode()
{
  DEBTRACE("GenericGui::onInDataNode");
  _guiEditor->CreateNode("PresetNode");
}

void GenericGui::onOutDataNode()
{
  DEBTRACE("GenericGui::onOutDataNode");
  _guiEditor->CreateNode("OutNode");
}

void GenericGui::onInStudyNode()
{
  DEBTRACE("GenericGui::onInStudyNode");
  _guiEditor->CreateNode("StudyInNode");
}

void GenericGui::onOutStudyNode()
{
  DEBTRACE("GenericGui::onOutStudyNode");
  _guiEditor->CreateNode("StudyOutNode");
}

void GenericGui::onInlineScriptNode()
{
  DEBTRACE("GenericGui::onInlineScriptNode");
  _guiEditor->CreateNode("PyScript");
}

void GenericGui::onInlineFunctionNode()
{
  DEBTRACE("GenericGui::onInlineFunctionNode");
  _guiEditor->CreateNode("PyFunction");
}

void GenericGui::onBlockNode()
{
  DEBTRACE("GenericGui::onBlockNode");
  _guiEditor->CreateBloc();
}

void GenericGui::onFORNode()
{
  DEBTRACE("GenericGui::onFORNode");
  _guiEditor->CreateForLoop();
}

void GenericGui::onFOREACHNode()
{
  DEBTRACE("GenericGui::onFOREACHNode");
  _guiEditor->CreateForEachLoop();
}

void GenericGui::onWHILENode()
{
  DEBTRACE("GenericGui::onWHILENode");
  _guiEditor->CreateWhileLoop();
}

void GenericGui::onSWITCHNode()
{
  DEBTRACE("GenericGui::onSWITCHNode");
  _guiEditor->CreateSwitch();
}

void GenericGui::onNodeFromCatalog()
{
  DEBTRACE("GenericGui::onNodeFromCatalog");
}

void GenericGui::onDeleteItem()
{
  DEBTRACE("GenericGui::onDeleteItem");
  _guiEditor->DeleteSubject();
}

void GenericGui::onRebuildLinks()
{
  DEBTRACE("GenericGui::onRebuildLinks");
  _guiEditor->rebuildLinks();
}

void GenericGui::onToggleAutomaticComputeLinks(bool checked)
{
  Scene::_autoComputeLinks = checked;
  DEBTRACE("Scene::_autoComputeLinks=" << checked);
}

void GenericGui::onToggleSimplifyLinks(bool checked)
{
  Scene::_simplifyLinks = checked;
  DEBTRACE("Scene::_simplifyLinks=" << checked);
}

void GenericGui::onToggleForce2NodesLinks(bool checked)
{
  Scene::_force2NodesLink  = checked;
  DEBTRACE("Scene::_force2NodesLink=" << checked);
}

void GenericGui::onSelectReference()
{
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  if (!sub) return;
  SubjectReference *ref = dynamic_cast<SubjectReference*>(sub);
  assert(ref);
  SubjectServiceNode *snode = dynamic_cast<SubjectServiceNode*>(ref->getReference());
  snode->select(true);
}

void GenericGui::onWithoutStopMode(bool checked)
{
  DEBTRACE("GenericGui::onWithoutStopMode " << checked);
  if (!QtGuiContext::getQtCurrent()) return;
  if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
  if (checked) QtGuiContext::getQtCurrent()->getGuiExecutor()->setContinueMode();
}

void GenericGui::onBreakpointsMode(bool checked)
{
  DEBTRACE("GenericGui::onWithoutStopMode " << checked);
  if (!QtGuiContext::getQtCurrent()) return;
  if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
  if (checked) QtGuiContext::getQtCurrent()->getGuiExecutor()->setBreakpointMode();
}

void GenericGui::onStepByStepMode(bool checked)
{
  DEBTRACE("GenericGui::onWithoutStopMode " << checked);
  if (!QtGuiContext::getQtCurrent()) return;
  if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
  if (checked) QtGuiContext::getQtCurrent()->getGuiExecutor()->setStepByStepMode();
}
