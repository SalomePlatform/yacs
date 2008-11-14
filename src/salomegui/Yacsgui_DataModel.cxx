
#include "Yacsgui_DataModel.hxx"

#define _DEVDEBUG_
#include "YacsTrace.hxx"

Yacsgui_DataModel::Yacsgui_DataModel(CAM_Module* theModule)
  : SalomeWrap_DataModel(theModule)
{
  DEBTRACE("Yacsgui_DataModel::Yacsgui_DataModel");
}

Yacsgui_DataModel:: ~Yacsgui_DataModel()
{
  DEBTRACE("Yacsgui_DataModel:: ~Yacsgui_DataModel");
}
  
bool Yacsgui_DataModel::open(const QString& fileName, CAM_Study* study, QStringList listOfFiles)
{
  DEBTRACE("Yacsgui_DataModel::open");
  SalomeWrap_DataModel::open(fileName, study, listOfFiles);
}

bool Yacsgui_DataModel::save(QStringList& listOfFiles)
{
  DEBTRACE("Yacsgui_DataModel::save");
  SalomeWrap_DataModel::save(listOfFiles);
}

bool Yacsgui_DataModel::saveAs(const QString& fileName, CAM_Study* study, QStringList& listOfFiles)
{
  DEBTRACE("Yacsgui_DataModel::saveAs");
  SalomeWrap_DataModel::saveAs(fileName, study, listOfFiles);
}

