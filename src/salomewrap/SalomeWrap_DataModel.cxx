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

#include "SalomeWrap_DataModel.hxx"

#include <SalomeApp_DataObject.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Module.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SALOME_ListIO.hxx>
#include <SUIT_DataBrowser.h>
#include <SUIT_ViewManager.h>
#include <QxScene_ViewWindow.h>

#include <QFileInfo>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

SalomeWrap_DataModel::SalomeWrap_DataModel(CAM_Module* theModule)
  : SalomeApp_DataModel(theModule)
{
  DEBTRACE("SalomeWrap_DataModel::SalomeWrap_DataModel");
  _viewEntryMap.clear();
  _entryViewMap.clear();
}

SalomeWrap_DataModel::~SalomeWrap_DataModel()
{
  DEBTRACE("SalomeWrap_DataModel::~SalomeWrap_DataModel");
}

void SalomeWrap_DataModel::createNewSchema(const QString& schemaName,
                                           QWidget* viewWindow)
{
  DEBTRACE("SalomeWrap_DataModel::createNewSchema : " << schemaName.toStdString());
  SalomeApp_ModuleObject* aRoot = dynamic_cast<SalomeApp_ModuleObject*>(root());
  if (!aRoot)
    {
      DEBTRACE("SalomeApp_ModuleObject* aRoot =0");
      return;
    }
  DEBTRACE("aRoot OK");
  _PTR(SComponent)         aSComp(aRoot->object());
  _PTR(Study)              aStudy = getStudy()->studyDS();
  _PTR(StudyBuilder)       aBuilder(aStudy->NewBuilder());
  _PTR(GenericAttribute)   anAttr;
  _PTR(AttributeName)      aName;
  _PTR(AttributePixMap)    aPixmap;
  _PTR(AttributeParameter) aType;

  // --- create a new schema SObject

  _PTR(SObject) aSObj;
  aSObj = aBuilder->NewObject(aSComp);

  anAttr =  aBuilder->FindOrCreateAttribute(aSObj, "AttributeName");
  aName = _PTR(AttributeName)(anAttr);
  aName->SetValue(QFileInfo(schemaName).baseName().toStdString());

  anAttr = aBuilder->FindOrCreateAttribute(aSObj, "AttributePixMap");
  aPixmap = _PTR(AttributePixMap)(anAttr);
  aPixmap->SetPixMap("schema.png");

  anAttr = aBuilder->FindOrCreateAttribute(aSObj, "AttributeParameter");
  aType = _PTR(AttributeParameter)(anAttr);
  aType->SetInt("ObjectType", SchemaObject);
  string filePath = schemaName.toStdString();
  aType->SetString("FilePath", filePath.c_str());
  DEBTRACE("DataModel : FilePath = " << filePath);

  _viewEntryMap[viewWindow] = aSObj->GetID();
  DEBTRACE(viewWindow);
  _entryViewMap[aSObj->GetID()] = viewWindow;
  DEBTRACE("--- " << viewWindow << " "<< aSObj->GetID());
  SalomeApp_Module *mod = dynamic_cast<SalomeApp_Module*>(module());
  if (mod) mod->updateObjBrowser();

  QxScene_ViewWindow *swv = dynamic_cast<QxScene_ViewWindow*>(viewWindow);
  if (!swv) return;
  QString tabName = QFileInfo(schemaName).baseName();
  swv->getViewManager()->setTitle(tabName);
}

bool SalomeWrap_DataModel::renameSchema(const QString& oldName,
                                        const QString& newName,
                                        QWidget* viewWindow)
{
  DEBTRACE("SalomeWrap_DataModel::renameSchema " << oldName.toStdString()
           << " " << newName.toStdString());
  SalomeApp_ModuleObject* aRoot = dynamic_cast<SalomeApp_ModuleObject*>(root());
  if (!aRoot) return false;
  if (!_viewEntryMap.count(viewWindow)) return false;

  _PTR(SComponent)         aSComp(aRoot->object());
  _PTR(Study)              aStudy = getStudy()->studyDS();

  string id = _viewEntryMap[viewWindow];
  _PTR(SObject) aSObj = aStudy->FindObjectID(id);

  _PTR(StudyBuilder)       aBuilder(aStudy->NewBuilder());
  _PTR(GenericAttribute)   anAttr;
  _PTR(AttributeName)      aName;
  _PTR(AttributeParameter) aType;

  anAttr =  aBuilder->FindOrCreateAttribute(aSObj, "AttributeName");
  aName = _PTR(AttributeName)(anAttr);
  aName->SetValue(QFileInfo(newName).baseName().toStdString());

  anAttr = aBuilder->FindOrCreateAttribute(aSObj, "AttributeParameter");
  aType = _PTR(AttributeParameter)(anAttr);
  aType->SetInt("ObjectType", SchemaObject);
  string filePath = newName.toStdString();
  aType->SetString("FilePath", filePath.c_str());
  DEBTRACE("DataModel : FilePath = " << filePath);

  SalomeApp_Module *mod = dynamic_cast<SalomeApp_Module*>(module());
  if (mod) mod->updateObjBrowser();

  QxScene_ViewWindow *swv = dynamic_cast<QxScene_ViewWindow*>(viewWindow);
  QString tabName = QFileInfo(newName).baseName();
  if (swv) swv->getViewManager()->setTitle(tabName);
  return true;
}

bool SalomeWrap_DataModel::deleteSchema(QWidget* viewWindow)
{
  DEBTRACE("SalomeWrap_DataModel::deleteSchema");
  SalomeApp_ModuleObject* aRoot = dynamic_cast<SalomeApp_ModuleObject*>(root());
  if (!aRoot) return false;
  if (!_viewEntryMap.count(viewWindow)) return false;

  _PTR(SComponent)         aSComp(aRoot->object());
  _PTR(Study)              aStudy = getStudy()->studyDS();

  string id = _viewEntryMap[viewWindow];
  _PTR(SObject) aSObj = aStudy->FindObjectID(id);

  _PTR(StudyBuilder)       aBuilder(aStudy->NewBuilder());
  aBuilder->RemoveObject(aSObj);

  SalomeApp_Module *mod = dynamic_cast<SalomeApp_Module*>(module());
  if (mod) mod->updateObjBrowser();
  return true;
}

void SalomeWrap_DataModel::createNewRun(const QString& schemaName,
                                        const QString& runName,
                                        QWidget* refWindow,
                                        QWidget* viewWindow)
{
  DEBTRACE("SalomeWrap_DataModel::createNewRun");
  SalomeApp_ModuleObject* aRoot = dynamic_cast<SalomeApp_ModuleObject*>(root());
  if (!aRoot) return;
  DEBTRACE(refWindow);
  if (!_viewEntryMap.count(refWindow)) return;
  DEBTRACE("---");
  
  _PTR(SComponent)         aSComp(aRoot->object());
  _PTR(Study)              aStudy = getStudy()->studyDS();

  _PTR(StudyBuilder)       aBuilder(aStudy->NewBuilder());
  _PTR(GenericAttribute)   anAttr;
  _PTR(AttributeName)      aName;
  _PTR(AttributePixMap)    aPixmap;
  _PTR(AttributeParameter) aType;

  string id = _viewEntryMap[refWindow];
  _PTR(SObject) aRefObj = aStudy->FindObjectID(id);

  // --- create a new schema SObject

  _PTR(SObject) aSObj;
  aSObj = aBuilder->NewObject(aRefObj);

  anAttr =  aBuilder->FindOrCreateAttribute(aSObj, "AttributeName");
  aName = _PTR(AttributeName)(anAttr);
  aName->SetValue(QFileInfo(runName).baseName().toStdString());

  anAttr = aBuilder->FindOrCreateAttribute(aSObj, "AttributePixMap");
  aPixmap = _PTR(AttributePixMap)(anAttr);
  aPixmap->SetPixMap("schema.png");

  anAttr = aBuilder->FindOrCreateAttribute(aSObj, "AttributeParameter");
  aType = _PTR(AttributeParameter)(anAttr);
  aType->SetInt("ObjectType", SchemaObject);
  string filePath = runName.toStdString();
  aType->SetString("FilePath", filePath.c_str());
  DEBTRACE("DataModel : FilePath = " << filePath);

  _viewEntryMap[viewWindow] = aSObj->GetID();
  _entryViewMap[aSObj->GetID()] = viewWindow;
  DEBTRACE("--- " << viewWindow << " "<< aSObj->GetID());
  SalomeApp_Module *mod = dynamic_cast<SalomeApp_Module*>(module());
  if (mod) mod->updateObjBrowser();

  QxScene_ViewWindow *swv = dynamic_cast<QxScene_ViewWindow*>(viewWindow);
  if (!swv) return;

  int count = 0;
  if (_runCountMap.count(schemaName.toStdString()))
    count = ++_runCountMap[schemaName.toStdString()];
  else
    _runCountMap[schemaName.toStdString()] = count;

  QString tabName = QFileInfo(schemaName).baseName() +QString("_run%1").arg(count);
  swv->getViewManager()->setTitle(tabName);

  QPixmap pixmap;
  pixmap.load("icons:run_active.png");
  swv->getViewManager()->setIcon(pixmap);
}

void SalomeWrap_DataModel::setSelected(QWidget* viewWindow)
{
  DEBTRACE("SalomeWrap_DataModel::setSelected");
  if (!_viewEntryMap.count(viewWindow)) return;
  string entry = _viewEntryMap[viewWindow];
  LightApp_SelectionMgr* selMgr = getModule()->getApp()->selectionMgr();
  SALOME_ListIO ioList;
  ioList.Append( new SALOME_InteractiveObject( entry.c_str(), "", "" ) );
  selMgr->setSelectedObjects( ioList, false );
}

SUIT_DataObject* SalomeWrap_DataModel::getDataObject(std::string entry)
{
  DEBTRACE("SalomeWrap_DataModel::getDataObject " << entry);
  QString refEntry = entry.c_str();
  SUIT_DataObject* sdo = 0;
  SalomeApp_ModuleObject* aRoot = dynamic_cast<SalomeApp_ModuleObject*>(root());
  if (!aRoot) return 0;

  QList<SUIT_DataObject*> objList = root()->children(true);
  for (int i = 0; i < objList.size(); ++i)
    {
      SalomeApp_DataObject* obj = dynamic_cast<SalomeApp_DataObject*>(objList.at(i));
      if (obj && (!QString::compare(refEntry, obj->entry())))
        {
          sdo = obj;
          break;
        }
    }
  return sdo;
}

QWidget* SalomeWrap_DataModel::getViewWindow(std::string entry)
{
  if (! _entryViewMap.count(entry)) return 0;
  return _entryViewMap[entry];
}
