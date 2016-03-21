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

#include "SchemaModel.hxx"
#include "SchemaItem.hxx"
#include "SchemaNodeItem.hxx"
#include "SchemaProcItem.hxx"
#include "QtGuiContext.hxx"
#include "GuiEditor.hxx"
#include "ItemMimeData.hxx"
#include "guiObservers.hxx"

#include <QMetaType>
#include <QVariant>
Q_DECLARE_METATYPE(YACS::HMI::Subject);

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

SchemaModel::SchemaModel(YACS::HMI::Subject *context,
                         QObject * parent): 
  QAbstractItemModel(parent)
{
  DEBTRACE("SchemaModel::SchemaModel");
  _context=context;
  _root=0;
  _rootItem=0;
  _context->attach(this);
  _stdBackBrush = QColor("white");
  _editedBackBrush = QColor("yellow");
  _emphasizeBackBrush = QColor("magenta");
  _isEdition = true;
}

SchemaModel::~SchemaModel()
{
  DEBTRACE("SchemaModel::~SchemaModel");
  _subjectSet.clear(); // --- avoid destruction loop on delete context
}

QModelIndex SchemaModel::index(int row, int column, const QModelIndex &parent) const
{
  //  DEBTRACE("SchemaModel::index");
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  SchemaItem *parentItem = 0;

  if (!parent.isValid())
    parentItem = _rootItem;
  else
    parentItem = static_cast<SchemaItem*>(parent.internalPointer());

  SchemaItem *childItem = parentItem->child(row);
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

QModelIndex SchemaModel::parent(const QModelIndex &index) const
{
  //  DEBTRACE("SchemaModel::parent");
  if (!index.isValid())
    return QModelIndex();
  SchemaItem *childItem = static_cast<SchemaItem*>(index.internalPointer());
  SchemaItem *parentItem = childItem->parent();
  if (parentItem == _rootItem)
    return QModelIndex();
  return createIndex(parentItem->row(), 0, parentItem);
}

int SchemaModel::rowCount(const QModelIndex &parent) const
{
  //DEBTRACE("SchemaModel::rowCount");
  SchemaItem *parentItem;
  if (parent.column() > 0)
    return 0;
  if (!parent.isValid())
    parentItem = _rootItem;
  else
    parentItem = static_cast<SchemaItem*>(parent.internalPointer());
  if (parentItem)
    return parentItem->childCount();
  else return 0;
}

int SchemaModel::columnCount(const QModelIndex &parent) const
{
  //DEBTRACE("SchemaModel::columnCount " << parent.isValid());
  if (parent.isValid())
    return static_cast<SchemaItem*>(parent.internalPointer())->columnCount();
  if (_rootItem)
    {
      if (_isEdition) return 3;
      else return 3; //_rootItem->columnCount();
    }
  else
    return 0;
}

QVariant SchemaModel::data(const QModelIndex &index, int role) const
{
  //DEBTRACE("SchemaModel::data");
  if (!index.isValid())
    return QVariant();
  SchemaItem *item = static_cast<SchemaItem*>(index.internalPointer());
  return item->data(index.column(), role);
}

QVariant SchemaModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  //DEBTRACE("SchemaModel::headerData");
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal)
    //return QString("Colonne %1").arg(section);
    if (_isEdition)
      switch (section)
        {
        case YLabel: return QString("Name");
        case YType:  return QString("Type");
        case YValue: return QString("Value");
        default:     return QString("- %1 -").arg(section);
        }
    else
      switch (section)
        {
        case YLabel: return QString("Name");
        case YType:  return QString("Type");
        case YState: return QString("State");
        default:     return QString("- %1 -").arg(section);
        }
    else
    return QVariant();
}

/*!
 *  For EditRole, setData only emit dataChanged signal,
 *  actual modification is done in SchemaItem.
 */
bool SchemaModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  DEBTRACE("SchemaModel::setData");
  if (index.isValid() && role == Qt::EditRole)
    {
      DEBTRACE("Qt::EditRole, emit dataChanged");
      emit dataChanged(index, index);
      return true;
    }
  if (index.isValid() && role == Qt::CheckStateRole)
    {
      DEBTRACE("Qt::CheckStateRole, toggle state");
      SchemaItem *item = static_cast<SchemaItem*>(index.internalPointer());
      item->toggleState();
      emit dataChanged(index, index);
      return true;
    }
  return false;
}

Qt::ItemFlags SchemaModel::flags(const QModelIndex &index) const
{
  //DEBTRACE("SchemaModel::flags");
  if (!index.isValid())
    return 0;
  SchemaItem *item = static_cast<SchemaItem*>(index.internalPointer());
  return item->flags(index);
}


void SchemaModel::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SchemaModel::update "<<eventName(event)<<" "<<type<<" "<<son);
  switch (event)
    {
    case YACS::HMI::NEWROOT:
      setNewRoot(son);
      break;
//     default:
//       DEBTRACE("SchemaModel::update(), event not handled: "<< eventName(event));
    }
}

void SchemaModel::updateSelection(const QItemSelection &selected,
                                  const QItemSelection &deselected)
{
  DEBTRACE("SchemaModel::updateSelection");
     QModelIndex index;
     QModelIndexList items = selected.indexes();

     foreach (index, items)
       {
         SchemaItem *item = static_cast<SchemaItem*>(index.internalPointer());
         DEBTRACE("updateSelection select "<< item->getSubject()->getName());
         item->getSubject()->select(true);
         emit signalSelection(index);         
       }

     items = deselected.indexes();

     foreach (index, items)
       {
         SchemaItem *item = static_cast<SchemaItem*>(index.internalPointer());
         DEBTRACE("updateSelection deselect "<< item->getSubject()->getName());
         item->getSubject()->select(false);
       }  
}

void SchemaModel::setEdition(bool isEdition)
{
  _isEdition = isEdition;
}

const QBrush& SchemaModel::stdBackBrush()
{
  return _stdBackBrush;
}

const QBrush& SchemaModel::editedBackBrush()
{
  return _editedBackBrush;
}

const QBrush& SchemaModel::emphasizeBackBrush()
{
  return _emphasizeBackBrush;
}

void SchemaModel::setNewRoot(YACS::HMI::Subject *root)
{
  _root = root;
  QString name = _root->getName().c_str();
  _rootItem= new SchemaItem(0, "root", QtGuiContext::getQtCurrent());
  SchemaProcItem *procItem = new SchemaProcItem(_rootItem, name, _root);
}

QMimeData* SchemaModel::mimeData(const QModelIndexList &indexes) const
{
  DEBTRACE("SchemaModel::mimeData");
  ItemMimeData *mime = new ItemMimeData;
  mime->setSubject(_root);

  if (indexes.empty())
    return mime;
  QModelIndex index = indexes.first();
  if (!index.isValid())
    return mime;
  SchemaItem *item = static_cast<SchemaItem*>(index.internalPointer());
  DEBTRACE("mimeData valid index");
  return item->mimeData(mime);
}

bool SchemaModel::dropMimeData(const QMimeData* data, Qt::DropAction action,
                               int row, int column, const QModelIndex& parent)
{
  DEBTRACE("SchemaModel::dropMimeData");
  if (action == Qt::IgnoreAction)
    return true;

  string name = "empty";
  SchemaItem *item = 0;
  if (parent.isValid())
    {
      item = static_cast<SchemaItem*>(parent.internalPointer());
      name = item->getSubject()->getName();
    }
  DEBTRACE(row << " " << column << " "<< name);

  if ((row >= 0) && (column >=0))
    {
      QModelIndex ind = index(row, column, parent);
      if (!ind.isValid())
        return false;
      DEBTRACE("---");
      item = static_cast<SchemaItem*>(ind.internalPointer());
    }
  if (!item)
    return false;
  return item->dropMimeData(data, action);
}

Qt::DropActions SchemaModel::supportedDropActions() const
{
  //DEBTRACE("SchemaModel::supportedDropActions");
  return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
}

QStringList SchemaModel::mimeTypes() const
{
  //DEBTRACE("SchemaModel::mimeTypes");
  QStringList types;
  types << "yacs/subject" << "yacs/subjectNode" << "yacs/subjectOutPort"
        << "yacs/cataService" << "yacs/cataType" << "yacs/cataNode"
        << "yacs/subjectOutGate";
  return types;
}
