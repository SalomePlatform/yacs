
#include "SceneDataPortItem.hxx"
#include "SceneTextItem.hxx"
#include "SceneNodeItem.hxx"
#include "Scene.hxx"

// #include "QtGuiContext.hxx"
// #include "Menus.hxx"
// #include <QGraphicsSceneHoverEvent>
// #include <QPointF>

// #include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;


SceneDataPortItem::SceneDataPortItem(QGraphicsScene *scene, SceneItem *parent,
                                     QString label, Subject *subject)
  : SceneObserverItem(scene, parent, label, subject), ScenePortItem(label)
{
  _width  = _portWidth;
  _height = _portHeight;
  setText(label);
  _brushColor   = QColor(158, 227, 151);
  _hiBrushColor = QColor(127, 227, 116);
  _penColor     = QColor( 15, 180,   0);
  _hiPenColor   = QColor( 11, 128,   0);
}

SceneDataPortItem::~SceneDataPortItem()
{
}

void SceneDataPortItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
  //DEBTRACE("ScenePortItem::paint");
  painter->save();
  painter->setPen(getPenColor());
  painter->setBrush(getBrushColor());
  painter->drawRoundRect(QRectF(0, 0, _width, _height), 33*_height/_width, 33);
  painter->restore();
}

void SceneDataPortItem::setText(QString label)
{
  if (!_text)
    _text = new SceneTextItem(_scene,
                              this,
                              label);
  else
    _text->setPlainText(label);
}

void SceneDataPortItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SceneDataPortItem::update "<< eventName(event)<<" "<<type<<" "<<son);
  switch (event)
    {
    case YACS::HMI::RENAME:
      _text->setPlainText(son->getName().c_str());
      break;
    case YACS::HMI::REMOVE:
      SceneObserverItem::update(event, type, son);
      break;
    }
}

SceneNodeItem* SceneDataPortItem::getParent()
{
  if (_parent)
    return dynamic_cast<SceneNodeItem*>(_parent);
  else
    return 0;
}

