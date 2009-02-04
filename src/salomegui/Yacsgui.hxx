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
  void onTryClose(bool &isClosed, QxScene_ViewWindow* window);

protected:
  virtual  CAM_DataModel* createDataModel();
  bool createSComponent(); 

  YACS::HMI::SuitWrapper* _wrapper;
  YACS::HMI::GenericGui* _genericGui;
  bool _selectFromTree;
};

#endif
