// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef _GENERICGUI_HXX_
#define _GENERICGUI_HXX_

#include "GenericGuiExport.hxx"

#include <QAction>
#include <QActionGroup>
#include <QObject>
#include <QMainWindow>
#include <QDockWidget>
#include <QString>

#include "SuitWrapper.hxx"

#include <map>
#include <list>
#include <string>

namespace YACS
{

  namespace ENGINE
  {
    class Proc;
    class Catalog;
  }
  namespace HMI
  {
    class QtGuiContext;
    class GuiEditor;
    class SuitWrapper;
    class CatalogWidget;
    class YACSGuiLoader;
    class SubjectDataPort;
    class BatchJobsListDialog;

    class GENERICGUI_EXPORT GenericGui: public QObject
    {
      Q_OBJECT

    public:
      GenericGui(YACS::HMI::SuitWrapper* wrapper, QMainWindow *parent = 0);
      virtual ~GenericGui();

      void createActions();
      void createMenus();
      void createTools();
      void initialMenus();
      void hideAllMenus();
      void showBaseMenus(bool show);
      void showEditionMenus(bool show);
      void showExecMenus(bool show);
      void showCommonMenus(bool show);
      void switchContext(QWidget *view, bool onExit = false);
      bool closeContext(QWidget *view, bool onExit = false);
      void showDockWidgets(bool isVisible);
      void raiseStacked();
      CatalogWidget* getCatalogWidget() { return _catalogsWidget; };
      std::list<std::string> getMachineList();
      void createForEachLoop(std::string type="double");
      void putGraphInForeachLoop(std::string type);
      virtual void loadSchema(const std::string& filename,bool edit=true, bool arrangeLocalNodes=false);
      virtual void onHelpContextModule( const QString&, const QString&, const QString& = QString() );
      void createContext(YACS::ENGINE::Proc* proc,
                         const QString& schemaName,
                         const QString& runName,
                         bool forEdition);
      YACSGuiLoader *getLoader() { return _loader; };

      QAction *_newSchemaAct;
      QAction *_importSchemaAct;
      QAction *_chooseBatchJobAct;
      QAction *_importSupervSchemaAct;
      QAction *_exportSchemaAct;
      QAction *_exportSchemaAsAct;
      QAction *_importCatalogAct;

      QAction *_runLoadedSchemaAct;
      QAction *_loadRunStateSchemaAct;
      QAction *_loadAndRunSchemaAct;

      QAction *_startResumeAct;
      QAction *_abortAct;
      QAction *_pauseAct;
      QAction *_resetAct;

      QAction *_saveRunStateAct;
      QAction *_newEditionAct;

      QActionGroup *_execModeGroup;
      QAction *_withoutStopModeAct;
      QAction *_breakpointsModeAct;
      QAction *_stepByStepModeAct;

      QAction *_toggleStopOnErrorAct;

      QAction *_getYacsContainerLogAct;
      QAction *_getErrorReportAct;
      QAction *_getErrorDetailsAct;
      QAction *_getContainerLogAct;
      QAction *_shutdownProcAct;

      QAction *_editDataTypesAct;
      QAction *_createDataTypeAct;
      QAction *_importDataTypeAct;
      QAction *_newContainerAct;
      QAction *_newHPContainerAct;
      QAction *_selectComponentInstanceAct;
      QAction *_newSalomeComponentAct;
      QAction *_newSalomePythonComponentAct;
      QAction *_newCorbaComponentAct;
      QAction *_salomeServiceNodeAct;
      QAction *_serviceInlineNodeAct;
      QAction *_CORBAServiceNodeAct;
      QAction *_nodeNodeServiceNodeAct;
      QAction *_cppNodeAct;
      QAction *_XMLNodeAct;
      QAction *_inDataNodeAct;
      QAction *_outDataNodeAct;
      QAction *_inStudyNodeAct;
      QAction *_outStudyNodeAct;
      QAction *_inlineScriptNodeAct;
      QAction *_inlineFunctionNodeAct;
      QAction *_blockNodeAct;
      QAction *_FORNodeAct;
      QAction *_FOREACHNodeAct;
      QAction *_WHILENodeAct;
      QAction *_SWITCHNodeAct;
      QAction *_OptimizerLoopAct;
      QAction *_nodeFromCatalogAct;
      QAction *_deleteItemAct;
      QAction *_cutItemAct;
      QAction *_copyItemAct;
      QAction *_pasteItemAct;
      QAction *_putInBlocAct;
      QAction *_putGraphInBlocAct;
      QAction *_putGraphInForLoopAct;
      QAction *_putGraphInWhileLoopAct;
      QAction *_putGraphInOptimizerLoopAct;
      QAction *_arrangeLocalNodesAct;
      QAction *_arrangeRecurseNodesAct;
      QAction *_computeLinkAct;
      QAction *_zoomToBlocAct;
      QAction *_centerOnNodeAct;
      QAction *_shrinkExpand;
      QAction *_shrinkExpandChildren;
      QAction *_shrinkExpandElementaryRecursively;

      QAction *_toggleStraightLinksAct;
      QAction *_toggleAutomaticComputeLinkAct;
      QAction *_toggleSimplifyLinkAct;
      QAction *_toggleForce2NodesLinkAct;
      QAction *_toggleAddRowColsAct;
      QAction *_toggleSceneItemVisibleAct;
      QAction *_selectReferenceAct;
      QAction *_whatsThisAct;

      QAction *_showAllLinksAct;
      QAction *_hideAllLinksAct;

      QAction *_showOnlyPortLinksAct;
      QAction *_showPortLinksAct;
      QAction *_hidePortLinksAct;
      QAction *_emphasisPortLinksAct;

      QAction *_showOnlyCtrlLinksAct;
      QAction *_showCtrlLinksAct;
      QAction *_hideCtrlLinksAct;
      QAction *_emphasisCtrlLinksAct;

      QAction *_showOnlyLinkAct;
      QAction *_showLinkAct;
      QAction *_hideLinkAct;
      QAction *_emphasisLinkAct;
      QAction *_deEmphasizeAllAct;

      QAction *_undoAct;
      QAction *_redoAct;
      QAction *_showUndoAct;
      QAction *_showRedoAct;

      YACS::HMI::GuiEditor *_guiEditor;
      void setLoadedPresentation(YACS::ENGINE::Proc* proc);

    public slots:
      void onCleanOnExit();

    protected:
      int getMenuId() { return _menuId++; }
      QString getSaveFileName(const QString& fileName = QString());

      YACSGuiLoader *_loader;
      SuitWrapper* _wrapper;
      QMainWindow* _parent;
      QDockWidget* _dwTree;
      QDockWidget* _dwStacked;
      QDockWidget* _dwCatalogs;
      BatchJobsListDialog* _BJLdialog;
      YACS::ENGINE::Catalog* _builtinCatalog;
      YACS::ENGINE::Catalog* _sessionCatalog;
      CatalogWidget* _catalogsWidget;
      std::map<QWidget*, YACS::HMI::QtGuiContext*> _mapViewContext;
      int _menuId;
      int _toolId;
      int _schemaCnt;
      bool _isSaved;
      std::list<std::string> _machineList;

    private slots:

      void onNewSchema();
      void onImportSchema();
      void onChooseBatchJob();
      void onImportSupervSchema();
      void onExportSchema();
      void onExportSchemaAs();
      void onImportCatalog();

      void onRunLoadedSchema(bool withState = false);
      void onLoadRunStateSchema();
      void onLoadAndRunSchema();

      void onStartResume();
      void onAbort();
      void onPause();
      void onReset();
      void onSaveRunState();
      void onNewEdition();

      void onWithoutStopMode(bool checked);
      void onBreakpointsMode(bool checked);
      void onStepByStepMode(bool checked);

      void onToggleStopOnError(bool checked);

      void onGetYacsContainerLog();
      void onGetErrorReport();
      void onGetErrorDetails();
      void onGetContainerLog();
      void onShutdownProc();

      void onEditDataTypes();
      void onCreateDataType();
      void onImportDataType();

      void onNewContainer();
      void onNewHPContainer();
      void onSelectComponentInstance();
      void onNewSalomeComponent();
      void onNewSalomePythonComponent();
      void onNewCorbaComponent();

      void onSalomeServiceNode();
      void onServiceInlineNode();
      void onCORBAServiceNode();
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
      void onOptimizerLoop();
      void onNodeFromCatalog();

      void onDeleteItem();
      void onCutItem();
      void onCopyItem();
      void onPasteItem();
      void onPutInBloc();
      void onPutGraphInBloc();
      void onPutGraphInForLoop();
      void onPutGraphInWhileLoop();
      void onPutGraphInOptimizerLoop();
      void onArrangeLocalNodes();
      void onArrangeRecurseNodes();
      void onRebuildLinks();
      void onZoomToBloc();
      void onCenterOnNode();
      void onShrinkExpand();
      void onShrinkExpandChildren();
      void onShrinkExpandElementaryRecursively();
      void onToggleStraightLinks(bool checked);
      void onToggleAutomaticComputeLinks(bool checked);
      void onToggleSimplifyLinks(bool checked);
      void onToggleForce2NodesLinks(bool checked);
      void onToggleAddRowCols(bool checked);

      void onToggleSceneItemVisible(bool checked);

      void onShowAllLinks();
      void onHideAllLinks();

      void onShowOnlyPortLinks();
      void onShowPortLinks();
      void onHidePortLinks();
      void onEmphasisPortLinks();

      void onShowOnlyCtrlLinks();
      void onShowCtrlLinks();
      void onHideCtrlLinks();
      void onEmphasisCtrlLinks();

      void onShowOnlyLink();
      void onShowLink();
      void onHideLink();
      void onEmphasisLink();
      void onDeEmphasizeAll();

      void onSelectReference();
      void onWhatsThis();

      void onUndo();
      void onRedo();
      void onShowUndo();
      void onShowRedo();

    private:
      void displayLinks(bool isShown);
      void displayControlLinks(bool isShown);
      void displayPortLinks(bool isShown);
      void displayALink(bool isShown);
      void emphasizePortLink(YACS::HMI::SubjectDataPort* sub, bool emphasize);
    };

  }
}

#endif
