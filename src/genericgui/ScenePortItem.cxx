
#include "ScenePortItem.hxx"

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

const int ScenePortItem::_portWidth  = 100;
const int ScenePortItem::_portHeight = 25;

ScenePortItem::ScenePortItem(QString label)
{
  _text = 0;
}

ScenePortItem::~ScenePortItem()
{
}

int ScenePortItem::getPortWidth()
{
  return _portWidth;
}

int ScenePortItem::getPortHeight()
{
  return _portHeight;
}

