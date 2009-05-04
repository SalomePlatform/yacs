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

#include <SalomeWrap_Resource.hxx>
#include <SUIT_ResourceMgr.h>

#define RESOURCE_YACS "YACS"

SalomeWrap_Resource::SalomeWrap_Resource(SUIT_ResourceMgr* r) {
  _resource = r;
}

int SalomeWrap_Resource::integerValue(const QString& name, const int def) const {
  return _resource->integerValue(RESOURCE_YACS, name, def);
}

double SalomeWrap_Resource::doubleValue(const QString& name, const double def) const {
  return _resource->doubleValue(RESOURCE_YACS, name, def);
}

bool SalomeWrap_Resource::booleanValue(const QString& name, const bool def) const {
  return _resource->booleanValue(RESOURCE_YACS, name, def);
}

QFont SalomeWrap_Resource::fontValue(const QString& name, const QFont def) const {
  return _resource->fontValue(RESOURCE_YACS, name, def);
}

QColor SalomeWrap_Resource::colorValue(const QString& name, const QColor def) const {
  return _resource->colorValue(RESOURCE_YACS, name, def);
}

QString SalomeWrap_Resource::stringValue(const QString& name, const QString def) const {
  return _resource->stringValue(RESOURCE_YACS, name, def);
}
