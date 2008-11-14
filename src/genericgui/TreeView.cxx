
#include "TreeView.hxx"
#include "SchemaItem.hxx"
#include "QtGuiContext.hxx"

#include <QMenu>
#include <QHeaderView>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

TreeView::TreeView(QWidget *parent)
  : QTreeView(parent)
{
  setDragDropMode(QAbstractItemView::DragDrop);
  setDragEnabled(true);
  setAcceptDrops(true);
  setDropIndicatorShown(true);
  _isEdition = true;
}

TreeView::~TreeView()
{
}

void TreeView::setModel(QAbstractItemModel *model)
{
  QTreeView::setModel(model);
  _isEdition = QtGuiContext::getQtCurrent()->isEdition();
  DEBTRACE("_isEdition=" << _isEdition);
}

void TreeView::viewSelection(const QModelIndex &ind)
{
  scrollTo(ind);
}

void TreeView::resizeColumns()
{
  Subject *sproc = QtGuiContext::getQtCurrent()->getSubjectProc();
  SchemaItem *item = QtGuiContext::getQtCurrent()->_mapOfSchemaItem[sproc];
  QModelIndex index = item->modelIndex();
  setExpanded(index, true);
  resizeColumnToContents(0);
  if (_isEdition)
    {
      setColumnHidden(YType,  false);
      setColumnHidden(YValue, false);
      setColumnWidth(YType,  100);
      setColumnWidth(YValue, 100);
    }
  else
    {
      setColumnHidden(YType,  true);
      setColumnHidden(YState, false);
      setColumnWidth(YState, 100);
    }
}

void TreeView::contextMenuEvent(QContextMenuEvent *event)
{
  QModelIndexList selList = selectedIndexes();
  if (selList.isEmpty())
    return;
  QModelIndex selected = selList.front();
  if (selected.isValid())
    {
      SchemaItem* item = static_cast<SchemaItem*>(selected.internalPointer());
      item->popupMenu(this, event->globalPos());
    }
}

// void TreeView::mousePressEvent(QMouseEvent *event)
// {
//   QTreeView::mousePressEvent(event);
// }

// void TreeView::mouseMoveEvent(QMouseEvent *event)
// {
//   QTreeView::mouseMoveEvent(event);
// }

// void TreeView::mouseReleaseEvent(QMouseEvent *event)
// {
//   QTreeView::mouseReleaseEvent(event);
// }

// void TreeView::dragEnterEvent(QDragEnterEvent *event)
// {
//   DEBTRACE("TreeView::dragEnterEvent");
//   QTreeView::dragEnterEvent(event);
// }

// void TreeView::dragLeaveEvent(QDragLeaveEvent *event)
// {
//   DEBTRACE("TreeView::dragLeaveEvent");
//   QTreeView::dragLeaveEvent(event);
// }

// void TreeView::dragMoveEvent(QDragMoveEvent *event)
// {
//   DEBTRACE("TreeView::dragMoveEvent");
//   QTreeView::dragMoveEvent(event);
// }

// void TreeView::dropEvent(QDropEvent *event)
// {
//   DEBTRACE("TreeView::dropEvent");
//   QTreeView::dropEvent(event);
// }
