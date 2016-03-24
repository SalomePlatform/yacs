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
