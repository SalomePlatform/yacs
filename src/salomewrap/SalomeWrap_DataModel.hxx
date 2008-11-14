
#ifndef _SALOMEWRAP_DATAMODEL_HXX_
#define _SALOMEWRAP_DATAMODEL_HXX_

#include <SalomeApp_DataModel.h>

#include <QWidget>
#include <map>
#include <string>

class SalomeWrap_DataModel: public SalomeApp_DataModel
{
public:
  SalomeWrap_DataModel(CAM_Module* theModule);
  virtual ~SalomeWrap_DataModel();
  
  virtual void createNewSchema(const QString& schemaName,
                               QWidget* viewWindow);
  virtual bool renameSchema(const QString& oldName,
                            const QString& newName,
                            QWidget* viewWindow);
  virtual void createNewRun(const QString& schemaName,
                            const QString& runName,
                            QWidget* refWindow,
                            QWidget* viewWindow);
  virtual void setSelected(QWidget* viewWindow);

  SUIT_DataObject* getDataObject(std::string entry);
  QWidget* getViewWindow(std::string entry);

 public:
  enum ObjectType
    {
      SchemaObject,
      RunObject,
      UnknownObject
    };

protected:
  std::map<QWidget*, std::string> _viewEntryMap;
  std::map<std::string, QWidget*> _entryViewMap;
  
};


#endif
