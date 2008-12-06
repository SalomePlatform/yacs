//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef _TREEVIEW_HXX_
#define _TREEVIEW_HXX_

#include <QTreeView>
#include <QContextMenuEvent>
#include <QAction>

namespace YACS
{
  namespace HMI
  {
    class TreeView: public QTreeView
    {
      Q_OBJECT

    public:
      TreeView(QWidget *parent = 0);
      virtual ~TreeView();
      virtual void setModel(QAbstractItemModel *model);

    public slots:
      void viewSelection(const QModelIndex &ind);
      void resizeColumns();

    protected:
      virtual bool event(QEvent *event);
      void contextMenuEvent(QContextMenuEvent *event);
      bool _isEdition;

//       virtual void mousePressEvent(QMouseEvent *event);
//       virtual void mouseMoveEvent(QMouseEvent *event);
//       virtual void mouseReleaseEvent(QMouseEvent *event);

//       void dragEnterEvent(QDragEnterEvent *event);
//       void dragLeaveEvent(QDragLeaveEvent *event);
//       void dragMoveEvent(QDragMoveEvent *event);
//       void dropEvent(QDropEvent *event);

    };
  }
}
#endif
