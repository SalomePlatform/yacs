// Copyright (C) 2006-2024  CEA, EDF
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

#ifndef _SCHEMADIRTLINKSITEM_HXX_
#define _SCHEMADIRTLINKSITEM_HXX_

#include "SchemaItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SchemaDirLinksItem: public SchemaItem
    {
    public:
      SchemaDirLinksItem(SchemaItem *parent, QString label, Subject* subject);
      void addLinkItem(Subject* subject);
      virtual Qt::ItemFlags flags(const QModelIndex &index);
    protected:
      virtual QVariant editionWhatsThis(int column) const;
    };
  }
}

#endif
