//  HXX2SALOME_GENERIC_CLASS_NAMEGUI : HXX2SALOME_GENERIC_CLASS_NAME component GUI implemetation 
//

#ifndef _HXX2SALOME_GENERIC_CLASS_NAMEGUI_H_
#define _HXX2SALOME_GENERIC_CLASS_NAMEGUI_H_

#include <SalomeApp_Module.h>

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(HXX2SALOME_GENERIC_CLASS_NAME_Gen)

class SalomeApp_Application;
class HXX2SALOME_GENERIC_CLASS_NAMEGUI: public SalomeApp_Module
{
  Q_OBJECT

public:
  HXX2SALOME_GENERIC_CLASS_NAMEGUI();

  void    initialize( CAM_Application* );
  QString engineIOR() const;
  void    windows( QMap<int, int>& ) const;

  static HXX2SALOME_GENERIC_CLASS_NAME_ORB::HXX2SALOME_GENERIC_CLASS_NAME_Gen_ptr InitHXX2SALOME_GENERIC_CLASS_NAMEGen( SalomeApp_Application* );

  virtual void                createPreferences();
  virtual void                preferencesChanged( const QString&, const QString& );

public slots:
  bool    deactivateModule( SUIT_Study* );
  bool    activateModule( SUIT_Study* );

protected slots:
  void            OnMyNewItem();
  void            OnCallAction();

private:
  bool default_bool;
  int default_int;
  int default_spinInt;
  double default_spinDbl;
  QString default_selection;
  
  QStringList selector_strings;
  
};

#endif
