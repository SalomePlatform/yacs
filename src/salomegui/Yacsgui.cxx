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
#include "Yacsgui.hxx"
#include "Yacsgui_DataModel.hxx"

#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewWindow.h>
#include <SalomeApp_Application.h>
#include <QxScene_ViewManager.h>
#include <QxScene_ViewModel.h>
#include <QxScene_ViewWindow.h>

#include <SalomeApp_DataObject.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Module.h>
#include <SUIT_DataBrowser.h>
#include <QtxTreeView.h>
#include <SUIT_DataObject.h>

#include <SALOME_LifeCycleCORBA.hxx>
#include <QInputDialog>
#include <QIcon>
#include <cassert>

#include "GenericGui.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

Yacsgui::Yacsgui() :
  SalomeWrap_Module( "YACS" ) // default name
{
  _wrapper = 0;
  _genericGui = 0;
  _selectFromTree = false;
}

void Yacsgui::initialize( CAM_Application* app )
{
  DEBTRACE("Yacsgui::initialize");
  SalomeApp_Module::initialize( app );

  QWidget* aParent = application()->desktop();
  DEBTRACE(app << "  " << application() << " " << application()->desktop() << " " << aParent);

  SUIT_ResourceMgr* aResourceMgr = app->resourceMgr();
  _wrapper = new SuitWrapper(this);
  _genericGui = new GenericGui(_wrapper, app->desktop());

  if ( app && app->desktop() )
    {
      connect( app->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
               this, SLOT(onWindowActivated( SUIT_ViewWindow* )) );

      connect( getApp()->objectBrowser()->treeView(),
               SIGNAL( doubleClicked(const QModelIndex&) ), 
               this,
               SLOT  ( onDblClick(const QModelIndex&) ) );

      connect( getApp(),
               SIGNAL(studyClosed()),
               _genericGui,
               SLOT  (onCleanOnExit()));
    }
  _genericGui->createActions();
  _genericGui->createMenus();
  _genericGui->createTools();

  if (createSComponent()) updateObjBrowser();
}

void Yacsgui::viewManagers( QStringList& list ) const
{
  DEBTRACE("Yacsgui::viewManagers");
  list.append( QxScene_Viewer::Type() );
}

bool Yacsgui::activateModule( SUIT_Study* theStudy )
{
  DEBTRACE("Yacsgui::activateModule");
  bool bOk = SalomeApp_Module::activateModule( theStudy );

  setMenuShown( true );
  setToolShown( true );
  _genericGui->showDockWidgets(true);

  return bOk;
}

bool Yacsgui::deactivateModule( SUIT_Study* theStudy )
{
  DEBTRACE("Yacsgui::deactivateModule");

  setMenuShown( false );
  setToolShown( false );
  _genericGui->showDockWidgets(false);

  SUIT_ViewManager *svm = getApp()->getViewManager(QxScene_Viewer::Type(), true);
  assert(svm);
  SUIT_ViewWindow* svw = svm->getActiveView();
  QxScene_ViewWindow *aView = 0;
  if (svw) aView = dynamic_cast<QxScene_ViewWindow*>(svw);
  DEBTRACE("aView " << aView);

  return SalomeApp_Module::deactivateModule( theStudy );
}

// --- Default windows

void Yacsgui::windows( QMap<int, int>& theMap ) const
{
  DEBTRACE("Yacsgui::windows");
  theMap.clear();
  theMap.insert( SalomeApp_Application::WT_ObjectBrowser, Qt::LeftDockWidgetArea );
//   theMap.insert( SalomeApp_Application::WT_PyConsole,     Qt::BottomDockWidgetArea );
}

QString  Yacsgui::engineIOR() const
{
  DEBTRACE("Yacsgui::engineIOR");
  return getApp()->defaultEngineIOR();
}

void Yacsgui::onDblClick(const QModelIndex& index)
{
  DEBTRACE("Yacsgui::onDblClick");
  DataObjectList dol =getApp()->objectBrowser()->getSelected();
  if (dol.isEmpty()) return;

  SalomeApp_DataObject* item = dynamic_cast<SalomeApp_DataObject*>(dol[0]);
  if (!item) return;

  DEBTRACE(item->name().toStdString());
  SalomeWrap_DataModel *model = dynamic_cast<SalomeWrap_DataModel*>(dataModel());
  if (!model) return;
  DEBTRACE(item->entry().toStdString());
  QWidget * viewWindow = model->getViewWindow(item->entry().toStdString());
  if (!_genericGui) return;
  if (!viewWindow) return;
  DEBTRACE("--- " << viewWindow << " "  << item->entry().toStdString());

  _selectFromTree = true;
  viewWindow->setFocus();
  _selectFromTree = false;
}

void Yacsgui::onWindowActivated( SUIT_ViewWindow* svw)
{
  DEBTRACE("Yacsgui::onWindowActivated");
  QxScene_ViewWindow* viewWindow = dynamic_cast<QxScene_ViewWindow*>(svw);
  if (!viewWindow) return;
  DEBTRACE("viewWindow " << viewWindow);
  DEBTRACE("activeModule()->moduleName() " << getApp()->activeModule()->moduleName().toStdString());
  if (getApp()->activeModule()->moduleName().compare("YACS") != 0)
    getApp()->activateModule("YACS");

  assert(_genericGui);
  _genericGui->switchContext(viewWindow);

  if (_selectFromTree) return;
  SalomeWrap_DataModel *model = dynamic_cast<SalomeWrap_DataModel*>(dataModel());
  if (!model) return;
  model->setSelected(svw);
}

void Yacsgui::onWindowClosed( SUIT_ViewWindow* svw)
{
  DEBTRACE("Yacsgui::onWindowClosed");
}

CAM_DataModel* Yacsgui::createDataModel()
{
  return new Yacsgui_DataModel(this);
}

bool Yacsgui::createSComponent()
{
  DEBTRACE("Yacsgui::createSComponent");
  _PTR(Study)            aStudy = (( SalomeApp_Study* )(getApp()->activeStudy()))->studyDS();
  _PTR(StudyBuilder)     aBuilder (aStudy->NewBuilder());
  _PTR(GenericAttribute) anAttr;
  _PTR(AttributeName)    aName;

  // --- Find or create "YACS" SComponent in the study

  _PTR(SComponent) aComponent = aStudy->FindComponent("YACS");
  if ( !aComponent )
    {
      aComponent = aBuilder->NewComponent("YACS");
      anAttr = aBuilder->FindOrCreateAttribute(aComponent, "AttributeName");
      aName = _PTR(AttributeName) (anAttr);
      aName->SetValue(getApp()->moduleTitle("YACS").toStdString());
      
      anAttr = aBuilder->FindOrCreateAttribute(aComponent, "AttributePixMap");
      _PTR(AttributePixMap) aPixmap(anAttr);
      aPixmap->SetPixMap("share/salome/resources/yacs/ModuleYacs.png");
      
      aBuilder->DefineComponentInstance(aComponent, getApp()->defaultEngineIOR().toStdString());

      return true;
    }
  return false;
}

// --- Export the module

extern "C"
{
  CAM_Module* createModule()
  {
    return new Yacsgui();
  }
}
