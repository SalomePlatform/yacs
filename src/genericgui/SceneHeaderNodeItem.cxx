//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "SceneHeaderNodeItem.hxx"
#include "SceneComposedNodeItem.hxx"
#include "SceneProcItem.hxx"
#include "SceneCtrlInPortItem.hxx"
#include "SceneCtrlOutPortItem.hxx"
#include "SceneInPortItem.hxx"
#include "SceneOutPortItem.hxx"
#include "Scene.hxx"

#include "QtGuiContext.hxx"
#include "Resource.hxx"
#include "Menus.hxx"
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SceneHeaderNodeItem::SceneHeaderNodeItem(QGraphicsScene *scene, SceneItem *parent,
                                         QString label)
  : SceneHeaderItem(scene, parent, label)
{
  _header = 0;
  _controlIn = 0;
  _controlOut = 0;
  _label = "H_" + _label;

  _header = new SceneHeaderItem(_scene,
                                this,
                                "header_"+_label);
  _header->setText(label);

  QPointF topLeft(_margin, _margin);
  _header->setTopLeft(topLeft);
  _hasHeader = true;
  _controlIn  = new SceneCtrlInPortItem(_scene,
                                        this,
                                        "Ctrl_I");
  autoPosControl(_controlIn);
  _controlOut = new SceneCtrlOutPortItem(_scene,
                                         this,
                                         "Ctrl_O");
  autoPosControl(_controlOut);
  if (QtGuiContext::getQtCurrent()->isEdition())
    _brushColor = Resource::normalNodeBrushColor;
  else
    _brushColor = Resource::runNodeBrushColor;
  _execState = YACS::UNDEFINED;
  _sc = Resource::validNodeColor;
  _stateDef = "";
  _isProc = dynamic_cast<SceneProcItem*>(_parent);
  _isValid = true;
  _isEdited = false;
}

SceneHeaderNodeItem::~SceneHeaderNodeItem()
{
}

void SceneHeaderNodeItem::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget)
{
//   DEBTRACE("SceneHeaderNodeItem::paint");
  painter->save();
  painter->setPen(getPenColor());
  painter->setBrush(getBrushColor());
  painter->drawRect(QRectF(0, 0, _width, _height));
  painter->setBrush(getValidColor(_isValid));
  painter->drawEllipse(QRectF((_width - SceneCtrlPortItem::getPortHeight())/2,
                              getHeaderBottom() + _margin,
                              SceneCtrlPortItem::getPortHeight(),
                              SceneCtrlPortItem::getPortHeight()));
  painter->restore();
}

void SceneHeaderNodeItem::setText(QString label)
{
  if (_header) _header->setText(label);
}

qreal SceneHeaderNodeItem::getHeaderBottom() const
{
  qreal bottom = 0;
  if (_hasHeader)
    bottom = childBoundingRect(_header).bottom();
  return bottom;
}

void SceneHeaderNodeItem::autoPosControl(AbstractSceneItem *item)
{
  SceneCtrlInPortItem* inPortItem = dynamic_cast<SceneCtrlInPortItem*>(item);
  bool isInPort = inPortItem;

  qreal xLeft = _margin;
  if (!isInPort) xLeft += getInternWidth() - SceneCtrlPortItem::getPortWidth();

  qreal yTop  = getHeaderBottom() + _margin;

  //DEBTRACE("left, top " << xLeft  << " " << yTop);
  QPointF topLeft(xLeft, yTop);
  item->setTopLeft(topLeft);
  //adaptComposedNode(getHeaderBottom() + _margin);
}

void SceneHeaderNodeItem::autoPosNewPort(AbstractSceneItem *item)
{
  SceneInPortItem* inPortItem = dynamic_cast<SceneInPortItem*>(item);
  bool isInPort = (inPortItem != 0);

  int nbPorts = 0;
  qreal xLeft = _margin;
  if (!isInPort)
    {
      xLeft += getInternWidth() - ScenePortItem::getPortWidth() -1;
      nbPorts = _outPorts.size();
      _outPorts.push_back(dynamic_cast<SceneOutPortItem*>(item));
    }
  else
    {
      nbPorts = _inPorts.size();
      _inPorts.push_back(inPortItem);
    }
  qreal yTop  = getHeaderBottom() +ScenePortItem::getPortHeight() + 2*_margin;
  qreal deltaY = ScenePortItem::getPortHeight() + _margin;
  yTop += nbPorts * deltaY;
  //DEBTRACE("left, top " << xLeft  << " " << yTop);
  QPointF topLeft(xLeft, yTop);
  item->setTopLeft(topLeft);
  adaptComposedNode(deltaY);
}

void SceneHeaderNodeItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  if (_parent) _parent->popupMenu(caller, globalPos);
//   HeaderNodeMenu m;
//   m.popupMenu(caller, globalPos);
}

void SceneHeaderNodeItem::adjustGeometry()
{
  prepareGeometryChange();
  _width = _parent->getInternWidth() -1;
  if (_header) _header->adjustGeometry();
  adjustPosPorts();
  update();
}

void SceneHeaderNodeItem::adjustPosPorts()
{
  if (_controlOut) autoPosControl(_controlOut);
}

QRectF SceneHeaderNodeItem::getMinimalBoundingRect() const
{
  qreal width = 3*_margin;
  if (_inPorts.size())
    width += ScenePortItem::getPortWidth();
  else
    width += SceneCtrlPortItem::getPortWidth()
      + SceneCtrlPortItem::getPortHeight() + 2*_margin;
  if (_outPorts.size())
    width += ScenePortItem::getPortWidth();
  else
    width += SceneCtrlPortItem::getPortWidth();
  qreal height = getHeaderBottom() + ScenePortItem::getPortHeight() + 2*_margin;
  int nbPorts = _inPorts.size();
  if (_outPorts.size() > nbPorts) nbPorts = _outPorts.size();
  if (nbPorts) height += nbPorts*(ScenePortItem::getPortHeight() + _margin);
  //DEBTRACE(nbPorts << " " << width << " " << height);
  return QRectF(x(), y(), width, height);
}

void SceneHeaderNodeItem::adaptComposedNode(qreal deltaY)
{
  QPointF oldPos(x(),y() - deltaY);
  if (_parent)
    {
      if (SceneComposedNodeItem *bloc = dynamic_cast<SceneComposedNodeItem*>(_parent))
        bloc->collisionResolv(this, oldPos);
      //_parent->checkGeometryChange();
    }
}

void SceneHeaderNodeItem::setEdited(bool isEdited)
{
  DEBTRACE("SceneHeaderNodeItem::setEdited " << isEdited);
  if (isEdited)
    _brushColor = Resource::editedNodeBrushColor;
  else
    {
      if (QtGuiContext::getQtCurrent()->isEdition())
        _brushColor = Resource::normalNodeBrushColor;
      else
        _brushColor = Resource::runNodeBrushColor;
    }
  if (isEdited != _isEdited)
    _parent->update();
  _isEdited = isEdited;
}

void SceneHeaderNodeItem::setValid(bool isValid)
{
  DEBTRACE("SceneHeaderNodeItem::setValid " << isValid);
  if (_isValid != isValid)
    {
      _isValid = isValid;
      _parent->update();
    }
}

void SceneHeaderNodeItem::setExecState(int execState)
{
  DEBTRACE("SceneHeaderNodeItem::setExecState " << execState);
  _execState = execState;
  if (!_isValid) return;
  QColor oldsc = _sc;
  if (_isProc)
    switch (_execState)
      {
      case YACS::NOTYETINITIALIZED: _sc = Resource::NOTYETINITIALIZED; _stateDef = "Not Yet Initialized"; break;
      case YACS::INITIALISED:       _sc = Resource::INITIALISED      ; _stateDef = "Initialized"        ; break;
      case YACS::RUNNING:           _sc = Resource::RUNNING          ; _stateDef = "Running"            ; break;
      case YACS::WAITINGTASKS:      _sc = Resource::WAITINGTASKS     ; _stateDef = "Waiting Tasks"      ; break;
      case YACS::PAUSED:            _sc = Resource::PAUSED           ; _stateDef = "Paused"             ; break;
      case YACS::FINISHED:          _sc = Resource::FINISHED         ; _stateDef = "Finished"           ; break;
      case YACS::STOPPED:           _sc = Resource::STOPPED          ; _stateDef = "Stopped"            ; break;
      default:                      _sc = Resource::UNKNOWN          ; _stateDef = "Unknown Status"     ;
      }
  else
    switch (_execState)
      {
      case YACS::UNDEFINED:         _sc = Resource::UNDEFINED        ; _stateDef = "UNDEFINED"          ; break;
      case YACS::INVALID:           _sc = Resource::INVALID          ; _stateDef = "INVALID"            ; break;
      case YACS::READY:             _sc = Resource::READY            ; _stateDef = "READY"              ; break;
      case YACS::TOLOAD:            _sc = Resource::TOLOAD           ; _stateDef = "TOLOAD"             ; break;
      case YACS::LOADED:            _sc = Resource::LOADED           ; _stateDef = "LOADED"             ; break;
      case YACS::TOACTIVATE:        _sc = Resource::TOACTIVATE       ; _stateDef = "TOACTIVATE"         ; break;
      case YACS::ACTIVATED:         _sc = Resource::ACTIVATED        ; _stateDef = "ACTIVATED"          ; break;
      case YACS::DESACTIVATED:      _sc = Resource::DESACTIVATED     ; _stateDef = "DESACTIVATED"       ; break;
      case YACS::DONE:              _sc = Resource::DONE             ; _stateDef = "DONE"               ; break;
      case YACS::SUSPENDED:         _sc = Resource::SUSPENDED        ; _stateDef = "SUSPENDED"          ; break;
      case YACS::LOADFAILED:        _sc = Resource::LOADFAILED       ; _stateDef = "LOADFAILED"         ; break;
      case YACS::EXECFAILED:        _sc = Resource::EXECFAILED       ; _stateDef = "EXECFAILED"         ; break;
      case YACS::PAUSE:             _sc = Resource::PAUSE            ; _stateDef = "PAUSE"              ; break;
      case YACS::INTERNALERR:       _sc = Resource::INTERNALERR      ; _stateDef = "INTERNALERR"        ; break;
      case YACS::DISABLED:          _sc = Resource::DISABLED         ; _stateDef = "DISABLED"           ; break;
      case YACS::FAILED:            _sc = Resource::FAILED           ; _stateDef = "FAILED"             ; break;
      case YACS::ERROR:             _sc = Resource::ERROR            ; _stateDef = "ERROR"              ; break;
      default:                      _sc = Resource::DEFAULT          ; _stateDef = "---"                ;
      }
  DEBTRACE("  - stateDef = " << _stateDef.toStdString());
  if(oldsc != _sc)
    _parent->update();
}

SceneCtrlPortItem* SceneHeaderNodeItem::getCtrlInPortItem()
{
  return _controlIn;
}

SceneCtrlPortItem* SceneHeaderNodeItem::getCtrlOutPortItem()
{
  return _controlOut;
}



void SceneHeaderNodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  event->ignore();
}

QColor SceneHeaderNodeItem::getValidColor(bool isValid)
{
  if (isValid)
    return _sc;
  else
    return Resource::invalidNodeColor;
}
