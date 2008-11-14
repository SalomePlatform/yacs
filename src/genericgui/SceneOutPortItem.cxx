
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
