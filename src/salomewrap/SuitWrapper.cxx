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

/*!
 *  only one declaration in src/genericgui,
 *  implementations in salomewrap and standalonegui
 */
    
#include "SuitWrapper.hxx"
#include "WrapGraphicsView.hxx"

#include "SalomeWrap_Module.hxx"
#include "SalomeWrap_DataModel.hxx"

#include <SalomeApp_Module.h>
#include <SalomeApp_Application.h>
#include "SUIT_Session.h"
#include "SUIT_ResourceMgr.h"

#include <QxScene_ViewManager.h>
#include <QxScene_ViewModel.h>
#include <QxScene_ViewWindow.h>
#include <QtxActionToolMgr.h>

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

SuitWrapper::SuitWrapper(QObject* wrapped) : _wrapped(wrapped)
{
}

SuitWrapper::~SuitWrapper()
{
}

QWidget* SuitWrapper::getNewWindow(QGraphicsScene *scene)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  return module->getNewWindow(scene);
}

/*!
 * return studyId used in context delete when study is closed.
 */
int SuitWrapper::AssociateViewToWindow(QGraphicsView* gView, QWidget* viewWindow)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  QxScene_ViewWindow *svw = dynamic_cast<QxScene_ViewWindow*>(viewWindow);
  YASSERT(svw);
  int studyId = module->AssociateViewToWindow(gView, svw);
  WrapGraphicsView* wgv = dynamic_cast<WrapGraphicsView*>(gView);
  YASSERT(wgv);
  QObject::disconnect(svw->toolMgr()->action(QxScene_ViewWindow::FitAllId),
                      SIGNAL(triggered(bool)),
                      svw, SLOT(onViewFitAll()));
  QObject::connect(svw->toolMgr()->action(QxScene_ViewWindow::FitAllId),
                   SIGNAL(triggered(bool)),
                   wgv, SLOT(onViewFitAll()));

  QObject::disconnect(svw->toolMgr()->action(QxScene_ViewWindow::FitRectId),
                      SIGNAL(triggered(bool)),
                      svw, SLOT(onViewFitArea()));
  QObject::connect(svw->toolMgr()->action(QxScene_ViewWindow::FitRectId),
                   SIGNAL(triggered(bool)),
                   wgv, SLOT(onViewFitArea()));

  QObject::disconnect(svw->toolMgr()->action(QxScene_ViewWindow::ZoomId),
                      SIGNAL(triggered(bool)),
                      svw, SLOT(onViewZoom()));
  QObject::connect(svw->toolMgr()->action(QxScene_ViewWindow::ZoomId),
                   SIGNAL(triggered(bool)),
                   wgv, SLOT(onViewZoom()));

  QObject::disconnect(svw->toolMgr()->action(QxScene_ViewWindow::PanId),
                      SIGNAL(triggered(bool)),
                      svw, SLOT(onViewPan()));
  QObject::connect(svw->toolMgr()->action(QxScene_ViewWindow::PanId),
                   SIGNAL(triggered(bool)),
                   wgv, SLOT(onViewPan()));

  QObject::disconnect(svw->toolMgr()->action(QxScene_ViewWindow::GlobalPanId),
                      SIGNAL(triggered(bool)),
                      svw, SLOT(onViewGlobalPan()));
  QObject::connect(svw->toolMgr()->action(QxScene_ViewWindow::GlobalPanId),
                   SIGNAL(triggered(bool)),
                   wgv, SLOT(onViewGlobalPan()));

  QObject::disconnect(svw->toolMgr()->action(QxScene_ViewWindow::ResetId),
                      SIGNAL(triggered(bool)),
                      svw, SLOT(onViewReset()));
  QObject::connect(svw->toolMgr()->action(QxScene_ViewWindow::ResetId),
                   SIGNAL(triggered(bool)),
                   wgv, SLOT(onViewReset()));
  return studyId;
}

int SuitWrapper::activeStudyId()
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  return module->activeStudyId();
}

QDockWidget* SuitWrapper::objectBrowser()
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  return module->objectBrowser();
}

QAction* SuitWrapper::createAction(const int id,
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
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  return module->wCreateAction(id, toolTip, icon, menu, status, shortCut,
                               parent, checkable, receiver, member);
}
      
int SuitWrapper::createMenu(const QString& subMenu,
                             const int parentMenuId,
                             const int menuId,
                             const int groupId,
                             const int index)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  return module->wCreateMenu(subMenu, parentMenuId, menuId, groupId, index);
}

int SuitWrapper::createMenu(const QString& subMenu,
                             const QString& parentMenu,
                             const int menuId,
                             const int groupId,
                             const int index)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  return module->wCreateMenu(subMenu, parentMenu, menuId, groupId, index);
}

int SuitWrapper::createMenu(const int actionId,
                             const int menuId,
                             const int groupId,
                             const int index)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  return module->wCreateMenu(actionId, menuId, groupId, index);
}

int SuitWrapper:: createMenu(const int actionId,
                             const QString& menu,
                             const int groupId,
                             const int index)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  return module->wCreateMenu(actionId, menu, groupId, index);
}

int SuitWrapper::createMenu(QAction* action,
                             const int menuId,
                             const int actionId,
                             const int groupId,
                             const int index)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  return module->wCreateMenu(action, menuId, actionId, groupId, index);
}

int SuitWrapper::createMenu(QAction* action,
                             const QString& menu,
                             const int actionId,
                             const int groupId,
                             const int index)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  return module->wCreateMenu(action, menu, actionId, groupId, index);
}

int SuitWrapper::createTool(const QString& title, const QString& name)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  return module->wCreateTool(title, name);
}

int SuitWrapper::createTool(const int actionId,
                             const int toolbarId, 
                             const int index)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  return module->wCreateTool(actionId, toolbarId, index);
}

int SuitWrapper::createTool(const int actionId,
                             const QString& toolbar,
                             const int index)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  return module->wCreateTool(actionId, toolbar, index);
}

int SuitWrapper::createTool(QAction* action, 
                             const int toolbarId,
                             const int actionId,
                             const int index)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  return module->wCreateTool(action, toolbarId, actionId, index);
}

int SuitWrapper::createTool(QAction* action,
                             const QString& toolbar,
                             const int actionId,
                             const int index)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  return module->wCreateTool(action, toolbar, actionId, index);
}

QAction* SuitWrapper::separator()
{
  return SalomeWrap_Module::wSeparator();
}

void SuitWrapper::setMenuShown(QAction* act, bool show)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  module->setMenuShown(act, show);
}

void SuitWrapper::setToolShown(QAction* act, bool show)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  module->setToolShown(act, show);
}

void SuitWrapper::createNewSchema(const QString& schemaName,
                                  QWidget* viewWindow)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  module->getDataModel()->createNewSchema(schemaName, viewWindow);
}

bool SuitWrapper::renameSchema(const QString& oldName,
                               const QString& newName,
                               QWidget* viewWindow)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  return module->getDataModel()->renameSchema(oldName, newName, viewWindow);
}

bool SuitWrapper::deleteSchema(QWidget* viewWindow)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  return module->getDataModel()->deleteSchema(viewWindow);
}

void SuitWrapper::createNewRun(const QString& schemaName,
                               const QString& runName,
                               QWidget* refWindow,
                               QWidget* viewWindow)
{
  SalomeWrap_Module* module = dynamic_cast<SalomeWrap_Module*>(_wrapped);
  module->getDataModel()->createNewRun(schemaName, runName, refWindow, viewWindow);
}

QStringList SuitWrapper::getQuickDirList()
{
  QStringList dirList;
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  if ( resMgr )
    dirList = resMgr->stringValue( "FileDlg", "QuickDirList" ).split( ';', QString::SkipEmptyParts );
  return dirList;
}

void SuitWrapper::onHelpContextModule( const QString& theComponentName, const QString& theFileName, const QString& theContext )
{
  LightApp_Application* app = (LightApp_Application*)( SUIT_Session::session()->activeApplication() );
  if(app)
    app->onHelpContextModule( theComponentName, theFileName, theContext );
}

