
#include "Scene.hxx"
#include "SceneItem.hxx"
#include "QtGuiContext.hxx"

#include <QGraphicsSceneMouseEvent>

#include <cassert>
#include <cmath>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

bool Scene::_autoComputeLinks = true;
bool Scene::_simplifyLinks = true;
bool Scene::_force2NodesLink = true;

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
