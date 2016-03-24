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

#include "YACSWidgets.hxx"
#include <QApplication>
#include <QClipboard>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::HMI;

YTableView::YTableView(QWidget *parent)
  : QTableView(parent)
{
}

bool YTableView::event(QEvent *e)
{
  if (e->type() == QEvent::ShortcutOverride)
    {
      e->accept();
      return true;
    }
  return QTableView::event(e);
}

QModelIndex YTableView::moveCursor(CursorAction cursorAction,Qt::KeyboardModifiers modifiers)
{
  QModelIndex ix = currentIndex();

  if (!ix.isValid()) 
    return QTableView::moveCursor(cursorAction,  modifiers);

  if(cursorAction==QAbstractItemView::MoveNext || cursorAction==QAbstractItemView::MovePrevious)
    {
      int row=ix.row();
      QModelIndex parent=ix.parent();
      int rowCount=model()->rowCount(parent);

      while(1)
        {
          if(cursorAction==QAbstractItemView::MoveNext)
            {
              row=row+1;
              if(row >= rowCount)
                row=0;
            }
          else
            {
              row=row-1;
              if(row < 0)
                row=rowCount -1;
            }
          if(!isRowHidden(row))
            break;
        }
      return model()->index(row, ix.column(), parent);
    }
  return QTableView::moveCursor(cursorAction,  modifiers);
}

