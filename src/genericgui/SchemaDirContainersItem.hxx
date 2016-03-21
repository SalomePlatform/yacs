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

#ifndef _SCHEMADIRTCONTAINERSITEM_HXX_
#define _SCHEMADIRTCONTAINERSITEM_HXX_

#include "SchemaItem.hxx"
#include <map>
#include <string>

namespace YACS
{
  namespace HMI
  {
    class SchemaDirContainersItem: public SchemaItem
    {
    public:
      SchemaDirContainersItem(SchemaItem *parent, QString label, Subject* subject);
      void addContainerItem(Subject* subject);
      void addComponentItem(Subject* subject);
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
    protected:
      virtual QVariant editionWhatsThis(int column) const;
      std::map<std::string, SchemaContainerItem*> _schemaContItemMap;
      std::multimap<std::string, Subject*> _waitingCompItemMap;
    };
  }
}

#endif
