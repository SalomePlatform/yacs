
#include "SceneTextItem.hxx"

// #include "QtGuiContext.hxx"
// #include "Menus.hxx"
// #include <QGraphicsSceneHoverEvent>
#include <QPointF>

// #include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;


SceneTextItem::SceneTextItem(QGraphicsScene *scene, SceneItem *parent,
                             QString label)
  : QGraphicsTextItem(label, parent), AbstractSceneItem(scene, parent, label)
{
  setToolTip(label);
  DEBTRACE("SceneTextItem::SceneTextItem "<<label.toStdString()<<" "<<this<<" "<<_parent<<" "<< _level); 
  setPos(x()+10, y());
}

SceneTextItem::~SceneTextItem()
{
}

QRectF SceneTextItem::boundingRect() const
{
  return QGraphicsTextItem::boundingRect();
}
void SceneTextItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option,
           QWidget *widget)
{
  QGraphicsTextItem::paint(painter, option, widget);
}

void SceneTextItem::setTopLeft(QPointF topLeft)
{
}

void SceneTextItem::checkGeometryChange()
{
}

void SceneTextItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  if (_parent) _parent->popupMenu(caller, globalPos);
}
