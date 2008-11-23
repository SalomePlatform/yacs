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
  _penColor     = QColor( 96,   0,  96);
  _hiPenColor   = QColor( 128,  0, 128);
  _brushColor   = QColor( 192,  0, 192);
  _hiBrushColor = QColor( 255,  0, 255);
}

SceneCtrlLinkItem::~SceneCtrlLinkItem()
{
}

QPointF SceneCtrlLinkItem::start()
{
  SceneCtrlPortItem* dpif = dynamic_cast<SceneCtrlPortItem*>(_from);
  QPointF localFrom(dpif->getWidth()*(9.5/10), dpif->getHeight()/2);
  return mapFromItem(dpif, localFrom);
}

QPointF SceneCtrlLinkItem::goal()
{
  SceneCtrlPortItem* dpit = dynamic_cast<SceneCtrlPortItem*>(_to);
  QPointF localTo(dpit->getWidth()/20, dpit->getHeight()/2);
  return mapFromItem(dpit, localTo);
}
