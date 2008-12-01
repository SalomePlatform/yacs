
#include "SalomeWrap_DataModel.hxx"
#include "guiContext.hxx"

#include <SalomeApp_DataObject.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Module.h>
#include <SalomeApp_Application.h>
#include <SUIT_DataBrowser.h>
#include <SUIT_ViewManager.h>
#include <QxScene_ViewWindow.h>

#include <QFileInfo>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

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
  swv->getViewManager()->setTitle(schemaName);
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
  if (swv) swv->getViewManager()->setTitle(newName);
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
  swv->getViewManager()->setTitle(runName);
}

void SalomeWrap_DataModel::setSelected(QWidget* viewWindow)
{
  DEBTRACE("SalomeWrap_DataModel::setSelected");
  if (!_viewEntryMap.count(viewWindow)) return;
  string entry = _viewEntryMap[viewWindow];
  SUIT_DataObject* item = getDataObject(entry);
  if(item) getModule()->getApp()->objectBrowser()->setSelected(item);
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
