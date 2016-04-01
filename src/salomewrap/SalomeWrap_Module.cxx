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

#include "SalomeWrap_Module.hxx"
#include "SalomeWrap_DataModel.hxx"

#include <SalomeApp_Application.h>
#include <QxScene_ViewManager.h>
#include <QxScene_ViewModel.h>
#include <QxScene_ViewWindow.h>
#include <CAM_DataModel.h>
#include <SUIT_Study.h>

#include <SUIT_DataBrowser.h>
#include <QtxTreeView.h>
#include <SUIT_DataObject.h>

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

SalomeWrap_Module::SalomeWrap_Module(const char* name) :
  SalomeApp_Module( name )
{
  _mapOfViewWindow.clear();
}

/*! create a new QxScene_ViewWindow unless the active view is an empty
 *  QxScene_ViewWindow automatically created when SALOME module is loaded.
 */
QxScene_ViewWindow* SalomeWrap_Module::getNewWindow(QGraphicsScene *scene)
{
  SUIT_ViewManager *svm = 0;
  if (_mapOfViewWindow.empty()) // --- reuse already created view manager 
    svm = getApp()->getViewManager(QxScene_Viewer::Type(), true);
  else
    svm = getApp()->createViewManager(QxScene_Viewer::Type());
  SUIT_ViewWindow* svw = svm->getActiveView();
  QxScene_ViewWindow *aView = 0;
  QGraphicsScene* existingScene = 0;
  if (!svw) svw = svm->createViewWindow();
  if (svw) aView = dynamic_cast<QxScene_ViewWindow*>(svw);
  if (aView) existingScene = aView->getScene();
  if (existingScene)
    {
      svw = svm->createViewWindow();
      if (svw) aView = dynamic_cast<QxScene_ViewWindow*>(svw);
    }
  YASSERT(aView);
  aView->setScene(scene);
  _mapOfViewWindow[scene] = aView;
  return aView;
}

int SalomeWrap_Module::AssociateViewToWindow(QGraphicsView* gView,
                                             QxScene_ViewWindow* viewWindow)
{
  viewWindow->setSceneView(gView);
  viewWindow->setCentralWidget(gView);
  return viewWindow->getViewManager()->study()->id();
}

int SalomeWrap_Module::activeStudyId()
{
  return getApp()->activeStudy()->id();
}

QDockWidget* SalomeWrap_Module::objectBrowser() {
  if ( !getApp()->objectBrowser() )
    return 0;

  QWidget* wid = getApp()->objectBrowser()->treeView();

  if ( !wid ) {
    return 0;
  };

  QDockWidget* dock = 0;
  QWidget* w = wid->parentWidget();
  while ( w && !dock ) {
    dock = ::qobject_cast<QDockWidget*>( w );
    w = w->parentWidget();
  };

  return dock;
}


QAction* SalomeWrap_Module::wCreateAction(const int id,
                                          const QString& toolTip,
                                          const QIcon& icon,
                                          const QString& menu,
                                          const QString& status,
                                          const int shortCut,
                                          QObject* parent,
                                          bool checkable,
                                          QObject* receiver,
                                          const char* member)
{
  return createAction(id, toolTip, icon, menu, status, shortCut,
                      parent, checkable, receiver, member);
}
      
int SalomeWrap_Module::wCreateMenu(const QString& subMenu,
                                   const int parentMenuId,
                                   const int menuId,
                                   const int groupId,
                                   const int index)
{
  return createMenu(subMenu, parentMenuId, menuId, groupId, index);
}

int SalomeWrap_Module::wCreateMenu(const QString& subMenu,
                                   const QString& parentMenu,
                                   const int menuId,
                                   const int groupId,
                                   const int index)
{
  return createMenu(subMenu, parentMenu, menuId, groupId, index);
}

int SalomeWrap_Module::wCreateMenu(const int actionId,
                                   const int menuId,
                                   const int groupId,
                                   const int index)
{
  return createMenu(actionId, menuId, groupId, index);
}

int SalomeWrap_Module:: wCreateMenu(const int actionId,
                                    const QString& menu,
                                    const int groupId,
                                    const int index)
{
  return createMenu(actionId, menu, groupId, index);
}

int SalomeWrap_Module::wCreateMenu(QAction* action,
                                   const int menuId,
                                   const int actionId,
                                   const int groupId,
                                   const int index)
{
  return createMenu(action, menuId, actionId, groupId, index);
}

int SalomeWrap_Module::wCreateMenu(QAction* action,
                                   const QString& menu,
                                   const int actionId,
                                   const int groupId,
                                   const int index)
{
  return createMenu(action, menu, actionId, groupId, index);
}

int SalomeWrap_Module::wCreateTool(const QString& title, const QString& name)
{
  return createTool(title, name);
}

int SalomeWrap_Module::wCreateTool(const int actionId,
                                   const int toolbarId, 
                                   const int index)
{
  return createTool(actionId, toolbarId, index);
}

int SalomeWrap_Module::wCreateTool(const int actionId,
                                   const QString& toolbar,
                                   const int index)
{
  return createTool(actionId, toolbar, index);
}

int SalomeWrap_Module::wCreateTool(QAction* action, 
                                   const int toolbarId,
                                   const int actionId,
                                   const int index)
{
  return createTool(action, toolbarId, actionId, index);
}

int SalomeWrap_Module::wCreateTool(QAction* action,
                                   const QString& toolbar,
                                   const int actionId,
                                   const int index)
{
  return createTool(action, toolbar, actionId, index);
}

QAction* SalomeWrap_Module::wSeparator()
{
  return separator();
}

SalomeWrap_DataModel* SalomeWrap_Module::getDataModel()
{
  return dynamic_cast<SalomeWrap_DataModel*>(dataModel());
}

CAM_DataModel* SalomeWrap_Module::createDataModel()
{
  return new SalomeWrap_DataModel(this);
}
