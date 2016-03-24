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

#include "FormUndoRedo.hxx"
#include "GraphicsView.hxx"
#include "QtGuiContext.hxx"
#include "commands.hxx"

#include <string>
#include <list>

#include <QTreeWidget>
#include <QList>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

FormUndoRedo::FormUndoRedo(QWidget *parent)
{
  setupUi(this);

  Invocator *procInvoc = GuiContext::getCurrent()->getInvoc();
  list<string> commandsDone = procInvoc->getDone();
  list<string> commandsUndone = procInvoc->getUndone();

  QList<QTreeWidgetItem *> itemsDone;
  list<string>::iterator it;
  for(it = commandsDone.begin(); it != commandsDone.end(); ++it)
    {
      (*it).erase((*it).rfind("\n"));
      QTreeWidgetItem *commItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString((*it).c_str())));
      itemsDone.append(commItem);
    }
  treeWdUndo->insertTopLevelItems(0, itemsDone);

  QList<QTreeWidgetItem *> itemsUndone;
  for(it = commandsUndone.begin(); it != commandsUndone.end(); ++it)
    {
      (*it).erase((*it).rfind("\n"));
      QTreeWidgetItem *commItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString((*it).c_str())));
      itemsUndone.append(commItem);
    }
  treeWdRedo->insertTopLevelItems(0, itemsUndone);
}

FormUndoRedo::~FormUndoRedo()
{
}
//  QTreeWidget *treeWidget = new QTreeWidget();
//  treeWidget->setColumnCount(1);
//  QList<QTreeWidgetItem *> items;
//  for (int i = 0; i < 10; ++i)
//      items.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("item: %1").arg(i))));
//  treeWidget->insertTopLevelItems(0, items);

