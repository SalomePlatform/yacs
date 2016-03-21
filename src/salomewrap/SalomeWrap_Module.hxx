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

#ifndef _SALOMEWRAP_MODULE_HXX_
#define _SALOMEWRAP_MODULE_HXX_

#include "SalomeWrapExport.hxx"

#include <SalomeApp_Module.h>
#include <SALOMEconfig.h>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDockWidget>

#include <map>

class SalomeApp_Application;
class QxScene_ViewWindow;

class SalomeWrap_DataModel;

class SALOMEWRAP_EXPORT SalomeWrap_Module: public SalomeApp_Module
{
  Q_OBJECT

public:
  SalomeWrap_Module(const char* name);

  QxScene_ViewWindow* getNewWindow(QGraphicsScene *scene);

  int AssociateViewToWindow(QGraphicsView* gView,
                            QxScene_ViewWindow* viewWindow);

  int activeStudyId();

  QDockWidget* objectBrowser();

  QAction* wCreateAction(const int id,
                         const QString& toolTip,
                         const QIcon& icon,
                         const QString& menu,
                         const QString& status,
                         const int shortCut,
                         QObject* parent =0,
                         bool checkable = false,
                         QObject* receiver =0,
                         const char* member =0);
      
  int wCreateMenu(const QString& subMenu,
                  const int parentMenuId,
                  const int menuId = -1,
                  const int groupId = -1,
                  const int index = -1);

  int wCreateMenu(const QString& subMenu,
                  const QString& parentMenu,
                  const int menuId = -1,
                  const int groupId = -1,
                  const int index = -1);

  int wCreateMenu(const int actionId,
                  const int menuId,
                  const int groupId = -1,
                  const int index = -1);

  int wCreateMenu(const int actionId,
                  const QString& menu,
                  const int groupId = -1,
                  const int index = -1);

  int wCreateMenu(QAction* action,
                  const int menuId,
                  const int actionId = -1,
                  const int groupId = -1,
                  const int index = -1);

  int wCreateMenu(QAction* action,
                  const QString& menu,
                  const int actionId = -1,
                  const int groupId = -1,
                  const int index = -1);

  int wCreateTool(const QString& title,
		  const QString& name = QString());

  int wCreateTool(const int actionId,
                  const int toolbarId, 
                  const int index = -1);

  int wCreateTool(const int actionId,
                  const QString& toolbar,
                  const int index = -1);

  int wCreateTool(QAction* action, 
                  const int toolbarId,
                  const int actionId = -1,
                  const int index = -1);

  int wCreateTool(QAction* action,
                  const QString& toolbar,
                  const int actionId = -1,
                  const int index = -1);

  static QAction* wSeparator();

  SalomeWrap_DataModel* getDataModel();

protected:
  virtual  CAM_DataModel* createDataModel();

  std::map<QGraphicsScene*, QxScene_ViewWindow*> _mapOfViewWindow;

};

#endif
