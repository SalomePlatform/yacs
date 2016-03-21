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

#include "ItemMimeData.hxx"
#include "guiObservers.hxx"
#include "Catalog.hxx"

#include <QStringList>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

ItemMimeData::ItemMimeData(): QMimeData()
{
  _sub.clear();
  _catalog.clear();
  _cataName.clear();
  _compoName.clear();
  _typeName.clear();
  _swCase=0;
  _control=false;
}

ItemMimeData::~ItemMimeData()
{
}

void ItemMimeData::setSubject(Subject *sub)
{
  _sub.push_back(sub);
}

void ItemMimeData::setCatalog(YACS::ENGINE::Catalog *cata)
{
  _catalog.push_back(cata);
}

void ItemMimeData::setCataName(std::string cataName)
{
  _cataName.push_back(cataName);
}

void ItemMimeData::setCompo(std::string compo)
{
  _compoName.push_back(compo);
}

void ItemMimeData::setType(std::string aType)
{
  _typeName.push_back(aType);
}

void ItemMimeData::setCase(int aCase)
{
  _swCase = aCase;
}

Subject* ItemMimeData::getSubject(int i) const
{
  YASSERT(i < _sub.size());
  return _sub[i];
}

YACS::ENGINE::Catalog* ItemMimeData::getCatalog(int i) const
{
  YASSERT(i < _catalog.size());
  return _catalog[i];
}

std::string ItemMimeData::getCataName(int i) const
{
  YASSERT(i < _cataName.size());
  return _cataName[i];
}

std::string ItemMimeData::getCompo(int i) const
{
  YASSERT(i < _compoName.size());
  return _compoName[i];
}

std::string ItemMimeData::getType(int i) const
{
  YASSERT(i < _typeName.size());
  return _typeName[i];
}

int ItemMimeData::getCase() const
{
  return _swCase;
}
void ItemMimeData::setControl(bool control)
{
  _control=control;
}
bool ItemMimeData::getControl() const
{
  return _control;
}

int ItemMimeData::getDataSize() const
{
  int lg=0;
  QStringList myFormats = formats();
  for (int i=0; i<myFormats.size(); i++)
    {
      if (myFormats[i].contains("yacs/cata"))
        {
          lg = _cataName.size();
          break;
        }
      else
        {
          lg = _sub.size();
          break;
        }
    }
  return lg;
}
