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
#include "SceneCtrlLinkItem.hxx"
#include "SceneCtrlPortItem.hxx"
#include "SceneElementaryNodeItem.hxx"
#include "SceneHeaderNodeItem.hxx"
#include "Scene.hxx"
#include "Menus.hxx"

#include <QPointF>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SceneCtrlLinkItem::SceneCtrlLinkItem(QGraphicsScene *scene, SceneItem *parent,
                                     ScenePortItem* from, ScenePortItem* to,
                                     QString label, Subject *subject)
  : SceneLinkItem(scene, parent, from, to, label, subject)
{
  _penColor     = QColor( 96,    0,  96);
  _hiPenColor   = QColor( 128,   0, 128);
  _brushColor   = QColor( 192,   0, 192);
  _hiBrushColor = QColor( 255, 192, 255);
}

SceneCtrlLinkItem::~SceneCtrlLinkItem()
{
}

QPointF SceneCtrlLinkItem::start()
{
  SceneCtrlPortItem* dpif = dynamic_cast<SceneCtrlPortItem*>(_from);
  QPointF localFrom(dpif->getWidth()*(9.5/10), dpif->getHeight()/2);
  DEBTRACE("localFrom(" << localFrom.x() << "," << localFrom.y() << ")");
  return mapFromItem(dpif, localFrom);
}

QPointF SceneCtrlLinkItem::goal()
{
  SceneCtrlPortItem* dpit = dynamic_cast<SceneCtrlPortItem*>(_to);
  QPointF localTo(dpit->getWidth()/20, dpit->getHeight()/2);
  DEBTRACE("localTo(" << localTo.x() << "," << localTo.y() << ")");
  return mapFromItem(dpit, localTo);
}
