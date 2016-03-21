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

#ifndef _SCHEMACOMPOSEDNODEITEM_HXX_
#define _SCHEMACOMPOSEDNODEITEM_HXX_

#include "SchemaItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SchemaDirTypesItem;
    class SchemaDirContainersItem;
    class SchemaDirLinksItem;

    class SchemaComposedNodeItem: public SchemaItem
    {
    public:
      SchemaComposedNodeItem(SchemaItem *parent, QString label, Subject* subject);
      virtual ~SchemaComposedNodeItem();
      virtual void update(GuiEvent event, int type, Subject* son);
      std::list<YACS::ENGINE::Node*> getDirectDescendants() const;
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
      virtual Qt::ItemFlags flags(const QModelIndex &index);
      virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action);
      virtual void setCaseValue();
    protected:
      virtual QString getMimeFormat();
      virtual QVariant editionWhatsThis(int column) const;
      SchemaDirTypesItem *_dirTypesItem;
      SchemaDirContainersItem *_dirContainersItem;
      SchemaDirLinksItem *_dirLinksItem;
    };
  }
}

#endif
