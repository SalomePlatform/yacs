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

#ifndef _SCHEMAITEM_HXX_
#define _SCHEMAITEM_HXX_

#include "SchemaModel.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Node;
  }

  namespace HMI
  {
    class ItemMimeData;

    class SchemaItem: public QObject, public GuiObserver
    {
      Q_OBJECT

    public:
      SchemaItem(SchemaItem *parent, QString label, Subject* subject);
      virtual ~SchemaItem();
      
      virtual void appendChild(SchemaItem *child);
      virtual void removeChild(SchemaItem *child);
      virtual void insertChild(int row, SchemaItem *child);
      
      virtual SchemaItem *child(int row);
      virtual int childCount() const;
      virtual int columnCount() const;
      virtual QVariant data(int column, int role) const;
      virtual Qt::ItemFlags flags(const QModelIndex &index);
      virtual int row() const;
      virtual SchemaItem *parent();
      virtual Subject* getSubject();
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual void select(bool isSelected);
      virtual void toggleState();
      QModelIndex modelIndex(int column = 0);
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
      virtual ItemMimeData* mimeData(ItemMimeData *mime);
      virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action);
      virtual void reparent(SchemaItem *parent);
      virtual void setCaseValue();

      bool isEmphasized() {return _emphasized; };
      void setEmphasize(bool emphasize) {_emphasized = emphasize; };

    protected:
      virtual QVariant editionToolTip(int column) const;
      virtual QVariant runToolTip(int column) const;
      virtual QVariant editionWhatsThis(int column) const;
      virtual QVariant runWhatsThis(int column) const;
      virtual QString getMimeFormat();
      virtual void setExecState(int execState);
      QList<SchemaItem*> _childItems;
      QList<QVariant> _itemData;
      QList<QVariant> _itemDeco;
      QList<QVariant> _itemForeground;
      QList<QVariant> _itemBackground;
      QList<QVariant> _itemCheckState;
      QList<QVariant> _itemToolTip;
      QList<QVariant> _itemWhatsThis;
      QString _label;
      SchemaItem *_parentItem;
      Subject* _subject;
      int _execState;
      bool _emphasized;
    };


//     class SchemaSelectionModel: public QItemSelectionModel
//     {
//     public:
//       SchemaSelectionModel(QAbstractItemModel *model);
//       SchemaSelectionModel(QAbstractItemModel *model, QObject *parent);
//       virtual ~SchemaSelectionModel();
//     };

  }
}

#endif
