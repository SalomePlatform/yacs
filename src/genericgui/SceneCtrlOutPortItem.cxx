
#include "SceneCtrlOutPortItem.hxx"

// #include "QtGuiContext.hxx"
#include "Menus.hxx"
// #include <QGraphicsSceneHoverEvent>
// #include <QPointF>

// #include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SceneCtrlOutPortItem::SceneCtrlOutPortItem(QGraphicsScene *scene, SceneItem *parent,
                                           QString label)
  : SceneCtrlPortItem(scene, parent, label)
{
  _dragable = true;
}

SceneCtrlOutPortItem::~SceneCtrlOutPortItem()
{
}

void SceneCtrlOutPortItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  CtrlOutPortMenu m;
  m.popupMenu(caller, globalPos);
}

QString SceneCtrlOutPortItem::getMimeFormat()
{
  return "yacs/subjectOutGate";
}
