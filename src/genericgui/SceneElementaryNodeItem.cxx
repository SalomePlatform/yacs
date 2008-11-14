
#include "SceneElementaryNodeItem.hxx"
#include "SceneInPortItem.hxx"
#include "SceneOutPortItem.hxx"
#include "commandsProc.hxx"
#include "Scene.hxx"

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

SceneElementaryNodeItem::SceneElementaryNodeItem(QGraphicsScene *scene, SceneItem *parent,
                                             QString label, Subject *subject)
  : SceneNodeItem(scene, parent, label, subject)
{
  _brushColor   = QColor(189,230,185);
  _hiBrushColor = QColor(209,255,205);
  _penColor     = QColor( 15,180,  0);
  _hiPenColor   = QColor( 11,128,  0);
}

SceneElementaryNodeItem::~SceneElementaryNodeItem()
{
}

void SceneElementaryNodeItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
  //DEBTRACE("SceneElementaryNodeItem::paint");
  painter->save();
  painter->setBrush(QBrush(Qt::NoBrush));
  painter->setPen(QPen(Qt::NoPen));
  painter->drawRect(QRectF(0, 0, _width, _height));
  painter->setPen(getPenColor());
  painter->setBrush(getBrushColor());
  painter->drawRect(QRectF(_nml, _nml,
                           _width-2*_nml, _height-2*_nml));
  painter->restore();
}

void SceneElementaryNodeItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SceneElementaryNodeItem::update "<< event<<" "<<type<<" "<<son);
  SceneNodeItem::update(event, type, son);
  SceneItem *item;
  switch (event)
    {
    case YACS::HMI::ADD:
      switch (type)
        {
        case YACS::HMI::INPUTPORT:
        case YACS::HMI::INPUTDATASTREAMPORT:
          item =  new SceneInPortItem(_scene,
                                      this,
                                      son->getName().c_str(),
                                      son);
          autoPosNewPort(item, _inPorts.size());
          _inPorts.push_back(item);
          break;
        case YACS::HMI::OUTPUTPORT:
        case YACS::HMI::OUTPUTDATASTREAMPORT:
          item =  new SceneOutPortItem(_scene,
                                       this,
                                       son->getName().c_str(),
                                       son);
          autoPosNewPort(item, _outPorts.size());
          _outPorts.push_back(item);
           break;
        default:
          DEBTRACE("SceneElementaryNodeItem::update() ADD, type not handled:" << type);
        }
      break;
    case YACS::HMI::REMOVE:
      //SceneObserverItem::update(event, type, son);
      reorganize();
      break;
    default:
      DEBTRACE("SceneElementaryNodeItem::update(), event not handled: << event");
    }
}

void SceneElementaryNodeItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  ElementaryNodeMenu m;
  m.popupMenu(caller, globalPos);
}

void SceneElementaryNodeItem::reorganize()
{
  DEBTRACE("SceneElementaryNodeItem::reorganize() " << _label.toStdString());
  int nbPorts = 0;
  for (list<AbstractSceneItem*>::const_iterator it=_inPorts.begin(); it!=_inPorts.end(); ++it)
    {
      autoPosNewPort(*it, nbPorts);
      nbPorts++;
    }
  nbPorts = 0;
  for (list<AbstractSceneItem*>::const_iterator it=_outPorts.begin(); it!=_outPorts.end(); ++it)
    {
      autoPosNewPort(*it, nbPorts);
      nbPorts++;
    }
}

