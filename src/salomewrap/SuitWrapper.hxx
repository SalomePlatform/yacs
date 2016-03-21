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

#ifndef _SUITWRAPPER_HXX_
#define _SUITWRAPPER_HXX_

#include "SalomeWrapExport.hxx"

#include <QAction>
#include <QString>
#include <QIcon>
#include <QObject>
#include <QWidget>
#include <QDockWidget>
#include <QGraphicsScene>

namespace YACS
{
  namespace HMI
  {
    //! viewer QGraphicsScene: deriver SUIT_ViewModel comme QxGraph_Viewer
    //                         deriver idem SUIT_ViewManager et SUIT_ViewWindows

    //! wrapping for SalomeApp_Module

    class SALOMEWRAP_EXPORT SuitWrapper
    {
      
    public:
      SuitWrapper(QObject* wrapped);
      virtual ~SuitWrapper();

      QWidget* getNewWindow(QGraphicsScene *scene);
      int AssociateViewToWindow(QGraphicsView* gView, QWidget* viewWindow);
      int activeStudyId();
      QDockWidget* objectBrowser();

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

      int createTool(const QString& title,
		     const QString& name = QString());

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
      bool deleteSchema(QWidget* viewWindow);
      void createNewRun(const QString& schemaName,
                        const QString& runName,
                        QWidget* refWindow,
                        QWidget* viewWindow);
      QStringList getQuickDirList();
      virtual void onHelpContextModule( const QString&, const QString&, const QString& = QString() );

    protected: 
      QObject* _wrapped; // SalomeApp_module or Standalone Application

    };
  }
}
#endif
