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

#include "SceneOutPortItem.hxx"
#include "SceneNodeItem.hxx"
#include "SceneItem.hxx"
#include "Menus.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SceneOutPortItem::SceneOutPortItem(QGraphicsScene *scene, SceneItem *parent,
                                 QString label, Subject *subject)
  : SceneDataPortItem(scene, parent, label, subject)
{
  _dragable = true;
}

SceneOutPortItem::~SceneOutPortItem()
{
  if (SceneNodeItem* parent = getParent())
    parent->removeOutPortFromList(this);
}

void SceneOutPortItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  OutPortMenu m;
  m.popupMenu(caller, globalPos);
}

QString SceneOutPortItem::getMimeFormat()
{
  return "yacs/subjectOutPort";
}
