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

#include <Python.h>
#include "YACSExport.hxx"
#include "Yacsgui.hxx"
#include "Yacsgui_DataModel.hxx"
#include "Yacsgui_Resource.hxx"

#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewWindow.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Engine_i.h>
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
#include <QDir>
#include <QInputDialog>
#include <QIcon>
#include <cassert>

#include "GenericGui.hxx"
#include "CatalogWidget.hxx"
#include "Resource.hxx"
#include "QtGuiContext.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

int  Yacsgui::_oldStudyId = -1;

Yacsgui::Yacsgui() :
  SalomeWrap_Module( "YACS" ) // default name
{
  DEBTRACE("Yacsgui::Yacsgui");
  _wrapper = 0;
  _genericGui = 0;
  _selectFromTree = false;
  _studyContextMap.clear();
}

Yacsgui::~Yacsgui()
{
  if ( getApp() )
    disconnect( getApp(), SIGNAL(studyClosed()), this, SLOT  (onCleanOnExit()));
  delete _wrapper;
  delete _genericGui;
}

void Yacsgui::initialize( CAM_Application* app )
{
  DEBTRACE("Yacsgui::initialize");
  _currentSVW = 0;
  SalomeApp_Module::initialize( app );

  QWidget* aParent = application()->desktop();
  DEBTRACE(app << "  " << application() << " " << application()->desktop() << " " << aParent);

  SUIT_ResourceMgr* aResourceMgr = app->resourceMgr();
  setResource(aResourceMgr);

  _wrapper = new SuitWrapper(this);
  _genericGui = new GenericGui(_wrapper, app->desktop());

  if ( app && app->desktop() )
    {
      connect( app->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
               this, SLOT(onWindowActivated( SUIT_ViewWindow* )) );

      connect( getApp(),
               SIGNAL(objectDoubleClicked( SUIT_DataObject* )), 
               this,
               SLOT  (onDblClick( SUIT_DataObject* )));

      connect( getApp(),
               SIGNAL(studyClosed()),
               this,
               SLOT  (onCleanOnExit()));
    }
  _genericGui->createActions();
  _genericGui->createMenus();
  _genericGui->createTools();
  this->studyActivated();

  // VSR 23/10/2014: note that this is not a good way to create SComponent from this point
  // as initialize() method can be potentially called when there's no yet open study;
  // this is better to do in activateModule()
  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if ( aStudy ) {
    bool aLocked = (_PTR(AttributeStudyProperties)(aStudy->studyDS()->GetProperties()))->IsLocked();
    if ( aLocked ) {
      SUIT_MessageBox::warning ( app->desktop(), QObject::tr("WRN_WARNING"), QObject::tr("WRN_STUDY_LOCKED") );
    }
    else  {
      if (createSComponent()) updateObjBrowser();
    }
  }

  // Load SALOME module catalogs
  QStringList appModules;
  app->modules(appModules,false);
  for ( QStringList::const_iterator it = appModules.begin(); it != appModules.end(); ++it )
    {
      QString aModule=*it;
      QString modName = app->moduleName( aModule );                    // module name
      if ( modName.isEmpty() ) modName = aModule;             
      QString rootDir = QString( "%1_ROOT_DIR" ).arg( modName );       // module root dir variable
      QString modDir  = getenv( rootDir.toLatin1().constData() );      // module root dir
      if ( !modDir.isEmpty() ) 
        {
          QStringList cataLst = QStringList() << modDir << "share" << "salome" << "resources" << modName.toLower() << modName+"SchemaCatalog.xml";
          QString cataFile = cataLst.join( QDir::separator() );          // YACS module catalog
          if ( QFile::exists( cataFile ) ) 
            _genericGui->getCatalogWidget()->addCatalogFromFile(cataFile.toStdString());
        }
    }
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

  QMainWindow* parent = application()->desktop();
  if(Resource::dockWidgetPriority)
    {
      parent->setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
      parent->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
      parent->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
      parent->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    }
  setMenuShown( true );
  setToolShown( true );
  _genericGui->showDockWidgets(true);

  // import Python module that manages YACS plugins (need to be here because SalomePyQt API uses active module)
  PyGILState_STATE gstate = PyGILState_Ensure();
  PyObject* pluginsmanager=PyImport_ImportModule((char*)"salome_pluginsmanager");
  if(pluginsmanager==NULL)
    PyErr_Print();
  else
    {
      PyObject* result=PyObject_CallMethod( pluginsmanager, (char*)"initialize", (char*)"isss",1,"yacs","YACS",tr("YACS_PLUGINS").toStdString().c_str());
      if(result==NULL)
        PyErr_Print();
      Py_XDECREF(result);
    }
  Py_XDECREF(pluginsmanager);

  PyGILState_Release(gstate);
  // end of YACS plugins loading

  if (_currentSVW)
    onWindowActivated(_currentSVW);

  return bOk;
}

bool Yacsgui::deactivateModule( SUIT_Study* theStudy )
{
  DEBTRACE("Yacsgui::deactivateModule");

  QMainWindow* parent = application()->desktop();
  parent->setCorner(Qt::TopLeftCorner, Qt::TopDockWidgetArea);
  parent->setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
  parent->setCorner(Qt::TopRightCorner, Qt::TopDockWidgetArea);
  parent->setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

  setMenuShown( false );
  setToolShown( false );
  _genericGui->showDockWidgets(false);
  QtGuiContext *context = QtGuiContext::getQtCurrent();
  _studyContextMap[theStudy->id()] = context;
  DEBTRACE("_studyContextMap[theStudy] " << theStudy << " " << context);
  return SalomeApp_Module::deactivateModule( theStudy );
}

// --- Default windows

void Yacsgui::windows( QMap<int, int>& theMap ) const
{
  DEBTRACE("Yacsgui::windows");
  theMap.clear();
  theMap.insert( SalomeApp_Application::WT_ObjectBrowser, Qt::LeftDockWidgetArea );
  theMap.insert( SalomeApp_Application::WT_PyConsole,     Qt::BottomDockWidgetArea );
}

QString  Yacsgui::engineIOR() const
{
  DEBTRACE("Yacsgui::engineIOR");
  QString anEngineIOR = SalomeApp_Engine_i::EngineIORForComponent( "YACS", true ).c_str();
  return anEngineIOR;
}

void Yacsgui::onDblClick(SUIT_DataObject*)
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
  if (getApp()->activeModule()->moduleName().compare("YACS") != 0)
    getApp()->activateModule("YACS");

  _selectFromTree = true;
  viewWindow->setFocus();
  _selectFromTree = false;
}

void Yacsgui::onWindowActivated( SUIT_ViewWindow* svw)
{
  DEBTRACE("Yacsgui::onWindowActivated");
  QxScene_ViewWindow* viewWindow = dynamic_cast<QxScene_ViewWindow*>(svw);
  _currentSVW = svw;
  if (!viewWindow)
    {
      _currentSVW = 0; // switch to another module
      return;
    }
  DEBTRACE("viewWindow " << viewWindow);
  DEBTRACE("activeModule()->moduleName() " << (getApp()->activeModule() ? getApp()->activeModule()->moduleName().toStdString() : "") );
  if (!getApp()->activeModule() || getApp()->activeModule()->moduleName() != "YACS")
    if ( !getApp()->activateModule("YACS") ) return;

  disconnect(viewWindow, SIGNAL( tryClose( bool&, QxScene_ViewWindow* ) ),
             this, SLOT(onTryClose( bool&, QxScene_ViewWindow* )) );
  disconnect(viewWindow->getViewManager(), SIGNAL( deleteView( SUIT_ViewWindow* ) ),
             this, SLOT(onWindowClosed( SUIT_ViewWindow* )) );
  connect(viewWindow, SIGNAL( tryClose( bool&, QxScene_ViewWindow* ) ),
          this, SLOT(onTryClose( bool&, QxScene_ViewWindow* )) );
  connect(viewWindow->getViewManager(), SIGNAL( deleteView( SUIT_ViewWindow* ) ),
          this, SLOT(onWindowClosed( SUIT_ViewWindow* )) );

  YASSERT(_genericGui);
  _genericGui->switchContext(viewWindow);
  _studyContextMap[getApp()->activeStudy()->id()] = QtGuiContext::getQtCurrent();
  
  if (_selectFromTree) return;
  SalomeWrap_DataModel *model = dynamic_cast<SalomeWrap_DataModel*>(dataModel());
  if (!model) return;
  model->setSelected(svw);
}

void Yacsgui::onWindowClosed( SUIT_ViewWindow* svw)
{
  DEBTRACE("Yacsgui::onWindowClosed");
  if ( svw && svw == _currentSVW )
    _currentSVW = 0;
}

void Yacsgui::onTryClose(bool &isClosed, QxScene_ViewWindow* window)
{
  DEBTRACE("Yacsgui::onTryClose");
  YASSERT(_genericGui);
  isClosed = _genericGui->closeContext(window);
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
      aPixmap->SetPixMap("ModuleYacs.png");
      
      aBuilder->DefineComponentInstance(aComponent, engineIOR().toLatin1().constData());

      return true;
    }
  return false;
}

void Yacsgui::setResource(SUIT_ResourceMgr* r) 
{
  DEBTRACE("Yacsgui::setResource");
  _myresource = new Yacsgui_Resource(r);
  _myresource->preferencesChanged();
}

void Yacsgui::createPreferences() 
{
  DEBTRACE("Yacsgui::createPreferences");
  _myresource->createPreferences(this);
}

void Yacsgui::preferencesChanged( const QString& sect, const QString& name ) 
{
  DEBTRACE("Yacsgui::preferencesChanged");
  _myresource->preferencesChanged(sect, name);
  if(name=="userCatalog")
    {
      _genericGui->getCatalogWidget()->addCatalogFromFile(Resource::userCatalog.toStdString());
    }
}

void Yacsgui::studyActivated()
{
  int newStudyId = getApp()->activeStudy()->id();
  DEBTRACE("Yacsgui::studyActivated " << _oldStudyId << " " << newStudyId);
  
  if (_oldStudyId != -1)
    {
      _studyContextMap[_oldStudyId] = QtGuiContext::getQtCurrent();      
      if (_studyContextMap.count(newStudyId))
        {
          DEBTRACE("switch to valid context " << QtGuiContext::getQtCurrent() << " " << _studyContextMap[newStudyId]);
          QtGuiContext::setQtCurrent(_studyContextMap[newStudyId]);
        }
      else
        {
          DEBTRACE("no switch to null context");
        }
    }
  _oldStudyId = newStudyId;
}

void Yacsgui::loadSchema(const std::string& filename,bool edit, bool arrangeLocalNodes)
{
  _genericGui->loadSchema(filename,edit,arrangeLocalNodes);
}

void Yacsgui::onCleanOnExit()
{
  if ( _genericGui )
    _genericGui->onCleanOnExit();
  _currentSVW = 0;
}

// --- Export the module

extern "C"
{
  YACS_EXPORT CAM_Module* createModule()
  {
    return new Yacsgui();
  }
}
