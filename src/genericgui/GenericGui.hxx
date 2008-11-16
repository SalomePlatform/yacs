#ifndef _GENERICGUI_HXX_
#define _GENERICGUI_HXX_

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
  class YACSLoader;

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

    class GenericGui: public QObject
    {
      Q_OBJECT

    public:
      GenericGui(YACS::HMI::SuitWrapper* wrapper, QMainWindow *parent = 0);
      virtual ~GenericGui();

      void createActions();
      void createMenus();
      void createTools();
      void initialMenus();
      void showBaseMenus(bool show);
      void showEditionMenus(bool show);
      void showExecMenus(bool show);
      void switchContext(QWidget *view);
      void showDockWidgets(bool isVisible);
      CatalogWidget* getCatalogWidget() { return _catalogsWidget; };
      std::list<std::string> getMachineList();

      QAction *_newSchemaAct;
      QAction *_importSchemaAct;
      QAction *_importSupervSchemaAct;
      QAction *_exportSchemaAct;
      QAction *_exportSchemaAsAct;
      QAction *_importCatalogAct;

      QAction *_runLoadedSchemaAct;
      QAction *_loadAndRunSchemaAct;

      QAction *_executeAct;
      QAction *_abortAct;
      QAction *_suspendResumeAct;
      QAction *_pauseAct;
      QAction *_resumeAct;

      QActionGroup *_execModeGroup;
      QAction *_withoutStopModeAct;
      QAction *_breakpointsModeAct;
      QAction *_stepByStepModeAct;

      QAction *_editDataTypesAct;
      QAction *_createDataTypeAct;
      QAction *_importDataTypeAct;
      QAction *_newContainerAct;
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
      QAction *_nodeFromCatalogAct;
      QAction *_deleteItemAct;
      QAction *_computeLinkAct;
      QAction *_toggleAutomaticComputeLinkAct;
      QAction *_toggleSimplifyLinkAct;
      QAction *_toggleForce2NodesLinkAct;
      QAction *_selectReferenceAct;

      YACS::HMI::GuiEditor *_guiEditor;

    public slots:
      void onCleanOnExit();

    protected:
      int getMenuId() { return _menuId++; }
      void createContext(YACS::ENGINE::Proc* proc,
                         const QString& schemaName,
                         const QString& runName,
                         bool forEdition);
      QString getSaveFileName(const QString& fileName = QString());

      YACS::YACSLoader *_loader;
      SuitWrapper* _wrapper;
      QMainWindow* _parent;
      QDockWidget* _dwTree;
      QDockWidget* _dwStacked;
      QDockWidget* _dwCatalogs;
      YACS::ENGINE::Catalog* _builtinCatalog;
      YACS::ENGINE::Catalog* _sessionCatalog;
      CatalogWidget* _catalogsWidget;
      std::map<QWidget*, YACS::HMI::QtGuiContext*> _mapViewContext;
      int _menuId;
      int _toolId;
      int _schemaCnt;
      std::list<std::string> _machineList;

    private slots:

      void onNewSchema();
      void onImportSchema();
      void onImportSupervSchema();
      void onExportSchema();
      void onExportSchemaAs();
      void onImportCatalog();

      void onRunLoadedSchema();
      void onLoadAndRunSchema();

      void onExecute();
      void onAbort();
      void onSuspendResume();
      void onPause();
      void onResume();

      void onWithoutStopMode(bool checked);
      void onBreakpointsMode(bool checked);
      void onStepByStepMode(bool checked);
  
      void onEditDataTypes();
      void onCreateDataType();
      void onImportDataType();

      void onNewContainer();
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
      void onNodeFromCatalog();

      void onDeleteItem();

      void onRebuildLinks();
      void onToggleAutomaticComputeLinks(bool checked);
      void onToggleSimplifyLinks(bool checked);
      void onToggleForce2NodesLinks(bool checked);

      void onSelectReference();

      //   void onReloadDataflow();
      //   void onRunDataflow();
      //   void onKillDataflow();
      //   void onSuspendResumeDataflow();
      //   void onFilterNextSteps();
      //   void onSaveDataflowState();
      //   void onCreateExecution();
      //   void onCreateBatchExecution();
      //   void onReloadExecution();
      //   void onStartResume();
      //   void onExecuteStBy();
      //   void onExecuteInBatch();
      //   void onReset();
      //   void onSaveExecutionState();
      //   void onCreateEdition();
      //   void onConnectToRunning();
      //   void onToggleStopOnError();
  
      //   void onChangeInformations();

      //   void onTableView();

      //   void onDblClick( QListViewItem* );
      //   void onExpanded( QListViewItem* theItem );
      //   void onCollapsed( QListViewItem* theItem );

      //   void onApplyInputPanel( const int theId );

      //   void onSetActive();
  
      //   void onWindowActivated( SUIT_ViewWindow* );
      //   void onWindowClosed( SUIT_ViewWindow* );

    private:
    };

  }
}

#endif
