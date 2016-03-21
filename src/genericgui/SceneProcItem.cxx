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

#include "SceneProcItem.hxx"
#include "Menus.hxx"
#include "SceneLinkItem.hxx"
#include "QtGuiContext.hxx"
#include "Scene.hxx"
#include "Resource.hxx"


//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SceneProcItem::SceneProcItem(QGraphicsScene *scene, SceneItem *parent,
                             QString label, Subject *subject)
  : SceneBlocItem(scene, parent, label, subject)
{
  DEBTRACE("SceneProcItem::SceneProcItem " <<label.toStdString());
//   _height = 2000;
//   _width = 2000;
//   setGeometryOptimization(false);
}

SceneProcItem::~SceneProcItem()
{
}

void SceneProcItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  ProcMenu m;
  m.popupMenu(caller, globalPos);
}

