// Copyright (C) 2006-2021  CEA/DEN, EDF R&D
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

#include "Yacsgui_DataModel.hxx"

//#define _DEVDEBUG_
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
  return SalomeWrap_DataModel::open(fileName, study, listOfFiles);
}

bool Yacsgui_DataModel::save(QStringList& listOfFiles)
{
  DEBTRACE("Yacsgui_DataModel::save");
  return SalomeWrap_DataModel::save(listOfFiles);
}

bool Yacsgui_DataModel::saveAs(const QString& fileName, CAM_Study* study, QStringList& listOfFiles)
{
  DEBTRACE("Yacsgui_DataModel::saveAs");
  return SalomeWrap_DataModel::saveAs(fileName, study, listOfFiles);
}

