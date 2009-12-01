//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include <Python.h>
#include "SALOME_ResourcesManager.hxx"
#include "SALOME_LifeCycleCORBA.hxx"

#include "RuntimeSALOME.hxx"
#include "Proc.hxx"
#include "InputPort.hxx"
#include "ServiceNode.hxx"
#include "parsers.hxx"
#include "Logger.hxx"
#include "YACSGuiLoader.hxx"
#include "ComponentInstance.hxx"

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
#include "LogViewer.hxx"
#include "chrono.hxx"
#include "Resource.hxx"

#include <QFileDialog>
#include <sstream>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>
#include <QWhatsThis>

#include <cstdlib>

#include <ctime>

#ifdef WNT
#define WEXITSTATUS(w)  ((int) ((w) & 0x40000000))
#endif

//#define _DEVDEBUG_
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
  _isSaved = false;
  _mapViewContext.clear();
  _machineList.clear();
  _menuId = 190;
  QtGuiContext::_counters = new counters(100);
  srand((unsigned)time(0)); 

  GuiObserver::setEventMap();

  string iconPath = getenv("YACS_ROOT_DIR");
  iconPath += "/share/salome/resources/yacs";
  DEBTRACE(iconPath);
  QDir::addSearchPath("icons", iconPath.c_str());

  YACS::ENGINE::RuntimeSALOME::setRuntime();
  _loader = new YACSGuiLoader();
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
              runTime->addCatalog(_sessionCatalog);
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
  _dwStacked->setMinimumWidth(270); // --- force a minimum until display
  _parent->addDockWidget(Qt::RightDockWidgetArea, _dwStacked);
  _dwCatalogs = new QDockWidget(_parent);
  _dwCatalogs->setWindowTitle("Catalogs");
  _parent->addDockWidget(Qt::RightDockWidgetArea, _dwCatalogs);
  _catalogsWidget = new CatalogWidget(_dwCatalogs,
                                      _builtinCatalog,
                                      _sessionCatalog);
  _dwCatalogs->setWidget(_catalogsWidget);

  _parent->tabifyDockWidget(_dwStacked, _dwCatalogs);
  _parent->tabifyDockWidget(_dwTree, _wrapper->objectBrowser());

  //Import user catalog
  std::string usercata=Resource::userCatalog.toStdString();
  _catalogsWidget->addCatalogFromFile(usercata);
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

  pixmap.load("icons:schema.png");
  _newSchemaAct = _wrapper->createAction(getMenuId(), tr("Create a new YACS Schema"), QIcon(pixmap),
                                         tr("New Schema"), tr("Create a new YACS Schema"),
                                         0, _parent, false, this,  SLOT(onNewSchema()));
  _newSchemaAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_N); // --- QKeySequence::New ambiguous in SALOME

  pixmap.load("icons:import_dataflow.png");
  _importSchemaAct = _wrapper->createAction(getMenuId(), tr("Import a YACS Schema for edition"), QIcon(pixmap),
                                            tr("Import Schema"), tr("Import a YACS Schema for edition"),
                                            0, _parent, false, this,  SLOT(onImportSchema()));
  _importSchemaAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_O); // --- QKeySequence::Open ambiguous in SALOME
  
  pixmap.load("icons:import_superv_dataflow.png");
  _importSupervSchemaAct = _wrapper->createAction(getMenuId(), tr("Import a SUPERV Schema for edition"), QIcon(pixmap),
                                                  tr("Import SUPERV Schema"), tr("Import a SUPERV Schema for edition"),
                                                  0, _parent, false, this,  SLOT(onImportSupervSchema()));
  
  pixmap.load("icons:save_dataflow.png");
  _exportSchemaAct = _wrapper->createAction(getMenuId(), tr("Save the current YACS Schema"), QIcon(pixmap),
                                            tr("Save Schema"), tr("Save the current YACS Schema"),
                                            0, _parent, false, this,  SLOT(onExportSchema()));
  _exportSchemaAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S); // --- QKeySequence::Save ambiguous in SALOME
  
  pixmap.load("icons:export_dataflow.png");
  _exportSchemaAsAct = _wrapper->createAction(getMenuId(), tr("Save the current YACS Schema As..."), QIcon(pixmap),
                                              tr("Save Schema As"), tr("Save the current YACS Schema As..."),
                                              0, _parent, false, this,  SLOT(onExportSchemaAs()));
  //_exportSchemaAsAct->setShortcut(QKeySequence::SaveAs); // --- ambiguous in SALOME

  pixmap.load("icons:insert_file.png");
  _importCatalogAct = _wrapper->createAction(getMenuId(), tr("Import a Schema as a Catalog"), QIcon(pixmap),
                                             tr("Import Catalog"), tr("Import a Schema as a Catalog"),
                                             0, _parent, false, this,  SLOT(onImportCatalog()));


  pixmap.load("icons:run_active.png");
  _runLoadedSchemaAct = _wrapper->createAction(getMenuId(), tr("Prepare the current edited schema for run"), QIcon(pixmap),
                                               tr("Run Current Schema"), tr("Prepare the current edited schema for run"),
                                               0, _parent, false, this,  SLOT(onRunLoadedSchema()));

  pixmap.load("icons:load_execution_state.png");
  _loadRunStateSchemaAct = _wrapper->createAction(getMenuId(), tr("Load a previous run state for this schema, prepare to run"), QIcon(pixmap),
                                                  tr("Load Run State"), tr("Load a previous run state for this schema, prepare to run"),
                                                  0, _parent, false, this,  SLOT(onLoadRunStateSchema()));

  pixmap.load("icons:run.png");
  _loadAndRunSchemaAct = _wrapper->createAction(getMenuId(), tr("Load a schema for run"), QIcon(pixmap),
                                                tr("Load Schema to run"), tr("Load a schema for run"),
                                                0, _parent, false, this,  SLOT(onLoadAndRunSchema()));

  pixmap.load("icons:batch.png");
  _loadBatchAct = _wrapper->createAction(getMenuId(), tr("Load Batch Schema for edition"), QIcon(pixmap),
                                         tr("Load Batch Schema"), tr("Load Batch Schema for edition"),
                                         0, _parent, false, this,  SLOT(onBatch()));

  pixmap.load("icons:suspend_resume.png");
  _startResumeAct = _wrapper->createAction(getMenuId(), tr("Start or Resume Schema execution"), QIcon(pixmap),
                                           tr("Start/Resume execution"), tr("Start or Resume Schema execution"),
                                           0, _parent, false, this,  SLOT(onStartResume()));

  pixmap.load("icons:kill.png");
  _abortAct = _wrapper->createAction(getMenuId(), tr("Abort the current execution"), QIcon(pixmap),
                                     tr("Abort execution"), tr("Abort the current execution"),
                                     0, _parent, false, this,  SLOT(onAbort()));

  pixmap.load("icons:pause.png");
  _pauseAct = _wrapper->createAction(getMenuId(), tr("Suspend the current execution"), QIcon(pixmap),
                                     tr("Suspend execution"), tr("Suspend the current execution"),
                                     0, _parent, false, this,  SLOT(onPause()));

  pixmap.load("icons:reset.png");
  _resetAct = _wrapper->createAction(getMenuId(), tr("Reset the current execution"), QIcon(pixmap),
                                     tr("Reset execution"), tr("Reset the current execution"),
                                     0, _parent, false, this,  SLOT(onReset()));


  pixmap.load("icons:save_dataflow_state.png");
  _saveRunStateAct = _wrapper->createAction(getMenuId(), tr("Save the current run state"), QIcon(pixmap),
                                            tr("Save State"), tr("Save the current run state"),
                                            0, _parent, false, this,  SLOT(onSaveRunState()));

  pixmap.load("icons:new_edition.png");
  _newEditionAct = _wrapper->createAction(getMenuId(), tr("Edit again the current schema in a new context"), QIcon(pixmap),
                                          tr("Edit Again"), tr("Edit again the current schema in a new context"),
                                          0, _parent, false, this,  SLOT(onNewEdition()));


  pixmap.load("icons:change_informations.png");
  _getYacsContainerLogAct = _wrapper->createAction(getMenuId(), tr("get YACS container log"), QIcon(pixmap),
                                                   tr("YACS Container Log"), tr("get YACS container log"),
                                                   0, _parent, false, this,  SLOT(onGetYacsContainerLog()));

  pixmap.load("icons:filter_notification.png");
  _getErrorReportAct = _wrapper->createAction(getMenuId(), tr("get Node Error Report"), QIcon(pixmap),
                                              tr("Node Error Report"), tr("get Node Error Report"),
                                              0, _parent, false, this,  SLOT(onGetErrorReport()));

  pixmap.load("icons:icon_text.png");
  _getErrorDetailsAct = _wrapper->createAction(getMenuId(), tr("get Node Error Details"), QIcon(pixmap),
                                               tr("Node Error Details"), tr("get Node Error Details"),
                                               0, _parent, false, this,  SLOT(onGetErrorDetails()));

  pixmap.load("icons:change_informations.png");
  _getContainerLogAct = _wrapper->createAction(getMenuId(), tr("get Node Container Log"), QIcon(pixmap),
                                               tr("Node Container Log"), tr("get Node Container Log"),
                                               0, _parent, false, this,  SLOT(onGetContainerLog()));



  pixmap.load("icons:kill.png");
  _editDataTypesAct = _wrapper->createAction(getMenuId(), tr("Edit Data Types"), QIcon(pixmap),
                                             tr("Edit Data Types"), tr("Edit Data Types"),
                                             0, _parent, false, this,  SLOT(onEditDataTypes()));

  pixmap.load("icons:kill.png");
  _createDataTypeAct = _wrapper->createAction(getMenuId(), tr("Create Data Types"), QIcon(pixmap),
                                              tr("Create Data Types"), tr("Create Data Types"),
                                              0, _parent, false, this,  SLOT(onCreateDataType()));

  pixmap.load("icons:folder_cyan.png");
  _importDataTypeAct = _wrapper->createAction(getMenuId(), tr("Import Data Types, use drag and drop from catalog"), QIcon(pixmap),
                                              tr("Import Data Types"), tr("Import Data Types, use drag and drop from catalog"),
                                              0, _parent, false, this,  SLOT(onImportDataType()));

  pixmap.load("icons:container.png");
  _newContainerAct = _wrapper->createAction(getMenuId(), tr("Create a New Container"), QIcon(pixmap),
                                            tr("Create Container"), tr("Create a New Container"),
                                            0, _parent, false, this,  SLOT(onNewContainer()));

  pixmap.load("icons:icon_select.png");
  _selectComponentInstanceAct = _wrapper->createAction(getMenuId(), tr("Select a Component Instance"), QIcon(pixmap),
                                                       tr("Select a Component Instance"), tr("Select a Component Instance"),
                                                       0, _parent, false, this,  SLOT(onSelectComponentInstance()));

  pixmap.load("icons:new_salome_component.png");
  _newSalomeComponentAct = _wrapper->createAction(getMenuId(), tr("Create a New SALOME Component Instance"), QIcon(pixmap),
                                                  tr("new Component Instance"), tr("Create a New SALOME Component Instance"),
                                                  0, _parent, false, this,  SLOT(onNewSalomeComponent()));

  pixmap.load("icons:new_salomepy_component.png");
  _newSalomePythonComponentAct = _wrapper->createAction(getMenuId(), tr("Create a New SALOME Python Component"), QIcon(pixmap),
                                                        tr("SALOME Python Component"), tr("Create a New SALOME Python Component"),
                                                        0, _parent, false, this,  SLOT(onNewSalomePythonComponent()));

  pixmap.load("icons:new_corba_component.png");
  _newCorbaComponentAct = _wrapper->createAction(getMenuId(), tr("Create a New CORBA Component"), QIcon(pixmap),
                                                 tr("CORBA Component"), tr("Create a New CORBA Component"),
                                                 0, _parent, false, this,  SLOT(onNewCorbaComponent()));

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

  pixmap.load("icons:node.png");
  _inDataNodeAct = _wrapper->createAction(getMenuId(), tr("Create a New Input data Node"), QIcon(pixmap),
                                          tr("Input Data Node"), tr("Create a New Input data Node"),
                                          0, _parent, false, this,  SLOT(onInDataNode()));

  pixmap.load("icons:node.png");
  _outDataNodeAct = _wrapper->createAction(getMenuId(), tr("Create a New Output data Node"), QIcon(pixmap),
                                           tr("Output Data Node"), tr("Create a New Output data Node"),
                                           0, _parent, false, this,  SLOT(onOutDataNode()));

  pixmap.load("icons:node.png");
  _inStudyNodeAct = _wrapper->createAction(getMenuId(), tr("Create a New Input Study Node"), QIcon(pixmap),
                                           tr("Input Study Node"), tr("Create a New Input Study Node"),
                                           0, _parent, false, this,  SLOT(onInStudyNode()));

  pixmap.load("icons:node.png");
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

  pixmap.load("icons:new_for_loop_node.png");
  _OptimizerLoopAct = _wrapper->createAction(getMenuId(), tr("Create a New Optimizer Loop Node"), QIcon(pixmap),
                                             tr("Optimizer Loop"), tr("Create a New Optimizer Loop"),
                                             0, _parent, false, this,  SLOT(onOptimizerLoop()));

  pixmap.load("icons:new_from_library_node.png");
  _nodeFromCatalogAct = _wrapper->createAction(getMenuId(), tr("Create a New Node from Catalog, use drag and drop from catalog"), QIcon(pixmap),
                                               tr("Node from Catalog"), tr("Create a New Node from Catalog, use drag and drop from catalog"),
                                               0, _parent, false, this,  SLOT(onNodeFromCatalog()));

  pixmap.load("icons:delete.png");
  _deleteItemAct = _wrapper->createAction(getMenuId(), tr("Delete a Schema Item"), QIcon(pixmap),
                                          tr("Delete Item"), tr("Delete a Schema Item"),
                                          0, _parent, false, this,  SLOT(onDeleteItem()));
  _deleteItemAct->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_D); // --- QKeySequence::Delete dangerous...

  pixmap.load("icons:cut.png");
  _cutItemAct = _wrapper->createAction(getMenuId(), tr("Cut a Schema Item"), QIcon(pixmap),
                                       tr("Cut Item"), tr("Cut a Schema Item"),
                                       0, _parent, false, this,  SLOT(onCutItem()));
  _cutItemAct->setShortcut(QKeySequence::Cut);

  pixmap.load("icons:copy.png");
  _copyItemAct = _wrapper->createAction(getMenuId(), tr("Copy a Schema Item"), QIcon(pixmap),
                                        tr("Copy Item"), tr("Copy a Schema Item"),
                                        0, _parent, false, this,  SLOT(onCopyItem()));
  _copyItemAct->setShortcut(QKeySequence::Copy);

  pixmap.load("icons:paste.png");
  _pasteItemAct = _wrapper->createAction(getMenuId(), tr("Paste a Schema Item"), QIcon(pixmap),
                                         tr("Paste Item"), tr("Paste a Schema Item"),
                                         0, _parent, false, this,  SLOT(onPasteItem()));
  _pasteItemAct->setShortcut(QKeySequence::Paste);

  _putInBlocAct = _wrapper->createAction(getMenuId(), tr("Put node in block"), QIcon(pixmap),
                                         tr("Put node in block"), tr("Put node in block"),
                                         0, _parent, false, this,  SLOT(onPutInBloc()));

  pixmap.load("icons:arrange_nodes.png");
  _arrangeLocalNodesAct = _wrapper->createAction(getMenuId(), tr("arrange nodes on that bloc level, without recursion"), QIcon(pixmap),
                                                 tr("arrange local nodes"), tr("arrange nodes on that bloc level, without recursion"),
                                                 0, _parent, false, this,  SLOT(onArrangeLocalNodes()));

  pixmap.load("icons:sample.png");
  _arrangeRecurseNodesAct = _wrapper->createAction(getMenuId(), tr("arrange nodes on that bloc level, with recursion"), QIcon(pixmap),
                                                   tr("arrange nodes recursion"), tr("arrange nodes on that bloc level, with recursion"),
                                                   0, _parent, false, this,  SLOT(onArrangeRecurseNodes()));

  pixmap.load("icons:rebuild_links.png");
  _computeLinkAct = _wrapper->createAction(getMenuId(), tr("compute orthogonal links"), QIcon(pixmap),
                                           tr("compute links"), tr("compute orthogonal links"),
                                           0, _parent, false, this,  SLOT(onRebuildLinks()));

  pixmap.load("icons:zoomToBloc.png");
  _zoomToBlocAct = _wrapper->createAction(getMenuId(), tr("zoom 2D view to selected bloc"), QIcon(pixmap),
                                          tr("zoom to bloc"), tr("zoom 2D view to the selected composed node"),
                                          0, _parent, false, this,  SLOT(onZoomToBloc()));

  pixmap.load("icons:centerOnNode.png");
  _centerOnNodeAct = _wrapper->createAction(getMenuId(), tr("center 2D view on selected node"), QIcon(pixmap),
                                            tr("center on node"), tr("center 2D view on selected node"),
                                            0, _parent, false, this,  SLOT(onCenterOnNode()));
  _centerOnNodeAct->setShortcut(QKeySequence::Find);

  pixmap.load("icons:straightLink.png");
  _toggleStraightLinksAct = _wrapper->createAction(getMenuId(), tr("draw straight or orthogonal links"), QIcon(pixmap),
                                                   tr("straight/orthogonal"), tr("draw straight or orthogonal links"),
                                                   0, _parent, true, this,  SLOT(onToggleStraightLinks(bool)));
  
  _toggleStraightLinksAct->setChecked(Resource::straightLinks);
  onToggleStraightLinks(Resource::straightLinks);

  pixmap.load("icons:autoComputeLink.png");
  _toggleAutomaticComputeLinkAct = _wrapper->createAction(getMenuId(), tr("compute othogonal links automatically when nodes move"), QIcon(pixmap),
                                                          tr("automatic link"), tr("compute othogonal links automatically when nodes move"),
                                                          0, _parent, true, this,  SLOT(onToggleAutomaticComputeLinks(bool)));

  _toggleAutomaticComputeLinkAct->setChecked(Resource::autoComputeLinks);
  onToggleAutomaticComputeLinks(Resource::autoComputeLinks); // Why is this needed ?

  pixmap.load("icons:simplifyLink.png");
  _toggleSimplifyLinkAct = _wrapper->createAction(getMenuId(), tr("simplify links by removing unnecessary direction changes"), QIcon(pixmap),
                                                  tr("simplify links"), tr("simplify links by removing unnecessary direction changes"),
                                                  0, _parent, true, this,  SLOT(onToggleSimplifyLinks(bool)));
  _toggleSimplifyLinkAct->setChecked(Resource::simplifyLink);
  onToggleSimplifyLinks(Resource::simplifyLink);

  pixmap.load("icons:force2nodeLink.png");
  _toggleForce2NodesLinkAct = _wrapper->createAction(getMenuId(), tr("force orthogonal links by adding an edge on simples links"), QIcon(pixmap),
                                                     tr("force ortho links"), tr("force orthogonal links by adding an edge on simples links"),
                                                     0, _parent, true, this,  SLOT(onToggleForce2NodesLinks(bool)));
  _toggleForce2NodesLinkAct->setChecked(true);

  pixmap.load("icons:addRowCols.png");
  _toggleAddRowColsAct = _wrapper->createAction(getMenuId(), tr("allow more path for the links, for a better separation"), QIcon(pixmap),
                                                tr("separate links"), tr("allow more path for the links, for a better separation"),
                                                0, _parent, true, this,  SLOT(onToggleAddRowCols(bool)));
  _toggleAddRowColsAct->setChecked(Resource::addRowCols);
  onToggleAddRowCols(Resource::addRowCols);

  pixmap.load("icons:ob_service_node.png");
  _selectReferenceAct = _wrapper->createAction(getMenuId(), tr("select reference"), QIcon(pixmap),
                                               tr("select reference"), tr("select reference"),
                                               0, _parent, false, this,  SLOT(onSelectReference()));

  pixmap.load("icons:whatsThis.png");
  _whatsThisAct = _wrapper->createAction(getMenuId(), tr("active whatsThis Mode to get help on widgets"), QIcon(pixmap),
                                         tr("whatsThis Mode"), tr("active whatsThis Mode to get help on widgets"),
                                         0, _parent, false, this,  SLOT(onWhatsThis()));
  _whatsThisAct->setShortcut(QKeySequence::WhatsThis);

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

  pixmap.load("icons:toggle_stop_on_error.png");
  _toggleStopOnErrorAct = _wrapper->createAction(getMenuId(), tr("Force stop on first error during execution"), QIcon(pixmap),
                                                 tr("stop on error"), tr("Force stop on first error during execution"),
                                                 0, _parent, true, this,  SLOT(onToggleStopOnError(bool)));

  pixmap.load("icons:toggleVisibility.png");
  _toggleSceneItemVisibleAct = _wrapper->createAction(getMenuId(), tr("toggle 2D scene item visibility"), QIcon(pixmap),
                                                      tr("visible/hidden"), tr("toggle 2D scene item visibility"),
                                                      0, _parent, true, this,  SLOT(onToggleSceneItemVisible(bool)));



  pixmap.load("icons:showLink.png");
  _showAllLinksAct = _wrapper->createAction(getMenuId(), tr("Show all the links"), QIcon(pixmap),
                                            tr("show all links"), tr("Show all the links"),
                                            0, _parent, false, this,  SLOT(onShowAllLinks()));

  pixmap.load("icons:hideLink.png");
  _hideAllLinksAct = _wrapper->createAction(getMenuId(), tr("Hide all the links"), QIcon(pixmap),
                                            tr("hide all links"), tr("Hide all the links"),
                                            0, _parent, false, this,  SLOT(onHideAllLinks()));
  

  pixmap.load("icons:showLink.png");
  _showOnlyPortLinksAct = _wrapper->createAction(getMenuId(), tr("Show only links from/to this port"), QIcon(pixmap),
                                                 tr("show only links"), tr("Show only links from/to this port"),
                                                 0, _parent, false, this,  SLOT(onShowOnlyPortLinks()));

  pixmap.load("icons:showLink.png");
  _showPortLinksAct = _wrapper->createAction(getMenuId(), tr("Show links from/to this port"), QIcon(pixmap),
                                             tr("show links"), tr("Show links from/to this port"),
                                             0, _parent, false, this,  SLOT(onShowPortLinks()));

  pixmap.load("icons:hideLink.png");
  _hidePortLinksAct = _wrapper->createAction(getMenuId(), tr("Hide links from/to this port"), QIcon(pixmap),
                                             tr("hide links"), tr("Hide links from/to this port"),
                                             0, _parent, false, this,  SLOT(onHidePortLinks()));
  
  
  pixmap.load("icons:showLink.png");
  _showOnlyCtrlLinksAct = _wrapper->createAction(getMenuId(), tr("Show only control links from/to this node"), QIcon(pixmap),
                                                 tr("show only Control links"), tr("Show only control links from/to this node"),
                                                 0, _parent, false, this,  SLOT(onShowOnlyCtrlLinks()));

  pixmap.load("icons:showLink.png");
  _showCtrlLinksAct = _wrapper->createAction(getMenuId(), tr("Show control links from/to this node"), QIcon(pixmap),
                                             tr("show control links"), tr("Show control links from/to this node"),
                                             0, _parent, false, this,  SLOT(onShowCtrlLinks()));

  pixmap.load("icons:hideLink.png");
  _hideCtrlLinksAct = _wrapper->createAction(getMenuId(), tr("Hide control links from/to this node"), QIcon(pixmap),
                                             tr("hide control links"), tr("Hide control links from/to this node"),
                                             0, _parent, false, this,  SLOT(onHideCtrlLinks()));

  
  pixmap.load("icons:showLink.png");
  _showOnlyLinkAct = _wrapper->createAction(getMenuId(), tr("Show only this link"), QIcon(pixmap),
                                            tr("show only"), tr("Show only this link"),
                                            0, _parent, false, this,  SLOT(onShowOnlyLink()));

  pixmap.load("icons:showLink.png");
  _showLinkAct = _wrapper->createAction(getMenuId(), tr("Show this link"), QIcon(pixmap),
                                        tr("show"), tr("Show this link"),
                                        0, _parent, false, this,  SLOT(onShowLink()));
  
  pixmap.load("icons:hideLink.png");
  _hideLinkAct = _wrapper->createAction(getMenuId(), tr("Hide this link"), QIcon(pixmap),
                                        tr("hide"), tr("Hide this link"),
                                        0, _parent, false, this,  SLOT(onHideLink()));


  pixmap.load("icons:emphasisLink.png");
  _emphasisPortLinksAct = _wrapper->createAction(getMenuId(), tr("emphasis on links from/to this port"), QIcon(pixmap),
                                                 tr("emphasize links"), tr("emphasis on links from/to this port"),
                                                 0, _parent, false, this,  SLOT(onEmphasisPortLinks()));
  
  pixmap.load("icons:emphasisLink.png");
  _emphasisCtrlLinksAct = _wrapper->createAction(getMenuId(), tr("emphasis on control links from/to this node"), QIcon(pixmap),
                                                 tr("emphasize control links"), tr("emphasis on control links from/to this node"),
                                                 0, _parent, false, this,  SLOT(onEmphasisCtrlLinks()));
  
  pixmap.load("icons:emphasisLink.png");
  _emphasisLinkAct = _wrapper->createAction(getMenuId(), tr("emphasis on this link"), QIcon(pixmap),
                                            tr("emphasize"), tr("emphasis on this link"),
                                            0, _parent, false, this,  SLOT(onEmphasisLink()));

  pixmap.load("icons:deEmphasisLink.png");
  _deEmphasizeAllAct = _wrapper->createAction(getMenuId(), tr("remove all emphasis"), QIcon(pixmap),
                                              tr("remove all emphasis"), tr("remove all emphasis"),
                                              0, _parent, false, this,  SLOT(onDeEmphasizeAll()));
  

  pixmap.load("icons:undo.png");
  _undoAct = _wrapper->createAction(getMenuId(), tr("undo last action"), QIcon(pixmap),
                                    tr("undo"), tr("undo last action"),
                                    0, _parent, false, this,  SLOT(onUndo()));
  _undoAct->setShortcut(QKeySequence::Undo);
  
  pixmap.load("icons:redo.png");
  _redoAct = _wrapper->createAction(getMenuId(), tr("redo last action"), QIcon(pixmap),
                                    tr("redo"), tr("redo last action"),
                                    0, _parent, false, this,  SLOT(onRedo()));
  _redoAct->setShortcut(QKeySequence::Redo);
  

  _execModeGroup = new QActionGroup(this);
  _execModeGroup->addAction(_withoutStopModeAct);
  _execModeGroup->addAction(_breakpointsModeAct);
  _execModeGroup->addAction(_stepByStepModeAct);
  _withoutStopModeAct->setChecked(true);
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
  _wrapper->createMenu( _loadRunStateSchemaAct, aMenuId );
  _wrapper->createMenu( _loadAndRunSchemaAct, aMenuId );
  _wrapper->createMenu( _loadBatchAct, aMenuId );
  _wrapper->createMenu( _wrapper->separator(), aMenuId);
  _wrapper->createMenu( _undoAct, aMenuId );
  _wrapper->createMenu( _redoAct, aMenuId );
  _wrapper->createMenu( _wrapper->separator(), aMenuId);
  _wrapper->createMenu( _startResumeAct, aMenuId );
  _wrapper->createMenu( _abortAct, aMenuId );
  _wrapper->createMenu( _pauseAct, aMenuId );
  _wrapper->createMenu( _resetAct, aMenuId );
  _wrapper->createMenu( _wrapper->separator(), aMenuId);
  _wrapper->createMenu( _saveRunStateAct, aMenuId );
  //_wrapper->createMenu( _newEditionAct, aMenuId );
  _wrapper->createMenu( _wrapper->separator(), aMenuId);
  _wrapper->createMenu( _withoutStopModeAct, aMenuId );
  _wrapper->createMenu( _breakpointsModeAct, aMenuId );
  _wrapper->createMenu( _stepByStepModeAct, aMenuId );
  _wrapper->createMenu( _wrapper->separator(), aMenuId);
  _wrapper->createMenu( _toggleStopOnErrorAct, aMenuId );
  _wrapper->createMenu( _wrapper->separator(), aMenuId);
  _wrapper->createMenu( _importCatalogAct, aMenuId );
  _wrapper->createMenu( _wrapper->separator(), aMenuId);
  _wrapper->createMenu( _toggleStraightLinksAct, aMenuId );
  _wrapper->createMenu( _toggleAutomaticComputeLinkAct, aMenuId );
  _wrapper->createMenu( _toggleSimplifyLinkAct, aMenuId );
  _wrapper->createMenu( _toggleForce2NodesLinkAct, aMenuId );
  _wrapper->createMenu( _toggleAddRowColsAct, aMenuId );
  _wrapper->createMenu( _wrapper->separator(), aMenuId);
  _wrapper->createMenu( _showAllLinksAct, aMenuId );
  _wrapper->createMenu( _hideAllLinksAct, aMenuId );
  _wrapper->createMenu( _wrapper->separator(), aMenuId);
  _wrapper->createMenu( _whatsThisAct, aMenuId );
}

void GenericGui::createTools()
{
  int aToolId = _wrapper->createTool ( tr( "YACS Toolbar" ) );
  _wrapper->createTool( _newSchemaAct, aToolId );
  _wrapper->createTool( _importSchemaAct, aToolId );
  _wrapper->createTool( _wrapper->separator(), aToolId );
  _wrapper->createTool( _exportSchemaAct, aToolId );
  _wrapper->createTool( _exportSchemaAsAct, aToolId );
  _wrapper->createTool( _wrapper->separator(), aToolId);
  _wrapper->createTool( _runLoadedSchemaAct, aToolId );
  _wrapper->createTool( _loadRunStateSchemaAct, aToolId );
  _wrapper->createTool( _loadAndRunSchemaAct, aToolId );
  _wrapper->createTool( _loadBatchAct, aToolId );
  _wrapper->createTool( _wrapper->separator(), aToolId );
  _wrapper->createTool( _undoAct, aToolId );
  _wrapper->createTool( _redoAct, aToolId );
  _wrapper->createTool( _wrapper->separator(), aToolId );
  _wrapper->createTool( _startResumeAct, aToolId );
  _wrapper->createTool( _abortAct, aToolId );
  _wrapper->createTool( _pauseAct, aToolId );
  _wrapper->createTool( _resetAct, aToolId );
  _wrapper->createTool( _wrapper->separator(), aToolId );
  _wrapper->createTool( _saveRunStateAct, aToolId );
  //_wrapper->createTool( _newEditionAct, aToolId );
  _wrapper->createTool( _withoutStopModeAct, aToolId );
  _wrapper->createTool( _breakpointsModeAct, aToolId );
  _wrapper->createTool( _stepByStepModeAct, aToolId );
  _wrapper->createTool( _wrapper->separator(), aToolId );
  _wrapper->createTool( _toggleStopOnErrorAct, aToolId );
  _wrapper->createTool( _wrapper->separator(), aToolId );
  _wrapper->createTool( _importCatalogAct, aToolId );
  _wrapper->createTool( _wrapper->separator(), aToolId );
  _wrapper->createTool( _toggleStraightLinksAct, aToolId );
  _wrapper->createTool( _toggleAutomaticComputeLinkAct, aToolId );
  _wrapper->createTool( _toggleSimplifyLinkAct, aToolId );
  //_wrapper->createTool( _toggleForce2NodesLinkAct, aToolId );
  _wrapper->createTool( _toggleAddRowColsAct, aToolId );
  _wrapper->createTool( _wrapper->separator(), aToolId );
  _wrapper->createTool( _showAllLinksAct, aToolId );
  _wrapper->createTool( _hideAllLinksAct, aToolId );
  _wrapper->createTool( _wrapper->separator(), aToolId );
  _wrapper->createTool( _whatsThisAct, aToolId );
}

void GenericGui::initialMenus()
{
  showEditionMenus(false);
  showExecMenus(false);
  showCommonMenus(false);
  showBaseMenus(true);
}

void GenericGui::showBaseMenus(bool show)
{
  DEBTRACE("GenericGui::showBaseMenus " << show);
  _wrapper->setMenuShown(_newSchemaAct, show);
  _wrapper->setToolShown(_newSchemaAct, show);
  _wrapper->setMenuShown(_importSchemaAct, show);
  _wrapper->setToolShown(_importSchemaAct, show);
  _wrapper->setMenuShown(_importSupervSchemaAct, show);
  _wrapper->setMenuShown(_loadAndRunSchemaAct, show);
  _wrapper->setToolShown(_loadAndRunSchemaAct, show);
  _wrapper->setMenuShown(_whatsThisAct, show);
  _wrapper->setToolShown(_whatsThisAct, show);
}

void GenericGui::showEditionMenus(bool show)
{
  DEBTRACE("GenericGui::showEditionMenus " << show);
  _wrapper->setMenuShown(_exportSchemaAct, show);
  _wrapper->setToolShown(_exportSchemaAct, show);
  _wrapper->setMenuShown(_exportSchemaAsAct, show);
  _wrapper->setToolShown(_exportSchemaAsAct, show);
  _wrapper->setMenuShown(_runLoadedSchemaAct, show);
  _wrapper->setToolShown(_loadRunStateSchemaAct, show);
  _wrapper->setMenuShown(_loadRunStateSchemaAct, show);
  _wrapper->setToolShown(_runLoadedSchemaAct, show);
  _wrapper->setMenuShown(_undoAct, show);
  _wrapper->setToolShown(_undoAct, show);
  _wrapper->setMenuShown(_redoAct, show);
  _wrapper->setToolShown(_redoAct, show);
  _wrapper->setMenuShown(_loadBatchAct, show);
  _wrapper->setToolShown(_loadBatchAct, show);
  _wrapper->setMenuShown(_importCatalogAct, show);
  _wrapper->setToolShown(_importCatalogAct, show);
}

void GenericGui::showExecMenus(bool show)
{
  DEBTRACE("GenericGui::showExecMenus " << show);
  _wrapper->setMenuShown(_startResumeAct, show);
  _wrapper->setToolShown(_startResumeAct, show);
  _wrapper->setMenuShown(_abortAct, show);
  _wrapper->setToolShown(_abortAct, show);
  _wrapper->setMenuShown(_pauseAct, show);
  _wrapper->setToolShown(_pauseAct, show);
  _wrapper->setMenuShown(_resetAct, show);
  _wrapper->setToolShown(_resetAct, show);
  _wrapper->setMenuShown(_saveRunStateAct, show);
  _wrapper->setToolShown(_saveRunStateAct, show);
  //_wrapper->setMenuShown(_newEditionAct, show);
  //_wrapper->setToolShown(_newEditionAct, show);
  _wrapper->setMenuShown(_withoutStopModeAct, show);
  _wrapper->setToolShown(_withoutStopModeAct, show);
  _wrapper->setMenuShown(_breakpointsModeAct, show);
  _wrapper->setToolShown(_breakpointsModeAct, show);
  _wrapper->setMenuShown(_stepByStepModeAct, show);
  _wrapper->setToolShown(_stepByStepModeAct, show);
  _wrapper->setMenuShown(_toggleStopOnErrorAct, show);
  _wrapper->setToolShown(_toggleStopOnErrorAct, show);
}

void GenericGui::showCommonMenus(bool show)
{
  DEBTRACE("GenericGui::showCommonMenus " << show);
  _wrapper->setMenuShown(_toggleStraightLinksAct, show);
  _wrapper->setToolShown(_toggleStraightLinksAct, show);
  _wrapper->setMenuShown(_toggleAutomaticComputeLinkAct, show);
  _wrapper->setToolShown(_toggleAutomaticComputeLinkAct, show);
  _wrapper->setMenuShown(_toggleSimplifyLinkAct, show);
  _wrapper->setToolShown(_toggleSimplifyLinkAct, show);
  _wrapper->setMenuShown(_toggleForce2NodesLinkAct, show);
  //_wrapper->setToolShown(_toggleForce2NodesLinkAct, show);
  _wrapper->setMenuShown(_toggleAddRowColsAct, show);
  _wrapper->setToolShown(_toggleAddRowColsAct, show);
  _wrapper->setMenuShown(_showAllLinksAct, show);
  _wrapper->setToolShown(_showAllLinksAct, show);
  _wrapper->setMenuShown(_hideAllLinksAct, show);
  _wrapper->setToolShown(_hideAllLinksAct, show);
}

void GenericGui::switchContext(QWidget *view)
{
  DEBTRACE("GenericGui::switchContext " << view);
  if (! _mapViewContext.count(view))
    {
      initialMenus();
      _dwTree->setWidget(0);
      _dwStacked->setWidget(0);
      return;
    }
  QtGuiContext* newContext = _mapViewContext[view];

  _dwTree->setWidget(newContext->getEditTree());
  _dwTree->widget()->show();
  _dwTree->raise();
  _dwStacked->setWidget(newContext->getStackedWidget());

  QtGuiContext::setQtCurrent(newContext);

  if (newContext->isEdition())
    {
      showExecMenus(false);
      showBaseMenus(true);
      showEditionMenus(true);
      showCommonMenus(true);
      if (_dwTree) _dwTree->setWindowTitle("Tree View: edition mode");
    }
  else
    {
      showEditionMenus(false);
      showBaseMenus(true);
      showExecMenus(true);
      showCommonMenus(true);
      _withoutStopModeAct->setChecked(true);
      if (_dwTree) _dwTree->setWindowTitle("Tree View: execution mode");
    }
  _dwStacked->setMinimumWidth(10);
}

bool GenericGui::closeContext(QWidget *view)
{
  DEBTRACE("GenericGui::closeContext");
  if (! _mapViewContext.count(view))
    return true;
  QtGuiContext* context = _mapViewContext[view];
  switchContext(view);

  bool tryToSave = false;

  if (QtGuiContext::getQtCurrent()->isEdition())
    {
      if (!QtGuiContext::getQtCurrent()->_setOfModifiedSubjects.empty())
        {
          QMessageBox msgBox;
          msgBox.setText("Some elements are modified and not taken into account.");
          string info = "do you want to apply your changes ?\n";
          info += " - Save    : do not take into account edition in progress,\n";
          info += "             but if there are other modifications, select a file name for save\n";
          info += " - Discard : discard all modifications and close the schema\n";
          info += " - Cancel  : do not close the schema, return to edition";
          msgBox.setInformativeText(info.c_str());
          msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
          msgBox.setDefaultButton(QMessageBox::Cancel);
          int ret = msgBox.exec();
          switch (ret)
            {
            case QMessageBox::Save:
              tryToSave = true;
              break;
            case QMessageBox::Discard:
              tryToSave = false;
              break;
            case QMessageBox::Cancel:
            default:
              return false;
              break;
            }
        }
      else
        if (QtGuiContext::getQtCurrent()->isNotSaved())
          {
            QMessageBox msgBox;
            msgBox.setText("The schema has been modified");
            string info = "do you want to save the schema ?\n";
            info += " - Save    : select a file name for save\n";
            info += " - Discard : discard all modifications and close the schema\n";
            info += " - Cancel  : do not close the schema, return to edition";
            msgBox.setInformativeText(info.c_str());
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Cancel);
            int ret = msgBox.exec();
            switch (ret)
              {
              case QMessageBox::Save:
                tryToSave = true;
                break;
              case QMessageBox::Discard:
                tryToSave = false;
                break;
              case QMessageBox::Cancel:
              default:
                return false;
                break;
              }
          }

      if (tryToSave)
        {
          onExportSchemaAs();
          if (! _isSaved) // --- probably, user has cancelled the save dialog. Do not close
            return false;
        }
    }

  map<QWidget*, YACS::HMI::QtGuiContext*>::iterator it = _mapViewContext.begin();
  QtGuiContext* newContext = 0;
  QWidget* newView = 0;
  for (; it != _mapViewContext.end(); ++it)
    {
      if ((*it).second != context)
        {
          newView = (*it).first;
          newContext = (*it).second;
          break;
        }
    }
  int studyId = _wrapper->activeStudyId();
  if (context->getStudyId() == studyId)
    {
      _wrapper->deleteSchema(view);
      DEBTRACE("delete context");
      if (GuiExecutor* exec = context->getGuiExecutor())
        {
          exec->closeContext();
        }
      delete context;
      _mapViewContext.erase(view);
      switchContext(newView);
    }
  return true;
}

void GenericGui::showDockWidgets(bool isVisible)
{
  DEBTRACE("GenericGui::showDockWidgets " << isVisible);
  if (_dwTree) _dwTree->setVisible(isVisible);
  if (_dwTree) _dwTree->toggleViewAction()->setVisible(isVisible);
  if (_dwStacked) _dwStacked->setVisible(isVisible);
  if (_dwStacked) _dwStacked->toggleViewAction()->setVisible(isVisible);
  if (_dwCatalogs) _dwCatalogs->setVisible(isVisible);
  if (_dwCatalogs) _dwCatalogs->toggleViewAction()->setVisible(isVisible);
}

void GenericGui::raiseStacked()
{
  if (_dwStacked) _dwStacked->raise();
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

  Engines::MachineParameters params;
  lcc.preSet(params);

  Engines::MachineList* machineList =
    resManager->GetFittingResources(params);

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
  clock_t  start_t;
  clock_t  end_t;
  start_t = clock();


  QWidget* central = _parent->centralWidget();
  if (central)
    central->setFocus();
  else
    DEBTRACE("No Central Widget");

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
  context->setCurrentCatalog(_builtinCatalog);

  // --- scene, viewWindow & GraphicsView

  Scene *scene = new Scene();
  QWidget *viewWindow = _wrapper->getNewWindow(scene);
  _mapViewContext[viewWindow] = context;
  GraphicsView* gView = new GraphicsView(viewWindow);
  gView->setScene(scene);
  gView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  int studyId = _wrapper->AssociateViewToWindow(gView, viewWindow);
  context->setStudyId(studyId);
  std::ostringstream value;
  value << studyId;
  proc->setProperty("DefaultStudyID",value.str());
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
  editTree->show();
  context->setEditTree(editTree);
  editTree->tv_schema->setModel(schemaModel);
  context->setSelectionModel(editTree->tv_schema->selectionModel());
  _dwTree->raise();

  QObject::connect(editTree->tv_schema->selectionModel(),
                   SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                   schemaModel,
                   SLOT(updateSelection(const QItemSelection &, const QItemSelection &)));

  // --- stacked widget

  QStackedWidget *stacked = new QStackedWidget(_dwStacked);
  _dwStacked->setWidget(stacked);
  context->setStackedWidget(stacked);
  YACS::HMI::ItemEditionRoot* rootEdit = new YACS::HMI::ItemEditionRoot(context,
                                                                        0,
                                                                        context->getName().c_str());
  context->setEditionRoot(rootEdit);

  QObject::connect(schemaModel,
                   SIGNAL(signalSelection(const QModelIndex &)),
                   editTree->tv_schema,
                   SLOT(viewSelection(const QModelIndex &)));

  // --- load schema

  proc->setEdition(forEdition);

  {
    end_t = clock();
    double passe =  (end_t -start_t);
    passe = passe/CLOCKS_PER_SEC;
    DEBTRACE("create context -1- : " << passe);
    start_t = end_t;
  }

  context->setLoading(true);

  context->setProc(proc);
  setLoadedPresentation(proc);

  {
    end_t = clock();
    double passe =  (end_t -start_t);
    passe = passe/CLOCKS_PER_SEC;
    DEBTRACE("create context - load proc- : " << passe);
    start_t = end_t;
  }

  context->setLoading(false);

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
  YASSERT(vtree);
  vtree->resizeColumns();
  _catalogsWidget->setMinimumWidth(10); // --- reset the constraint on width
  editTree->setMinimumHeight(40);
  _dwStacked->setMinimumWidth(10);
  // --- show menus

  if (forEdition)
    {
      showExecMenus(false);
      showEditionMenus(true);
      showCommonMenus(true);
      if (_dwTree) _dwTree->setWindowTitle("Tree View: edition mode");
    }
  else
    {
      showEditionMenus(false);
      showExecMenus(true);
      showCommonMenus(true);
      _withoutStopModeAct->setChecked(true);
      if (_dwTree) _dwTree->setWindowTitle("Tree View: execution mode");
    }

  QtGuiContext::getQtCurrent()->setNotSaved(false);
}

// -----------------------------------------------------------------------------

void GenericGui::setLoadedPresentation(YACS::ENGINE::Proc* proc)
{
  DEBTRACE("GenericGui::setLoadedPresentation");
  map<YACS::ENGINE::Node*, PrsData> presNodes = _loader->getPrsData(proc);
  if (!presNodes.empty())
    {
      map<YACS::ENGINE::Node*, PrsData>::iterator it = presNodes.begin();
      for (; it!= presNodes.end(); ++it)
        {
          YACS::ENGINE::Node* node = (*it).first;
          PrsData pres = (*it).second;
          SubjectNode *snode = QtGuiContext::getQtCurrent()->_mapOfSubjectNode[node];
          SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[snode];
          YASSERT(item);
          item->setPos(QPointF(pres._x, pres._y));
          item->setWidth(pres._width);
          item->setHeight(pres._height);
        }
    }
  if (Scene::_autoComputeLinks)
    _guiEditor->rebuildLinks();
}

// -----------------------------------------------------------------------------

void GenericGui::onNewSchema()
{
  DEBTRACE("GenericGui::onNewSchema");

  std::stringstream name;
  name << "newSchema_" << ++_schemaCnt;

  YACS::ENGINE::RuntimeSALOME* runTime = YACS::ENGINE::getSALOMERuntime();
  YACS::ENGINE::Proc *proc = runTime->createProc(name.str());

  _loader->reset();

  QString fileName = name.str().c_str();
  createContext(proc, fileName, "", true);
}

void GenericGui::loadSchema(const std::string& filename,bool edit)
{
  YACS::ENGINE::Proc *proc = _loader->load(filename.c_str());
  if (!proc)
    return;
  YACS::ENGINE::Logger* logger= proc->getLogger("parser");
  if(!logger->isEmpty())
    {
      DEBTRACE(logger->getStr());
    }
  QString fn=QString::fromStdString(filename);
  if(edit)
    createContext(proc, fn, "", true);
  else
    createContext(proc, fn, fn, false);
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
      DEBTRACE("file loaded : " <<fn.toStdString());
      YACS::ENGINE::Proc *proc = _loader->load(fn.toLatin1());
      if (!proc)
        {
          QMessageBox msgBox(QMessageBox::Critical,
                             "Import YACS Schema, native YACS XML format",
                             "The file has not the native YACS XML format or is not readable.");
          msgBox.exec();
          return;
        }
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
  QString fn = QFileDialog::getOpenFileName( _parent,
                                             "Choose a  SUPERV filename to load" ,
                                             QString::null,
                                             tr( "XML-Files (*.xml);;All Files (*)" ));
  if (fn.isEmpty()) return;

  DEBTRACE("file loaded : " <<fn.toStdString());
  QString tmpFileName;
  try
    {
#ifdef WNT
      QString tmpDir = getenv("TEMP");
#else
      QString tmpDir = "/tmp";
#endif
      QDir aTmpDir(tmpDir);
      aTmpDir.mkdir(QString("YACS_") + getenv("USER"));
      YASSERT(aTmpDir.cd(QString("YACS_") + getenv("USER")));
      QDateTime curTime = QDateTime::currentDateTime();   
      tmpFileName = "SUPERV_import_" + curTime.toString("yyyyMMdd_hhmmss") + ".xml";
      QString tmpOutput = "salomeloader_output";
      tmpFileName = aTmpDir.absoluteFilePath(tmpFileName);
      DEBTRACE(tmpFileName.toStdString());
      
      QString aCall = "salomeloader.sh " + fn + " " + tmpFileName + " > " + tmpOutput;
      DEBTRACE(aCall.toStdString());
      
      int ret = system(aCall.toAscii());
      if(ret != 0)
        {
          // --- read file with logs
          fstream f(tmpOutput.toAscii());
          stringstream hfile;
          hfile << f.rdbuf();
          f.close();
          
          // --- Problem in execution
          int status=WEXITSTATUS(ret);
          if(status == 1)
            {
              QString mes = "Problems in conversion: some errors but an incomplete proc has nevertheless been created.\n\n";
              mes += QString(hfile.str().c_str());
              QMessageBox msgBox(QMessageBox::Warning,
                                 "Import YACS Schema, SUPERV XML format",
                                 mes);
              msgBox.exec();
            }
          else if(status == 2)
            {
              QString mes = "Problems in conversion: a fatal error has been encountered. The proc can't be created.\n\n";
              mes += QString(hfile.str().c_str());
              QMessageBox msgBox(QMessageBox::Critical,
                                 "Import YACS Schema, SUPERV XML format",
                                 mes);
              msgBox.exec();
              return;
            }
          else
            {
              DEBTRACE("Unknown problem: " << ret );
              QMessageBox msgBox(QMessageBox::Critical,
                                 "Import YACS Schema, SUPERV XML format",
                                 "Unexpected exception in salomeloader.");
              msgBox.exec();
              return;
            }
        }
    }
  catch(...)
    {
      QMessageBox msgBox(QMessageBox::Critical,
                         "Import YACS Schema, SUPERV XML format",
                         "Unexpected exception in convertSupervFile");
      msgBox.exec();
      return;
    }

  fn = tmpFileName;
  if (fn.isEmpty()) return; // must not happen

  DEBTRACE("file loaded : " <<fn.toStdString());
  YACS::ENGINE::Proc *proc = _loader->load(fn.toLatin1());
  if (!proc)
    {
      QMessageBox msgBox(QMessageBox::Critical,
                         "Import YACS Schema, SUPERV file converted in native YACS XML format",
                         "The file has not the native YACS XML format or is not readable.");
      msgBox.exec();
      return;
    }
  YACS::ENGINE::Logger* logger= proc->getLogger("parser");
  if(!logger->isEmpty())
    {
      DEBTRACE(logger->getStr());
    }
  createContext(proc, fn, "", true);
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
  dialog.setConfirmOverwrite(true);
  //dialog.setConfirmOverwrite(false);  // bug Qt4.3.3
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  QString selectedFile;
  QStringList fileNames;
  fileNames.clear();
  if (bool ret = dialog.exec())
    {
      DEBTRACE(ret << " " << dialog.confirmOverwrite());
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
  aWriter.visitProc();
  aWriter.closeFileSchema();
  QtGuiContext::getQtCurrent()->setNotSaved(false);

  if (fn.compare(foo) && _wrapper)
    _wrapper->renameSchema(foo, fn, QtGuiContext::getQtCurrent()->getWindow());
}

void GenericGui::onExportSchemaAs()
{
  DEBTRACE("GenericGui::onExportSchemaAs");
  _isSaved = false;
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
  aWriter.visitProc();
  aWriter.closeFileSchema();
  _isSaved = true;
  QtGuiContext::getQtCurrent()->setNotSaved(false);

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
  
void GenericGui::onRunLoadedSchema(bool withState)
{
  DEBTRACE("GenericGui::onRunLoadedSchema");
  if (!QtGuiContext::getQtCurrent()) return;

  clock_t  start_t;
  clock_t  end_t;
  start_t = clock();

  // --- check proc state (cf. editionProc)

  if (!QtGuiContext::getQtCurrent()->_setOfModifiedSubjects.empty())
    {
      set<Subject*>::iterator it = QtGuiContext::getQtCurrent()->_setOfModifiedSubjects.begin();
      (*it)->select(true);
      return;
    }

  YACS::ENGINE::Proc* proc = QtGuiContext::getQtCurrent()->getProc();
  if (!proc->isValid())
    {
      QtGuiContext::getQtCurrent()->getSubjectProc()->select(true);
      return;
    }

  YACS::ENGINE::LinkInfo info(YACS::ENGINE::LinkInfo::ALL_STOP_ASAP);
  try
    {
      proc->checkConsistency(info);
    }
  catch (Exception &ex)
    {
      DEBTRACE(ex.what());
      QtGuiContext::getQtCurrent()->getSubjectProc()->select(true);
      return;
    }
  if (info.areWarningsOrErrors()) return;
 
  {
    end_t = clock();
    double passe =  (end_t -start_t);
    passe = passe/CLOCKS_PER_SEC;
    cerr <<"run schema -1- : " << passe << endl;
    start_t = end_t;
  }
  // --- save proc under a run name

  QFileInfo fo = QtGuiContext::getQtCurrent()->getFileName();
  QString procName = fo.baseName();
  //QString tmpDir = SALOMEDS_Tool::GetTmpDir().c_str();
#ifdef WNT
  QString tmpDir = getenv("TEMP");
#else
  QString tmpDir = "/tmp";
#endif
  QDir aTmpDir(tmpDir);
  aTmpDir.mkdir(QString("YACS_") + getenv("USER"));
  YASSERT(aTmpDir.cd(QString("YACS_") + getenv("USER")));
  QDateTime curTime = QDateTime::currentDateTime();   
  QString aRunName = procName + "_" + curTime.toString("yyyyMMdd_hhmmss") + ".xml";
  aRunName = aTmpDir.absoluteFilePath(aRunName);
  DEBTRACE(aRunName.toStdString());

  VisitorSaveGuiSchema aWriter(proc);
  aWriter.openFileSchema(aRunName.toStdString());
  aWriter.visitProc();
  aWriter.closeFileSchema();

  // --- create a run context

  YACS::ENGINE::Proc *procrun = _loader->load(aRunName.toLatin1());
  {
    end_t = clock();
    double passe =  (end_t -start_t);
    passe = passe/CLOCKS_PER_SEC;
    cerr <<"run schema -2- : " << passe << endl;
    start_t = end_t;
  }
  createContext(procrun, QtGuiContext::getQtCurrent()->getFileName(), aRunName, false);

  // load state if required

  GuiExecutor *executor = QtGuiContext::getQtCurrent()->getGuiExecutor();
  if (!executor) return;
  if (withState)
    {
      QString fn = QFileDialog::getOpenFileName( _parent,
                                                 "Choose a previous run state to load" ,
                                                 QString::null,
                                                 tr( "XML-Files (*.xml);;All Files (*)" ));
      if (!fn.isEmpty())
        {
          DEBTRACE("run state to load: " <<fn.toStdString());
          executor->setLoadStateFile(fn.toStdString());
        }
    }
  executor->startResumeDataflow(true); // --- initialise gui state
}

void GenericGui::onLoadRunStateSchema()
{
  DEBTRACE("GenericGui::onLoadRunStateSchema");
  onRunLoadedSchema(true);
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
      DEBTRACE("file loaded : " <<fn.toStdString());
      YACS::ENGINE::Proc *proc = _loader->load(fn.toLatin1());
      if (!proc)
        {
          QMessageBox msgBox(QMessageBox::Critical,
                             "Import YACS Schema, native YACS XML format",
                             "The file has not the native YACS XML format or is not readable.");
          msgBox.exec();
          return;
        }
      YACS::ENGINE::Logger* logger= proc->getLogger("parser");
      if(!logger->isEmpty())
        {
          DEBTRACE(logger->getStr());
        }
      createContext(proc, fn, "", true);
      onRunLoadedSchema();
    }
}

void GenericGui::onBatch() {
  DEBTRACE("GenericGui::onBatch");

  if (!QtGuiContext::getQtCurrent()) return;

  // Save the current schema
  YACS::ENGINE::Proc* proc = QtGuiContext::getQtCurrent()->getProc();
  VisitorSaveGuiSchema aWriter(proc);
  aWriter.openFileSchema("/tmp/graph_user.xml");
  aWriter.visitProc();
  aWriter.closeFileSchema();

  // Load the Batch Schema
  QString fb = getenv("YACS_ROOT_DIR");
  fb += "/share/salome/resources/yacs/batch_graph.xml";
  DEBTRACE("file loaded : " << fb.toStdString());

  proc = _loader->load(fb.toLatin1());
  if (!proc) {
    QMessageBox msgBox(QMessageBox::Critical,
		       "Import Batch Schema, native YACS XML format",
		       "The batch_graph.xml has not the native YACS XML format or is not readable." );
    msgBox.exec();
  } else {
    YACS::ENGINE::Logger* logger= proc->getLogger("parser");
    if(!logger->isEmpty()) {
      DEBTRACE(logger->getStr());
    };
    QString fn = QtGuiContext::getQtCurrent()->getFileName();
    proc->nodeMap["Submit"]->getInputPort("graphfic")->edInit(fn.toLatin1().data());
    createContext(proc, fb, "", true);
  };
}

void GenericGui::onStartResume()
{
  DEBTRACE("GenericGui::onStartResume");
  if (!QtGuiContext::getQtCurrent()) return;
  if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
  QtGuiContext::getQtCurrent()->getGuiExecutor()->startResumeDataflow();
}

void GenericGui::onAbort()
{
  DEBTRACE("GenericGui::onAbort");
  if (!QtGuiContext::getQtCurrent()) return;
  if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
  QtGuiContext::getQtCurrent()->getGuiExecutor()->killDataflow();
}

void GenericGui::onPause()
{
  DEBTRACE("GenericGui::onPause");
  if (!QtGuiContext::getQtCurrent()) return;
  if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
  QtGuiContext::getQtCurrent()->getGuiExecutor()->suspendDataflow();
}

void GenericGui::onReset()
{
  DEBTRACE("GenericGui::onReset");
  if (!QtGuiContext::getQtCurrent()) return;
  if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
  QtGuiContext::getQtCurrent()->getGuiExecutor()->resetDataflow();
}

void GenericGui::onSaveRunState()
{
  DEBTRACE("GenericGui::onSaveRunState");
  if (!QtGuiContext::getQtCurrent()) return;
  if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
  QDateTime curTime = QDateTime::currentDateTime(); 
  QFileInfo procName = QtGuiContext::getQtCurrent()->getFileName();
  QString stateName = procName.baseName();
  DEBTRACE(stateName.toStdString());
  stateName += "_state_" + curTime.toString("yyyyMMdd_hhmmss") + ".xml";
  DEBTRACE(stateName.toStdString());
  stateName = getSaveFileName(stateName);
  DEBTRACE(stateName.toStdString());
  if (!stateName.isEmpty())
    QtGuiContext::getQtCurrent()->getGuiExecutor()->saveState(stateName.toStdString());
}

void GenericGui::onNewEdition()
{
  DEBTRACE("GenericGui::onNewEdition");
  //   if (!QtGuiContext::getQtCurrent()) return;
  //   if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
  //   QtGuiContext::getQtCurrent()->getGuiExecutor()->resetDataflow();
}

void GenericGui::onGetYacsContainerLog()
{
  DEBTRACE("GenericGui::onGetYacsContainerLog");
  if (!QtGuiContext::getQtCurrent()) return;
  if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
  string log = QtGuiContext::getQtCurrent()->getGuiExecutor()->getContainerLog();
  LogViewer *lv = new LogViewer("YACS Container Log", _parent);
  lv->readFile(log);
  lv->show();
}

void GenericGui::onGetErrorReport()
{
  DEBTRACE("GenericGui::onGetErrorReport");
  if (!QtGuiContext::getQtCurrent()) return;
  if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  SubjectNode *snode = dynamic_cast<SubjectNode*>(sub);
  if (!snode) return;
  string log = QtGuiContext::getQtCurrent()->getGuiExecutor()->getErrorReport(snode->getNode());
  LogViewer *lv = new LogViewer("Node error report", _parent);
  lv->setText(log);
  lv->show();
}

void GenericGui::onGetErrorDetails()
{
  DEBTRACE("GenericGui::onGetErrorDetails");
  if (!QtGuiContext::getQtCurrent()) return;
  if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  SubjectNode *snode = dynamic_cast<SubjectNode*>(sub);
  if (!snode) return;
  string log = QtGuiContext::getQtCurrent()->getGuiExecutor()->getErrorDetails(snode->getNode());
  LogViewer *lv = new LogViewer("Node Error Details", _parent);
  lv->setText(log);
  lv->show();
}

void GenericGui::onGetContainerLog()
{
  DEBTRACE("GenericGui::onGetContainerLog");
  if (!QtGuiContext::getQtCurrent()) return;
  if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  SubjectElementaryNode *snode = dynamic_cast<SubjectElementaryNode*>(sub);
  if (!snode) return;
  string log = QtGuiContext::getQtCurrent()->getGuiExecutor()->getContainerLog(snode->getNode());

  LogViewer *lv = new LogViewer("Node Container Log", _parent);
  if (log.empty())
    {
      string info = "\n";
      if (dynamic_cast<YACS::ENGINE::ServiceNode*>(snode->getNode()))
        {
          info +="The container log of this node\n";
          info += "is not stored in a file and \n";
          info += "can't be displayed here, \n";
          info += "but you can have a look at \n";
          info += "the SALOME standard output,\n";
          info += "on your terminal...";
        }
      else
        {
          info += "See YACS Container log \n";
          info += "(on main proc menu) \n";
          info += "for all inline nodes";
        }
      lv->setText(info);
    }
  else
    lv->readFile(log);
  lv->show();
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
  if (_dwCatalogs) _dwCatalogs->raise();
}

void GenericGui::onSelectComponentInstance()
{
  DEBTRACE("GenericGui::onSelectComponentInstance");
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  if (!sub) return;
  SubjectComponent *ref = dynamic_cast<SubjectComponent*>(sub);
  YASSERT(ref);
  YACS::ENGINE::ComponentInstance* compo=ref->getComponent();
  QtGuiContext::getQtCurrent()->_mapOfLastComponentInstance[compo->getCompoName()]=compo;
}

void GenericGui::onNewContainer()
{
  DEBTRACE("GenericGui::onNewContainer");
  _guiEditor->CreateContainer();
}

void GenericGui::onNewSalomeComponent()
{
  DEBTRACE("GenericGui::onNewSalomeComponent");
  _guiEditor->CreateComponentInstance();
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
  createForEachLoop("double");
}

void GenericGui::createForEachLoop(std::string type)
{
  DEBTRACE("GenericGui::createForEachLoop");
  _guiEditor->CreateForEachLoop(type);
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

void GenericGui::onOptimizerLoop()
{
  DEBTRACE("GenericGui::onOptimizerLoop");
  _guiEditor->CreateOptimizerLoop();
}

void GenericGui::onNodeFromCatalog()
{
  DEBTRACE("GenericGui::onNodeFromCatalog");
  if (_dwCatalogs) _dwCatalogs->raise();
}

void GenericGui::onDeleteItem()
{
  DEBTRACE("GenericGui::onDeleteItem");
  _guiEditor->DeleteSubject();
}

void GenericGui::onCutItem()
{
  DEBTRACE("GenericGui::onCutItem");
  _guiEditor->CutSubject();
}

void GenericGui::onCopyItem()
{
  DEBTRACE("GenericGui::onCopyItem");
  _guiEditor->CopySubject();
}

void GenericGui::onPasteItem()
{
  DEBTRACE("GenericGui::onPasteItem");
  _guiEditor->PasteSubject();
}

void GenericGui::onPutInBloc()
{
  _guiEditor->PutSubjectInBloc();
}

void GenericGui::onArrangeLocalNodes()
{
  DEBTRACE("GenericGui::onArrangeLocalNodes");
  _guiEditor->arrangeNodes(false);
}

void GenericGui::onArrangeRecurseNodes()
{
  DEBTRACE("GenericGui::onArrangeRecurseNodes");
  _guiEditor->arrangeNodes(true);
}

void GenericGui::onRebuildLinks()
{
  DEBTRACE("GenericGui::onRebuildLinks");
  _guiEditor->rebuildLinks();
}

void GenericGui::onZoomToBloc()
{
  DEBTRACE("GenericGui::onZoomToBloc");
  QtGuiContext::getQtCurrent()->getView()->onZoomToBloc();
}

void GenericGui::onCenterOnNode()
{
  DEBTRACE("GenericGui::onCenterOnNode");
  QtGuiContext::getQtCurrent()->getView()->onCenterOnNode();
}

void GenericGui::onToggleStraightLinks(bool checked)
{
  Scene::_straightLinks = checked;
  DEBTRACE("Scene::_straightLinks=" << checked);
  if (!QtGuiContext::getQtCurrent())
    return;
  map<Subject*, SchemaItem*>::const_iterator it = QtGuiContext::getQtCurrent()->_mapOfSchemaItem.begin();
  for( ; it != QtGuiContext::getQtCurrent()->_mapOfSchemaItem.end(); ++it)
    {
      Subject* sub = (*it).first;
      sub->update(SWITCHSHAPE, 0, 0);
    }
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

void GenericGui::onToggleAddRowCols(bool checked)
{
  Scene::_addRowCols  = checked;
  DEBTRACE("Scene::_addRowCols=" << checked);
}

void GenericGui::onSelectReference()
{
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  if (!sub) return;
  SubjectReference *ref = dynamic_cast<SubjectReference*>(sub);
  YASSERT(ref);
  SubjectServiceNode *snode = dynamic_cast<SubjectServiceNode*>(ref->getReference());
  snode->select(true);
}

void GenericGui::onWhatsThis()
{
  DEBTRACE("GenericGui::onWhatsThis");
  QWhatsThis::enterWhatsThisMode();
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

void GenericGui::onToggleStopOnError(bool checked)
{
  DEBTRACE("GenericGui::onToggleStopOnError " << checked);
  if (!QtGuiContext::getQtCurrent()) return;
  if (!QtGuiContext::getQtCurrent()->getGuiExecutor()) return;
}

void GenericGui::onToggleSceneItemVisible(bool checked)
{
  DEBTRACE("GenericGui::onToggleSceneItemVisible " << checked);
  if (!QtGuiContext::getQtCurrent()) return;
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  SubjectNode *snode = dynamic_cast<SubjectNode*>(sub);
  if (!snode) return;
  DEBTRACE("---");
  if (!QtGuiContext::getQtCurrent()->_mapOfSceneItem.count(sub)) return;
  DEBTRACE("---");
  SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sub];
  item->setVisible(checked);
}

void GenericGui::displayLinks(bool isShown)
{
  if (!QtGuiContext::getQtCurrent()) return;
  map<pair<YACS::ENGINE::OutPort*, YACS::ENGINE::InPort*>,YACS::HMI::SubjectLink*>::const_iterator it;
  for (it = QtGuiContext::getQtCurrent()->_mapOfSubjectLink.begin();
       it != QtGuiContext::getQtCurrent()->_mapOfSubjectLink.end();
       ++it)
    {
      YACS::HMI::SubjectLink* sub = (*it).second;
      if (!sub) continue;
      SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sub];
      item->setVisible(isShown);
    }
}

void GenericGui::displayControlLinks(bool isShown)
{
  if (!QtGuiContext::getQtCurrent()) return;
  map<pair<YACS::ENGINE::Node*, YACS::ENGINE::Node*>,YACS::HMI::SubjectControlLink*>::const_iterator it;
  for (it = QtGuiContext::getQtCurrent()->_mapOfSubjectControlLink.begin();
       it != QtGuiContext::getQtCurrent()->_mapOfSubjectControlLink.end();
       ++it)
    {
      YACS::HMI::SubjectControlLink* sub = (*it).second;
      if (!sub) continue;
      SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sub];
      item->setVisible(isShown);
    }
}

void GenericGui::displayPortLinks(bool isShown)
{
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  DEBTRACE("displayPortLinks, subject : " << sub->getName());
  SubjectDataPort *sport = dynamic_cast<SubjectDataPort*>(sub);
  if (sport)
    {
      DEBTRACE("dataPort : " << sport->getName());
      list<SubjectLink*> linkList = sport->getListOfSubjectLink();
      list<SubjectLink*>::const_iterator it = linkList.begin();
      for( ; it != linkList.end(); ++it)
        {
          YACS::HMI::SubjectLink* sub = (*it);
          if (!sub) continue;
          SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sub];
          item->setVisible(isShown);
        }
      return;
    }
  SubjectNode *snode = dynamic_cast<SubjectNode*>(sub);
  if (snode)
    {
      DEBTRACE("Node : " << snode->getName());
      list<SubjectControlLink*> linkList = snode->getSubjectControlLinks();
      list<SubjectControlLink*>::const_iterator it = linkList.begin();
      for( ; it != linkList.end(); ++it)
        {
          YACS::HMI::SubjectControlLink* sub = (*it);
          if (!sub) continue;
          SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sub];
          item->setVisible(isShown);
        }
      return;
    }
}

void GenericGui::displayALink(bool isShown)
{
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  if (! QtGuiContext::getQtCurrent()->_mapOfSceneItem.count(sub)) return;
  SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sub];
  item->setVisible(isShown);
}

void GenericGui::onShowAllLinks()
{
  DEBTRACE("GenericGui::onShowAllLinks");
  displayLinks(true);
  displayControlLinks(true);
}

void GenericGui::onHideAllLinks()
{
  DEBTRACE("GenericGui::onHideAllLinks");
  displayLinks(false);
  displayControlLinks(false);
}

void GenericGui::onShowOnlyPortLinks()
{
  DEBTRACE("GenericGui::onShowOnlyPortLinks");
  onHideAllLinks();
  displayPortLinks(true);
}

void GenericGui::onShowPortLinks()
{
  DEBTRACE("GenericGui::onShowPortLinks");
  displayPortLinks(true);
}

void GenericGui::onHidePortLinks()
{
  DEBTRACE("GenericGui::onHidePortLinks");
  displayPortLinks(false);
}

void GenericGui::onEmphasisPortLinks()
{
  DEBTRACE("GenericGui::onEmphasisPortLinks");
  if (!QtGuiContext::getQtCurrent()) return;
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  DEBTRACE("EmphasizePortLinks, subject : " << sub->getName());
  if (!sub)
    return;

  SubjectDataPort *sport = dynamic_cast<SubjectDataPort*>(sub);
  if (sport)
    {
      emphasizePortLink(sport, true);
      return;
    }

  // --- if a Node, explore all data ports

  SubjectNode *snode = dynamic_cast<SubjectNode*>(sub);
  if (snode)
    {
      DEBTRACE("Node : " << snode->getName());
      {
        list<SubjectInputPort*> linkList = snode->getSubjectInputPorts();
        list<SubjectInputPort*>::const_iterator it = linkList.begin();
        for( ; it != linkList.end(); ++it)
          {
            YACS::HMI::SubjectInputPort* sub = (*it);
            if (!sub) continue;
            emphasizePortLink(sub, true);
          }
      }
      {
        list<SubjectOutputPort*> linkList = snode->getSubjectOutputPorts();
        list<SubjectOutputPort*>::const_iterator it = linkList.begin();
        for( ; it != linkList.end(); ++it)
          {
            YACS::HMI::SubjectOutputPort* sub = (*it);
            if (!sub) continue;
            emphasizePortLink(sub, true);
          }
      }
      return;
    }
}

void GenericGui::onShowOnlyCtrlLinks()
{
  DEBTRACE("GenericGui::onShowOnlyCtrlLinks");
  onHideAllLinks();
  displayPortLinks(true);
}

void GenericGui::onShowCtrlLinks()
{
  DEBTRACE("GenericGui::onShowCtrlLinks");
  displayPortLinks(true);
}

void GenericGui::onHideCtrlLinks()
{
  DEBTRACE("GenericGui::onHideCtrlLinks");
  displayPortLinks(false);
}

void GenericGui::onEmphasisCtrlLinks()
{
  DEBTRACE("GenericGui::onEmphasisCtrlLinks");
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  DEBTRACE("Emphasize Ctrl Links, subject : " << sub->getName());
  if (!sub)
    return;
  SubjectNode *snode = dynamic_cast<SubjectNode*>(sub);
  if (snode)
    {
      DEBTRACE("Node : " << snode->getName());
      list<SubjectControlLink*> linkList = snode->getSubjectControlLinks();
      list<SubjectControlLink*>::const_iterator it = linkList.begin();
      for( ; it != linkList.end(); ++it)
        {
          YACS::HMI::SubjectControlLink* sub = (*it);
          if (!sub) continue;
          sub->update(EMPHASIZE, true, sub);
          Subject *sin = sub->getSubjectInNode();
          Subject *sout = sub->getSubjectOutNode();
          sin->update(EMPHASIZE, true, sub);
          sout->update(EMPHASIZE, true, sub);
        }
      return;
    }
}

void GenericGui::onShowOnlyLink()
{
  DEBTRACE("GenericGui::onShowOnlyLink");
  onHideAllLinks();
  displayALink(true);
}

void GenericGui::onShowLink()
{
  DEBTRACE("GenericGui::onShowLink");
  displayALink(true);
}

void GenericGui::onHideLink()
{
  DEBTRACE("GenericGui::onHideLink");
  displayALink(false);
}

void GenericGui::onEmphasisLink()
{
  DEBTRACE("GenericGui::onEmphasisLink");
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  if (!sub)
    return;
  sub->update(EMPHASIZE, true, sub);
}

void GenericGui::onDeEmphasizeAll()
{
  DEBTRACE("GenericGui::onDeEmphasizeAll");
  map<Subject*, SchemaItem*>::const_iterator it = QtGuiContext::getQtCurrent()->_mapOfSchemaItem.begin();
  for( ; it != QtGuiContext::getQtCurrent()->_mapOfSchemaItem.end(); ++it)
    {
      Subject* sub = (*it).first;
      sub->update(EMPHASIZE, false, sub);
    }
}

void GenericGui::onUndo()
{
  DEBTRACE("GenericGui::onUndo");
  QtGuiContext::getQtCurrent()->getInvoc()->undo();
}

void GenericGui::onRedo()
{
  DEBTRACE("GenericGui::onRedo");
  QtGuiContext::getQtCurrent()->getInvoc()->redo();
}

void GenericGui::onCleanOnExit()
{
  DEBTRACE("GenericGui::onCleanOnExit");
  int studyId = _wrapper->activeStudyId();
  map<QWidget*, YACS::HMI::QtGuiContext*> mapViewContextCopy = _mapViewContext;
  map<QWidget*, YACS::HMI::QtGuiContext*>::iterator it = mapViewContextCopy.begin();
  for (; it != mapViewContextCopy.end(); ++it)
    {
      closeContext((*it).first);
    }
}

void GenericGui::emphasizePortLink(YACS::HMI::SubjectDataPort* sub, bool emphasize)
{
  DEBTRACE("dataPort : " << sub->getName());
  list<SubjectLink*> linkList = sub->getListOfSubjectLink();
  list<SubjectLink*>::const_iterator it = linkList.begin();
  for( ; it != linkList.end(); ++it)
    {
      YACS::HMI::SubjectLink* subli = (*it);
      if (!subli) continue;
      subli->update(EMPHASIZE, true, sub);
      Subject *sin = subli->getSubjectInPort();
      Subject *sout = subli->getSubjectOutPort();
      sin->update(EMPHASIZE, true, sub);
      sout->update(EMPHASIZE, true, sub);
    }
}
