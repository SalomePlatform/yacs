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

#ifndef _TREEVIEW_HXX_
#define _TREEVIEW_HXX_

#include <QTreeView>
#include <QContextMenuEvent>
#include <QAction>

namespace YACS
{
  namespace HMI
  {
    class ValueDelegate;

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
      virtual void onCommitData(QWidget *editor);

    protected:
      virtual bool event(QEvent *event);
      void contextMenuEvent(QContextMenuEvent *event);
      bool _isEdition;
      ValueDelegate* _valueDelegate;
    };
  }
}
#endif
