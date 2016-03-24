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

#include "Scene.hxx"
#include "SceneItem.hxx"
#include "SceneTextItem.hxx"
#include "QtGuiContext.hxx"

#include <QGraphicsSceneMouseEvent>
#include <QToolTip>

#include <cassert>
#include <cmath>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

bool Scene::_straightLinks = false;
bool Scene::_autoComputeLinks = true;
bool Scene::_simplifyLinks = true;
bool Scene::_force2NodesLink = true;
bool Scene::_addRowCols = true;

Scene::Scene(QObject *parent): QGraphicsScene(parent)
{
  _zooming = false;
}

Scene::~Scene()
{
}

/*! to notify scene when mouse actions must not be used for selection
 *  or node displacement, typically when mouse is used to adjust the view,
 *  like zoom or recenter of the view.
 */
void Scene::setZoom(bool zooming)
{
  _zooming = zooming;
}

/*! check if mouse move event can be used for selection or node
 *  displacement. @see setZoom()
 */
bool Scene::isZooming()
{
  return _zooming;
}
 
void Scene::helpEvent(QGraphicsSceneHelpEvent *event)
{
  DEBTRACE("Scene::helpEvent");
  QGraphicsItem *qit = itemAt(event->scenePos(), QTransform());
  SceneItem * item = dynamic_cast<SceneItem*>(qit);
  if (item)
    {
      QToolTip::showText(event->screenPos(), item->getToolTip());
      return;
    }
  SceneTextItem * itemt = dynamic_cast<SceneTextItem*>(qit);
  if (itemt)
    {
      QToolTip::showText(event->screenPos(), itemt->getToolTip());
      return;
    }
  QToolTip::hideText();
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
  //DEBTRACE("Scene::mousePressEvent");
  QGraphicsScene::mousePressEvent(mouseEvent);
  QGraphicsItem *qit  = mouseGrabberItem();
  if (qit)
    {
      //DEBTRACE(qit);
      SceneItem *item = dynamic_cast<SceneItem*>(qit);
      if (item)
        DEBTRACE("mouseGrabberItem " <<item->handlesChildEvents()
                 << " " << item->getLabel().toStdString());
    }
//   QList<QGraphicsItem*> selItems = items(mouseEvent->scenePos());
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
  _zooming = false;
  QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
  //QGraphicsScene::mouseMoveEvent(mouseEvent);
  QGraphicsScene::mouseMoveEvent(mouseEvent);
   
}
