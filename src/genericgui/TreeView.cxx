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

#include "TreeView.hxx"
#include "SchemaItem.hxx"
#include "QtGuiContext.hxx"
#include "ValueDelegate.hxx"

#include "Port.hxx"
#include "DataPort.hxx"
#include "TypeCode.hxx"

#include <QMenu>
#include <QHeaderView>
#include <QToolTip>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <cassert>

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

  _valueDelegate = new ValueDelegate(parent);

  connect(_valueDelegate, SIGNAL(commitData(QWidget*)),
          this, SLOT(onCommitData(QWidget*)));

  setItemDelegateForColumn(YLabel, _valueDelegate); // --- port label
  setItemDelegateForColumn(YValue, _valueDelegate); // --- port value
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
  QModelIndex ind0 = ind.sibling(ind.row(), 0);
  //DEBTRACE("TreeView::viewSelection " << ind.row() << " " << ind.column() << " / " << ind0.row() << " " << ind0.column());
  scrollTo(ind0);
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

bool TreeView::event(QEvent *event)
{
  if (event->type() == QEvent::WhatsThisClicked)
    {
      QWhatsThisClickedEvent* clicked = static_cast<QWhatsThisClickedEvent*>(event);
      QtGuiContext::getQtCurrent()->getGMain()->onHelpContextModule("YACS",clicked->href());
      return true; // what's this remains open if true is returned
    }

  if (event->type() == QEvent::ToolTip)
    {
      QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
      QModelIndex index = indexAt(helpEvent->pos());
      if (index.isValid())
        {
          QString valtip = model()->data(index, Qt::ToolTipRole).toString();
          QToolTip::showText(helpEvent->globalPos(), valtip);
        }
      else
        QToolTip::hideText();
    }
  return QTreeView::event(event);
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

/*!
 *  After edition with a specific editor created by ValueDelegate
 *  for a cell of Tree item, the resulting string is tested
 *  for setValue on the corresponding subject. Result of the setValue
 *  (succes or failure) is transmitted to ValueDelegate for further
 *  action in case of failure.
 */
void TreeView::onCommitData(QWidget *editor)
{
  DEBTRACE("TreeView::onCommitData " << editor);
  GenericEditor* gedit = dynamic_cast<GenericEditor*>(editor);
  YASSERT(gedit);
  QString val = gedit->GetStrValue();
  string strval = val.toStdString();
  DEBTRACE(strval);
  bool isOk = false;

  Subject *sub = gedit->getSubject();
  YASSERT(sub);
  SubjectDataPort *sdp = dynamic_cast<SubjectDataPort*>(sub);
  if (sdp)
    {
      if (gedit->getColumnInSubject() == YValue)
        {
          if (sdp->getPort()->edGetType()->kind() == YACS::ENGINE::String)
            strval = "\"" + strval + "\"";
          DEBTRACE(strval);
          isOk = sdp->setValue(strval);

          GuiExecutor* executor = QtGuiContext::getQtCurrent()->getGuiExecutor();
          if (executor) executor->setInPortValue(sdp->getPort(), strval);
        }
      else // --- YLabel
        {
          isOk = sdp->setName(strval);
        }
    }
  else
    {
      SubjectNode *snode = dynamic_cast<SubjectNode*>(sub);
      YASSERT(snode);
      sub = snode->getParent();
      SubjectSwitch *sswitch = dynamic_cast<SubjectSwitch*>(sub);
      YASSERT(sswitch);
      isOk = sswitch->setCase(strval, snode);
    }

  if (_valueDelegate)
    _valueDelegate->setResultEditing(editor, isOk);
 }
