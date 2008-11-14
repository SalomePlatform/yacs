
#ifndef _YACSGUI_DATAMODEL_HXX_
#define _YACSGUI_DATAMODEL_HXX_

#include <SalomeWrap_DataModel.hxx>

class Yacsgui_DataModel: public SalomeWrap_DataModel
{
public:
  Yacsgui_DataModel(CAM_Module* theModule);
  virtual ~Yacsgui_DataModel();
  
  virtual bool open(const QString& fileName, CAM_Study* study, QStringList listOfFiles);
  virtual bool save(QStringList& listOfFiles);
  virtual bool saveAs(const QString& fileName, CAM_Study* study, QStringList& listOfFiles);
  
protected:
  
};


#endif
