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

#ifndef _YACSGUI_HXX_
#define _YACSGUI_HXX_

#include "SalomeWrap_Module.hxx"
#include "Yacsgui_Resource.hxx"
#include <SALOMEconfig.h>

#include <QModelIndex>
#include <map>

namespace YACS
{
  namespace HMI
  {
    class GenericGui;
    class SuitWrapper;
    class QtGuiContext;
  }
}

class SalomeApp_Application;
class SUIT_ViewWindow;

class Yacsgui: public SalomeWrap_Module
{
  Q_OBJECT

  friend class Yacsgui_Resource;

public:
  Yacsgui();
  virtual ~Yacsgui();

  void initialize( CAM_Application* app);
  void windows( QMap<int, int>& theMap) const;
  virtual QString  engineIOR() const;

  virtual void viewManagers( QStringList& list) const;
  virtual void setResource(SUIT_ResourceMgr* r);
  virtual void createPreferences();
  virtual void preferencesChanged( const QString& sect, const QString& name );
  virtual void loadSchema(const std::string& filename,bool edit=true, bool arrangeLocalNodes=false);
  virtual void studyActivated();
  
public slots:
  bool deactivateModule( SUIT_Study* theStudy);
  bool activateModule( SUIT_Study* theStudy);
  void onDblClick(SUIT_DataObject* object);
  void onCleanOnExit();
  
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
  Yacsgui_Resource* _myresource;
  SUIT_ViewWindow* _currentSVW;
  std::map<int, YACS::HMI::QtGuiContext*> _studyContextMap;
  static int _oldStudyId;
};

#endif
