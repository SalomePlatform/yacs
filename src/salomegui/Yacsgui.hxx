#ifndef _YACSGUI_HXX_
#define _YACSGUI_HXX_

#include "SalomeWrap_Module.hxx"
#include <SALOMEconfig.h>

#include <QModelIndex>

namespace YACS
{
  namespace HMI
  {
    class GenericGui;
    class SuitWrapper;
  }
}

class SalomeApp_Application;
class SUIT_ViewWindow;

class Yacsgui: public SalomeWrap_Module
{
  Q_OBJECT

public:
  Yacsgui();

  void initialize( CAM_Application* app);
  void windows( QMap<int, int>& theMap) const;
  virtual QString  engineIOR() const;

  virtual void viewManagers( QStringList& list) const;

public slots:
  bool deactivateModule( SUIT_Study* theStudy);
  bool activateModule( SUIT_Study* theStudy);
  void onDblClick(const QModelIndex& index);

protected slots:
  void onWindowActivated( SUIT_ViewWindow* svw);
  void onWindowClosed( SUIT_ViewWindow* svw);

protected:
  virtual  CAM_DataModel* createDataModel();
  bool createSComponent(); 

  YACS::HMI::SuitWrapper* _wrapper;
  YACS::HMI::GenericGui* _genericGui;
  bool _selectFromTree;
};

#endif
