

#include "SchemaItem.hxx"
#include "QtGuiContext.hxx"
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
  _itemData << QVariant() << QVariant() << QVariant(); // --- 3 columns max
  _itemDeco << QVariant() << QVariant() << QVariant(); // --- 3 columns max
  _itemForeground << QVariant() << QVariant() << QVariant(); // --- 3 columns max
  _itemBackground << QVariant() << QVariant() << QVariant(); // --- 3 columns max
  _itemCheckState << QVariant() << QVariant() << QVariant(); // --- 3 columns max
  _itemData.replace(YLabel, label);
  _itemForeground.replace(YLabel, QColor("blue"));
  _itemBackground.replace(YLabel, QtGuiContext::getQtCurrent()->getSchemaModel()->stdBackBrush());
  //_itemCheckState.replace(YLabel, Qt::Unchecked); // --- only for item with checkbox
  _subject = subject;
  if (_subject)
    {
      _subject->attach(this);
      QtGuiContext::getQtCurrent()->_mapOfSchemaItem[_subject]=this;
    }
  if (_parentItem)
    _parentItem->appendChild(this);
  _execState = YACS::UNDEFINED;
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
  //DEBTRACE("SchemaItem::update "<<event<<" "<<type<<" "<<son);
  QModelIndex index = QModelIndex();
  SchemaModel *model = QtGuiContext::getQtCurrent()->getSchemaModel();
  switch (event)
    {
    case RENAME:
      DEBTRACE("SchemaItem::update RENAME " << _subject->getName());
      _label = _subject->getName().c_str();
      _itemData.replace(YLabel, _label);
      model->setData(modelIndex(), 0);  // --- to emit dataChanged signal
      break;
    case EDIT:
      if (type)
        _itemBackground.replace(YLabel, model->editedBackBrush());
      else
        _itemBackground.replace(YLabel, model->stdBackBrush());
      model->setData(modelIndex(), 0);  // --- to emit dataChanged signal
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
    case YACS::UNDEFINED:    sc=Qt::lightGray;       stateDef = "UNDEFINED";     break;
    case YACS::INVALID:      sc=Qt::red;             stateDef = "INVALID";       break;
    case YACS::READY:        sc=Qt::darkGray;        stateDef = "READY";         break;
    case YACS::TOLOAD:       sc=Qt::darkYellow;      stateDef = "TOLOAD";        break;
    case YACS::LOADED:       sc=Qt::darkMagenta;     stateDef = "LOADED";        break;
    case YACS::TOACTIVATE:   sc=Qt::darkCyan;        stateDef = "TOACTIVATE";    break;
    case YACS::ACTIVATED:    sc=Qt::darkBlue;        stateDef = "ACTIVATED";     break;
    case YACS::DESACTIVATED: sc=Qt::gray;            stateDef = "DESACTIVATED";  break;
    case YACS::DONE:         sc=Qt::darkGreen;       stateDef = "DONE";          break;
    case YACS::SUSPENDED:    sc=Qt::gray;            stateDef = "SUSPENDED";     break;
    case YACS::LOADFAILED:   sc.setHsv(320,255,255); stateDef = "LOADFAILED";    break;
    case YACS::EXECFAILED:   sc.setHsv( 20,255,255); stateDef = "EXECFAILED";    break;
    case YACS::PAUSE:        sc.setHsv(180,255,255); stateDef = "PAUSE";         break;
    case YACS::INTERNALERR:  sc.setHsv(340,255,255); stateDef = "INTERNALERR";   break;
    case YACS::DISABLED:     sc.setHsv( 40,255,255); stateDef = "DISABLED";      break;
    case YACS::FAILED:       sc.setHsv( 20,255,255); stateDef = "FAILED";        break;
    case YACS::ERROR:        sc.setHsv(  0,255,255); stateDef = "ERROR";         break;
    default:                 sc=Qt::lightGray;       stateDef = "---";
   }
  _itemData.replace(YState, stateDef);
  _itemForeground.replace(YState, sc);
}
