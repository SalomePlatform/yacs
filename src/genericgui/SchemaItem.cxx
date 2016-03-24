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

#include "SchemaItem.hxx"
#include "QtGuiContext.hxx"
#include "Resource.hxx"
#include "Menus.hxx"
#include "ItemMimeData.hxx"

#include <QMetaType>
#include <QVariant>
#include <QMenu>

Q_DECLARE_METATYPE(YACS::HMI::Subject);

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;


SchemaItem::SchemaItem(SchemaItem *parent, QString label, Subject* subject)
{
  DEBTRACE("SchemaItem::SchemaItem " << label.toStdString() << " " << this);
  _parentItem = parent;
  _label = label;
  _subject = subject;

  _itemData << QVariant() << QVariant() << QVariant(); // --- 3 columns max
  _itemDeco << QVariant() << QVariant() << QVariant(); // --- 3 columns max
  _itemForeground << QVariant() << QVariant() << QVariant(); // --- 3 columns max
  _itemBackground << QVariant() << QVariant() << QVariant(); // --- 3 columns max
  _itemCheckState << QVariant() << QVariant() << QVariant(); // --- 3 columns max
  _itemToolTip    << QVariant() << QVariant() << QVariant(); // --- 3 columns max
  _itemWhatsThis  << QVariant() << QVariant() << QVariant(); // --- 3 columns max
  _itemData.replace(YLabel, label);
  _itemToolTip.replace(YLabel, label);
  _itemWhatsThis.replace(YLabel, QString("This is the default WhatsThis of ") + label);

  _itemForeground.replace(YLabel, QColor("blue"));
  _itemBackground.replace(YLabel, QtGuiContext::getQtCurrent()->getSchemaModel()->stdBackBrush());
  //_itemCheckState.replace(YLabel, Qt::Unchecked); // --- only for item with checkbox

  if (_subject)
    {
      _subject->attach(this);
      QtGuiContext::getQtCurrent()->_mapOfSchemaItem[_subject]=this;
    }

  if (_parentItem)
    _parentItem->appendChild(this);
  _execState = YACS::UNDEFINED;
  _emphasized = false;
}

SchemaItem::~SchemaItem()
{
  DEBTRACE("SchemaItem::~SchemaItem " << _label.toStdString() << " " << this);
  if (_parentItem)
    {
      SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
      int position = row();
      model->beginRemoveRows(_parentItem->modelIndex(), position, position);
      if (_subject) QtGuiContext::getQtCurrent()->_mapOfSchemaItem.erase(_subject);
      _parentItem->removeChild(this);
      model->endRemoveRows();
    }
}

void SchemaItem::appendChild(SchemaItem *child)
{
  DEBTRACE("SchemaItem::appendChild");
  _childItems.append(child);
}

void SchemaItem::removeChild(SchemaItem *child)
{
  DEBTRACE("SchemaItem::removeChild");
  _childItems.removeAll(child);
}

void SchemaItem::insertChild(int row, SchemaItem *child)
{
  DEBTRACE("SchemaItem::insertChild");
  _childItems.insert(row, child);
}

SchemaItem *SchemaItem::child(int row)
{
  //DEBTRACE("SchemaItem::child");
  return _childItems.value(row);
}

int SchemaItem::childCount() const
{
  //DEBTRACE("SchemaItem::childCount " << _label.toStdString() << " " << _childItems.count());
  return _childItems.count();
}

int SchemaItem::columnCount() const
{
  //DEBTRACE("SchemaItem::columnCount " << _itemData.count());
  return _itemData.count();
}

QVariant SchemaItem::data(int column, int role) const
{
  //DEBTRACE("SchemaItem::data "<< column);
  if (role == Qt::DisplayRole)
    return _itemData.value(column);
  if (role == Qt::DecorationRole)
    return _itemDeco.value(column);
  if (role == Qt::ForegroundRole)
    return _itemForeground.value(column);
  if (role == Qt::BackgroundRole)
    return _itemBackground.value(column);
  if (role == Qt::CheckStateRole)
    return _itemCheckState.value(column);
  if (role == Qt::ToolTipRole)
    if (QtGuiContext::getQtCurrent()->isEdition())
      return editionToolTip(column);
    else
      return runToolTip(column);
  if (role == Qt::WhatsThisRole)
    if (QtGuiContext::getQtCurrent()->isEdition())
      return editionWhatsThis(column);
    else
      return runWhatsThis(column);
  return QVariant();
}

Qt::ItemFlags SchemaItem::flags(const QModelIndex &index)
{
  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;// | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

int SchemaItem::row() const
{
  //DEBTRACE("SchemaItem::row");
  int row=0;
  if (_parentItem)
    row = _parentItem->_childItems.indexOf(const_cast<SchemaItem*>(this));
  return row;
}

SchemaItem *SchemaItem::parent()
{
  //DEBTRACE("SchemaItem::parent");
  return _parentItem;
}

Subject* SchemaItem::getSubject()
{
  return _subject;
}

void SchemaItem::update(GuiEvent event, int type, Subject* son)
{
  //DEBTRACE("SchemaItem::update "<< eventName(event) <<" "<<type<<" "<<son);
  QModelIndex index = QModelIndex();
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  switch (event)
    {
    case RENAME:
      DEBTRACE("SchemaItem::update RENAME " << _subject->getName());
      _label = _subject->getName().c_str();
      _itemData.replace(YLabel, _label);
      model->setData(modelIndex(YLabel), 0);  // --- to emit dataChanged signal
      break;
    case EDIT:
      if (type)
        _itemBackground.replace(YLabel, model->editedBackBrush());
      else
        _itemBackground.replace(YLabel, model->stdBackBrush());
      model->setData(modelIndex(YLabel), 0);  // --- to emit dataChanged signal
      break;
    case EMPHASIZE:
      if (type)
        _itemBackground.replace(YLabel, model->emphasizeBackBrush());
      else
        _itemBackground.replace(YLabel, model->stdBackBrush());
      model->setData(modelIndex(YLabel), 0);  // --- to emit dataChanged signal
      break;
    default:
      break;
    }
}

void SchemaItem::select(bool isSelected)
{
  DEBTRACE("SchemaItem::select " << _label.toStdString() << " " << isSelected);
  QItemSelectionModel* selectionModel = QtGuiContext::getQtCurrent()->getSelectionModel();
  QModelIndex anIndex = modelIndex();
  QItemSelection newSelection(anIndex, anIndex);

  if (isSelected)
    {
      if (!QtGuiContext::getQtCurrent()->_mapOfEditionItem.count(_subject))
        {
          int elemType = _subject->getType();
          YACS::HMI::GuiEvent event = YACS::HMI::ADD;
          if (elemType == YACS::HMI::DATALINK) event = YACS::HMI::ADDLINK;
          else if (elemType == YACS::HMI::CONTROLLINK) event = YACS::HMI::ADDCONTROLLINK;
          QtGuiContext::getQtCurrent()->getEditionRoot()->update(event, elemType, _subject);
        }

      QtGuiContext::getQtCurrent()->getGMain()->raiseStacked();
      QItemSelection currentSelected = selectionModel->selection();
      if (currentSelected != newSelection)
        {
          DEBTRACE("currentSelected != newSelection");
          // selectionModel->select(newSelection, QItemSelectionModel::ClearAndSelect);
          selectionModel->select(newSelection, QItemSelectionModel::Clear);
          selectionModel->select(newSelection, QItemSelectionModel::Select);
        }
      QtGuiContext::getQtCurrent()->setSelectedSubject(_subject);
    }
  else
    selectionModel->select(newSelection, QItemSelectionModel::Deselect);
}

void SchemaItem::toggleState()
{
  if (_itemCheckState.value(YLabel) == Qt::Unchecked)
    {
      DEBTRACE("SchemaItem::toggleState true");
      _itemCheckState.replace(YLabel, Qt::Checked);
    }
  else
    {
      DEBTRACE("SchemaItem::toggleState false");
      _itemCheckState.replace(YLabel, Qt::Unchecked);
    }
}

QModelIndex SchemaItem::modelIndex(int column)
{
  //DEBTRACE("SchemaItem::modelIndex " << _label.toStdString() << " " << column);
  SchemaModel *schema = QtGuiContext::getQtCurrent()->getSchemaModel();
  if (_parentItem && (_parentItem !=schema->getRootItem()))
    return schema->index(row(),
                         column,
                         _parentItem->modelIndex());
  else
    return schema->index(row(),
                         column,
                         QModelIndex());
}

void SchemaItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  MenusBase m;
  m.popupMenu(caller, globalPos);
}

/*!
 * setData mime type must be coherent with SchemaModel::mimeTypes
 */
ItemMimeData* SchemaItem::mimeData(ItemMimeData *mime)
{
  DEBTRACE("SchemaItem::mimeData");
  mime->setSubject(_subject);
  mime->setData(getMimeFormat(), "_subject");
  return mime;
}

bool SchemaItem::dropMimeData(const QMimeData* data, Qt::DropAction action)
{
  return false;
}

void SchemaItem::reparent(SchemaItem *parent)
{
  _parentItem = parent;
  if (_parentItem)
    _parentItem->appendChild(this);
}

//! used in node derived classes
void SchemaItem::setCaseValue()
{
}

QVariant SchemaItem::editionToolTip(int column) const
{
  QString val = QString("Edition: ") + _itemData.value(0).toString();
  QString val1 = _itemData.value(1).toString();
  QString val2 = _itemData.value(2).toString();
  if (!val1.isEmpty()) val += QString(" | ") + val1;
  if (!val2.isEmpty()) val += QString(" | ") + val2;
  return val;
}

QVariant SchemaItem::runToolTip(int column) const
{
  QString val = QString("Execution: ") + _itemData.value(0).toString();
  QString val1 = _itemData.value(1).toString();
  QString val2 = _itemData.value(2).toString();
  if (!val1.isEmpty()) val += QString(" | ") + val1;
  if (!val2.isEmpty()) val += QString(" | ") + val2;
  return val;
}

QVariant SchemaItem::editionWhatsThis(int column) const
{
  QString val = QString("Edition help: ") + _itemWhatsThis.value(column).toString();
  return val;
}

QVariant SchemaItem::runWhatsThis(int column) const
{
  QString val = QString("Execution help: ") + _itemWhatsThis.value(column).toString();
  return val;
}

QString SchemaItem::getMimeFormat()
{
  return "yacs/subject";
}

void SchemaItem::setExecState(int execState)
{
  DEBTRACE("SchemaItem::setExecState " << execState);
  _execState = execState;
  QString stateDef;
  QColor sc;
  switch (_execState)
    {
      case YACS::UNDEFINED:    sc = Resource::UNDEFINED   ; stateDef = "UNDEFINED"   ; break;
      case YACS::INVALID:      sc = Resource::INVALID     ; stateDef = "INVALID"     ; break;
      case YACS::READY:        sc = Resource::READY       ; stateDef = "READY"       ; break;
      case YACS::TOLOAD:       sc = Resource::TOLOAD      ; stateDef = "TOLOAD"      ; break;
      case YACS::LOADED:       sc = Resource::LOADED      ; stateDef = "LOADED"      ; break;
      case YACS::TOACTIVATE:   sc = Resource::TOACTIVATE  ; stateDef = "TOACTIVATE"  ; break;
      case YACS::ACTIVATED:    sc = Resource::ACTIVATED   ; stateDef = "ACTIVATED"   ; break;
      case YACS::DESACTIVATED: sc = Resource::DESACTIVATED; stateDef = "DESACTIVATED"; break;
      case YACS::DONE:         sc = Resource::DONE        ; stateDef = "DONE"        ; break;
      case YACS::SUSPENDED:    sc = Resource::SUSPENDED   ; stateDef = "SUSPENDED"   ; break;
      case YACS::LOADFAILED:   sc = Resource::LOADFAILED  ; stateDef = "LOADFAILED"  ; break;
      case YACS::EXECFAILED:   sc = Resource::EXECFAILED  ; stateDef = "EXECFAILED"  ; break;
      case YACS::PAUSE:        sc = Resource::PAUSE       ; stateDef = "PAUSE"       ; break;
      case YACS::INTERNALERR:  sc = Resource::INTERNALERR ; stateDef = "INTERNALERR" ; break;
      case YACS::DISABLED:     sc = Resource::DISABLED    ; stateDef = "DISABLED"    ; break;
      case YACS::FAILED:       sc = Resource::FAILED      ; stateDef = "FAILED"      ; break;
      case YACS::ERROR:        sc = Resource::ERROR       ; stateDef = "ERROR"       ; break;
      default:                 sc = Resource::DEFAULT     ; stateDef = "---"         ;
   }
  _itemData.replace(YState, stateDef);
  _itemForeground.replace(YState, sc);
}
