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
#include "ItemMimeData.hxx"
#include "guiObservers.hxx"
#include "Catalog.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

ItemMimeData::ItemMimeData(): QMimeData()
{
  _sub = 0;
  _catalog = 0;
  _cataName.clear();
  _compoName.clear();
  _typeName.clear();
  _swCase=0;
}

ItemMimeData::~ItemMimeData()
{
}

void ItemMimeData::setSubject(Subject *sub)
{
  _sub = sub;
}

void ItemMimeData::setCatalog(YACS::ENGINE::Catalog *cata)
{
  _catalog = cata;
}

void ItemMimeData::setCataName(std::string cataName)
{
  _cataName = cataName;
}

void ItemMimeData::setCompo(std::string compo)
{
  _compoName = compo;
}

void ItemMimeData::setType(std::string aType)
{
  _typeName = aType;
}

void ItemMimeData::setCase(int aCase)
{
  _swCase = aCase;
}

Subject* ItemMimeData::getSubject() const
{
  return _sub;
}

YACS::ENGINE::Catalog* ItemMimeData::getCatalog() const
{
  return _catalog;
}

std::string ItemMimeData::getCataName() const
{
  return _cataName;
}

std::string ItemMimeData::getCompo() const
{
  return _compoName;
}

std::string ItemMimeData::getType() const
{
  return _typeName;
}

int ItemMimeData::getCase() const
{
  return _swCase;
}
