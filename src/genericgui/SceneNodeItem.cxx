
#include "SceneNodeItem.hxx"
#include "SceneComposedNodeItem.hxx"
#include "SceneHeaderNodeItem.hxx"
#include "SceneInPortItem.hxx"
#include "SceneOutPortItem.hxx"
#include "SceneCtrlInPortItem.hxx"
#include "SceneCtrlOutPortItem.hxx"
#include "Scene.hxx"
#include "QtGuiContext.hxx"
#include "Menus.hxx"

#include <QGraphicsSceneHoverEvent>
#include <QPointF>

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SceneNodeItem::SceneNodeItem(QGraphicsScene *scene, SceneItem *parent,
                             QString label, Subject *subject)
  : SceneObserverItem(scene, parent, label, subject)
{
  _inPorts.clear();
  _outPorts.clear();
  _header = 0;
  _height = 50 + 2*_nml;
  _width = 2*ScenePortItem::getPortWidth() + 3*_margin + 2*_nml;
  _brushColor = QColor(0,0,128);
  _moving = false;
  _hasNml = true;
  _dragable = true;
  _dragButton = Qt::MidButton;
  _execState = YACS::UNDEFINED;
}

SceneNodeItem::~SceneNodeItem()
{
  if (SceneComposedNodeItem* parent = getParent())
    parent->removeChildFromList(this);
}

void SceneNodeItem::addHeader()
{
  if (!_hasHeader)
    {
      _hasHeader = true;
      _header = new SceneHeaderNodeItem(_scene,
                                        this,
                                        _label);
      updateState();
      QPointF topLeft(_margin + _nml, _margin + _nml);
      _header->setTopLeft(topLeft);
      checkGeometryChange();
    }
}

SceneHeaderItem* SceneNodeItem::getHeader()
{
  return _header;
}

void SceneNodeItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
  //DEBTRACE("SceneNodeItem::paint");
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

void SceneNodeItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SceneNodeItem::update "<< event<<" "<<type<<" "<<son);
  SubjectNode *snode = 0;
  Node *node = 0;
  switch (event)
    {
    case YACS::HMI::RENAME:
      DEBTRACE("SceneNodeItem::update RENAME " << _subject->getName());
      if (_header) _header->setText(_subject->getName().c_str());
      break;
    case YACS::HMI::EDIT:
      if (_header) _header->setEdited(type);
      break;
    case YACS::HMI::UPDATE:
      updateState();
      break;
    case YACS::HMI::UPDATEPROGRESS:
      setExecState(type);
      break;
    default:
      ;
    }
}

qreal SceneNodeItem::getHeaderBottom()
{
  qreal bottom = 0;
  if (_hasHeader)
    bottom = childBoundingRect(_header).bottom();
  return bottom;
}

void SceneNodeItem::autoPosNewPort(AbstractSceneItem *item, int nbPorts)
{
  SceneInPortItem* inPortItem = dynamic_cast<SceneInPortItem*>(item);
  bool isInPort = (inPortItem != 0);

  qreal xLeft = _margin + _nml;
  if (!isInPort) xLeft += ScenePortItem::getPortWidth() + _margin + _nml;

  qreal yTop  = getHeaderBottom() + _margin;
  yTop += nbPorts *(ScenePortItem::getPortHeight() + _margin);

  DEBTRACE("left, top " << xLeft  << " " << yTop);
  QPointF topLeft(xLeft, yTop);
  item->setTopLeft(topLeft);
}

void SceneNodeItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  NodeMenu m;
  m.popupMenu(caller, globalPos);
}

SceneComposedNodeItem* SceneNodeItem::getParent()
{
  if (_parent)
    return dynamic_cast<SceneComposedNodeItem*>(_parent);
  else
    return 0;
}
void SceneNodeItem::removeInPortFromList(AbstractSceneItem* inPort)
{
  _inPorts.remove(inPort);
}

void SceneNodeItem::removeOutPortFromList(AbstractSceneItem* outPort)
{
  _outPorts.remove(outPort);
}

void SceneNodeItem::setMoving(bool moving)
{
  //DEBTRACE("SceneNodeItem::setMoving " << _label.toStdString() << " " << moving);
  _moving = moving;
  SceneNodeItem *nodep = 0;
  if (_parent && (nodep = dynamic_cast<SceneNodeItem*>(_parent)))
    nodep->setMoving(false);
}

ScenePortItem* SceneNodeItem::getCtrlInPortItem()
{
  if (! _header) return 0;
  return _header->getCtrlInPortItem();
}

ScenePortItem* SceneNodeItem::getCtrlOutPortItem()
{
  if (! _header) return 0;
  return _header->getCtrlOutPortItem();
}


QString SceneNodeItem::getMimeFormat()
{
  return "yacs/subjectNode";
}

void SceneNodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  DEBTRACE("SceneNodeItem::mousePressEvent " << _label.toStdString());
  SceneObserverItem::mousePressEvent(event);
  if (!_scene->isZooming())
    {
      if (!_draging) setMoving(true);
      _prevPos = pos();
    }
}

void SceneNodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  //DEBTRACE("SceneNodeItem::mouseReleaseEvent " << _label.toStdString());
  SceneObserverItem::mouseReleaseEvent(event);
  setMoving(false);
  if ((pos() != _prevPos) && Scene::_autoComputeLinks)
    {
      YACS::HMI::SubjectProc* subproc = QtGuiContext::getQtCurrent()->getSubjectProc();
      SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[subproc];
      SceneComposedNodeItem *proc = dynamic_cast<SceneComposedNodeItem*>(item);
      proc->rebuildLinks();
    }
}

void SceneNodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  SceneObserverItem::mouseMoveEvent(event);
  if (_moving)
    {
      if (SceneComposedNodeItem *bloc = dynamic_cast<SceneComposedNodeItem*>(_parent))
        {
          QPointF oldPos = pos();
          QPointF aPos = oldPos + event->scenePos() - event->lastScenePos();
          if (aPos.x() < _margin + _nml)
            aPos.setX(_margin + _nml);
          if (aPos.y() < _margin + bloc->getHeaderBottom() + _nml)
            aPos.setY(_margin + bloc->getHeaderBottom() + _nml);
          setTopLeft(aPos);
          bloc->collisionResolv(this, oldPos);
        }
    }
}

void SceneNodeItem::setTopLeft(QPointF topLeft)
{
  QPointF oldPos = pos();
  setPos(topLeft);
  if (_parent)
    {
      if (SceneComposedNodeItem *bloc = dynamic_cast<SceneComposedNodeItem*>(_parent))
        bloc->collisionResolv(this, oldPos);
      _parent->checkGeometryChange();
    }
}

void SceneNodeItem::adjustHeader()
{
  if (_header) _header->adjustGeometry();
}

 void SceneNodeItem::updateState()
{
  SubjectNode *snode = dynamic_cast<SubjectNode*>(_subject);
  assert(snode);
  Node *node = snode->getNode();
  assert(node);
  switch (node->getState())
    {
    case YACS::INVALID:
      if (_header) _header->setValid(false);
      break;
    case YACS::READY:
      if (_header) _header->setValid(true);
      break;
    default:
      break;
    }
}

void SceneNodeItem::setExecState(int execState)
{
  DEBTRACE("SceneNodeItem::setExecState " << execState);
  _execState = execState;
  if (_header) _header->setExecState(execState);
}