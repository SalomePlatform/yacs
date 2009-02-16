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
  virtual bool deleteSchema(QWidget* viewWindow);
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