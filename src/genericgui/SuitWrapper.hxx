
#ifndef _SUITWRAPPER_HXX_
#define _SUITWRAPPER_HXX_

#include <QAction>
#include <QString>
#include <QIcon>
#include <QObject>
#include <QWidget>
#include <QGraphicsScene>

namespace YACS
{
  namespace HMI
  {
    //! viewer QGraphicsScene: deriver SUIT_ViewModel comme QxGraph_Viewer
    //                         deriver idem SUIT_ViewManager et SUIT_ViewWindows

    //! wrapping for SalomeApp_Module

    class SuitWrapper
    {
      
    public:
      SuitWrapper(QObject* wrapped);
      ~SuitWrapper();

      QWidget* getNewWindow(QGraphicsScene *scene);
      int AssociateViewToWindow(QGraphicsView* gView, QWidget* viewWindow);
      int activeStudyId();

      QAction* createAction(const int id,
                            const QString& toolTip,
                            const QIcon& icon,
                            const QString& menu,
                            const QString& status,
                            const int shortCut,
                            QObject* parent =0,
                            bool checkable = false,
                            QObject* receiver =0,
                            const char* member =0);
      
      int createMenu(const QString& subMenu,
                     const int parentMenuId,
                     const int menuId = -1,
                     const int groupId = -1,
                     const int index = -1);

      int createMenu(const QString& subMenu,
                     const QString& parentMenu,
                     const int menuId = -1,
                     const int groupId = -1,
                     const int index = -1);

      int createMenu(const int actionId,
                     const int menuId,
                     const int groupId = -1,
                     const int index = -1);

      int createMenu(const int actionId,
                     const QString& menu,
                     const int groupId = -1,
                     const int index = -1);

      int createMenu(QAction* action,
                     const int menuId,
                     const int actionId = -1,
                     const int groupId = -1,
                     const int index = -1);

      int createMenu(QAction* action,
                     const QString& menu,
                     const int actionId = -1,
                     const int groupId = -1,
                     const int index = -1);

      int createTool(const QString& name);

      int createTool(const int actionId,
                     const int toolbarId, 
                     const int index = -1);

      int createTool(const int actionId,
                     const QString& toolbar,
                     const int index = -1);

      int createTool(QAction* action, 
                     const int toolbarId,
                     const int actionId = -1,
                     const int index = -1);

      int createTool(QAction* action,
                     const QString& toolbar,
                     const int actionId = -1,
                     const int index = -1);

      static QAction* separator();

      void setMenuShown(QAction* act, bool show);
      void setToolShown(QAction* act, bool show);

      void createNewSchema(const QString& schemaName,
                           QWidget* viewWindow);
      bool renameSchema(const QString& oldName,
                        const QString& newName,
                        QWidget* viewWindow);
      void createNewRun(const QString& schemaName,
                        const QString& runName,
                        QWidget* refWindow,
                        QWidget* viewWindow);

    protected: 
      QObject* _wrapped; // SalomeApp_module or Standalone Application

    };
  }
}
#endif
