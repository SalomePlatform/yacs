// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
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
#include "GuiEditor.hxx"
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
   _fatherNode= dynamic_cast<SceneNodeItem*>(parent);
  _width  = 2*Resource::Corner_Margin + 2*Resource::DataPort_Width + Resource::Space_Margin;
  _height = Resource::Header_Height;
  _maxPorts = 0;

  _header = 0;
  _controlIn = 0;
  _controlOut = 0;
  _label = "H_" + _label;
  _hasHeader = true;

  _controlIn = new SceneCtrlInPortItem(_scene, this, "in");
  _controlIn->setTopLeft(QPointF(Resource::Corner_Margin, Resource::Corner_Margin));

  _header = new SceneHeaderItem(_scene, this, "header_"+_label);
  _header->setText(label);
  _header->setTopLeft(QPointF(Resource::Corner_Margin + Resource::CtrlPort_Width + Resource::Space_Margin, Resource::Corner_Margin));

  _controlOut = new SceneCtrlOutPortItem(_scene, this, "out");
  _controlOut->setTopLeft(QPointF(Resource::Corner_Margin + 2*Resource::DataPort_Width + Resource::Space_Margin - Resource::CtrlPort_Width, Resource::Corner_Margin));

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
  //DEBTRACE("SceneHeaderNodeItem::paint");
  painter->save();

  int x = Resource::Border_Margin + 1;
  int y = Resource::Header_Height - Resource::Line_Space;
  if (_fatherNode && _fatherNode->hasProgressBar())
    y += Resource::progressBar_Height;
  int w = Resource::Corner_Margin + 2*Resource::DataPort_Width + 2*Resource::Space_Margin;
  if (_parent->getWidth() > w) w = _parent->getWidth() - Resource::Border_Margin;
  QPen pen(getPenColor());
  pen.setWidth(Resource::Line_Width);
  painter->setPen(pen);
  painter->drawLine(x, y, w, y);

  x = Resource::Corner_Margin + Resource::CtrlPort_Width + Resource::Space_Margin;
  y = Resource::Corner_Margin;
  w = w - 2*Resource::CtrlPort_Width - 3*Resource::Space_Margin - Resource::Corner_Margin;
  int h = Resource::CtrlPort_Height;
  pen.setWidth(Resource::Thickness);
  painter->setPen(pen);
  
  bool expanded = (_fatherNode && _fatherNode->isExpanded());
  QColor baseColor = getBrushColor();
  if (expanded)
    painter->setBrush(baseColor);
  else
    {
       int h, s, v, a, h2;
       baseColor.getHsv(&h, &s, &v, &a);
       DEBTRACE("h="<<h<<" s="<<s<<" v="<<v);
       h2 = h+60;
       if (h>359) h2 = h2-359;
       QLinearGradient gradient;
       gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
       gradient.setColorAt(0, baseColor.darker(200));
       //gradient.setColorAt(1, baseColor.lighter(150));
       //gradient.setColorAt(0, QColor::fromHsv(h-60, s, v, a));
       gradient.setColorAt(1, QColor::fromHsv(h2, s, v, a));
       QBrush brush(gradient);
       painter->setBrush(brush);
    }
  painter->drawRoundedRect(QRect(x, y, w, h), Resource::Radius, Resource::Radius);

  painter->restore();
}

void SceneHeaderNodeItem::setText(QString label)
{
  if (_header) _header->setText(label);
}

qreal SceneHeaderNodeItem::getHeaderBottom() const
{
  qreal res = 0;
  if (_hasHeader) {
    res += Resource::Header_Height + _maxPorts * (Resource::DataPort_Height + Resource::Space_Margin);
    if (_fatherNode && _fatherNode->hasProgressBar())
      res += Resource::progressBar_Height;
  }
  return res;
}

void SceneHeaderNodeItem::autoPosControl(AbstractSceneItem *item)
{
}

void SceneHeaderNodeItem::autoPosNewPort(AbstractSceneItem *item)
{
  DEBTRACE("SceneHeaderNodeItem::autoPosNewPort");
  SceneInPortItem* inPortItem = dynamic_cast<SceneInPortItem*>(item);

  int nbPorts;
  qreal xLeft;
  if (inPortItem) {
    xLeft = Resource::Corner_Margin;
    nbPorts = _inPorts.size();
    _inPorts.push_back(inPortItem);
  } else {
    if (_parent->getWidth() > (2*Resource::Corner_Margin + 2*Resource::DataPort_Width + Resource::Space_Margin)) {
      xLeft = _parent->getWidth() - Resource::Corner_Margin - Resource::DataPort_Width;
    } else {
      xLeft = Resource::Corner_Margin + Resource::DataPort_Width + Resource::Space_Margin;
    };
    nbPorts = _outPorts.size();
    _outPorts.push_back(dynamic_cast<SceneOutPortItem*>(item));
  };
  qreal yTop   = Resource::Header_Height;
  if (_fatherNode && _fatherNode->hasProgressBar())
    yTop += Resource::progressBar_Height;
  qreal deltaY = Resource::DataPort_Height + Resource::Space_Margin;
  yTop += nbPorts * deltaY;
  if (nbPorts >=_maxPorts) {
    _maxPorts = nbPorts+1;
  };
  //DEBTRACE("left, top " << xLeft  << " " << yTop);
  QPointF topLeft(xLeft, yTop);
  item->setTopLeft(topLeft);
  adaptComposedNode(dynamic_cast<SceneItem*>(item), deltaY);
}

void SceneHeaderNodeItem::reorganizePorts(shownState ss)
{
  DEBTRACE("SceneHeaderNodeItem::reorganizePorts() " << ss << " " << _label.toStdString());

  qreal yTop;
  qreal href = Resource::Header_Height;
  if (_fatherNode && _fatherNode->hasProgressBar())
    href += Resource::progressBar_Height;
  bool isShown = (ss != shrinkHidden);
  if (!isShown) href = Resource::Corner_Margin;

  std::list<SceneInPortItem*>::iterator iti = _inPorts.begin();
  int nbPorts = 0;
  for (; iti != _inPorts.end(); ++iti)
    {
      yTop = href + nbPorts * (Resource::DataPort_Height + Resource::Space_Margin);
      QPointF topLeft(Resource::Corner_Margin, yTop);
      (*iti)->setTopLeft(topLeft);
      if (isShown) nbPorts++; // otherwise (shrink from ancestor) put all ports at the same position
    }

  std::list<SceneOutPortItem*>::iterator ito = _outPorts.begin();
  nbPorts = 0;
  qreal xLeft;
  if (ss == expandShown) {
    xLeft = _parent->getWidth() - Resource::Corner_Margin - Resource::DataPort_Width;
  } else {
    xLeft = Resource::Corner_Margin + Resource::DataPort_Width + Resource::Space_Margin;
  };
  for (; ito != _outPorts.end(); ++ito)
    {
      yTop = href + nbPorts * (Resource::DataPort_Height + Resource::Space_Margin);
      QPointF topLeft(xLeft, yTop);
      (*ito)->setTopLeft(topLeft);
      if (isShown) nbPorts++; // otherwise (shrink from ancestor) put all ports at the same position
    }
  //updateLinks();
}

void SceneHeaderNodeItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  if (_parent) _parent->popupMenu(caller, globalPos);
//   HeaderNodeMenu m;
//   m.popupMenu(caller, globalPos);
}

void SceneHeaderNodeItem::adjustGeometry()
{
  DEBTRACE("SceneHeaderNodeItem::adjustGeometry() " << _label.toStdString());
  prepareGeometryChange();
  _width = _parent->getWidth();
  if (_header) _header->adjustGeometry();
  adjustPosPorts();
  update();
}

void SceneHeaderNodeItem::adjustPosPorts()
{
  YASSERT(_fatherNode);
  shownState ss = _fatherNode->getShownState();
  if (_controlOut)
    {
      int x = Resource::Corner_Margin + 2*Resource::DataPort_Width + Resource::Space_Margin;
      if ((ss == expandShown)  && (_parent->getWidth() > (x + Resource::Corner_Margin)))
        x = _parent->getWidth() - Resource::Corner_Margin;
    _controlOut->setTopLeft(QPointF(x - Resource::CtrlPort_Width, Resource::Corner_Margin));
  };
  reorganizePorts(ss);
}

QRectF SceneHeaderNodeItem::getMinimalBoundingRect() const
{
  qreal width  = 2*Resource::Corner_Margin + 2*Resource::DataPort_Width + Resource::Space_Margin;
  qreal height = Resource::Header_Height + Resource::Border_Margin;
  int nbPorts  = _inPorts.size();
  if (_outPorts.size() > nbPorts) nbPorts = _outPorts.size();
  if (nbPorts) height += nbPorts*(Resource::DataPort_Height + Resource::Space_Margin);
  if (_fatherNode && _fatherNode->hasProgressBar())
    height += Resource::progressBar_Height;
  //DEBTRACE(nbPorts << " " << width << " " << height);
  return QRectF(x(), y(), width, height);
}

void SceneHeaderNodeItem::adaptComposedNode(SceneItem* port, qreal deltaY)
{
  DEBTRACE("SceneHeaderNodeItem::adaptComposedNode " << deltaY);
  QPointF oldPos(x(),y() - deltaY);
  if (_parent)
    {
      if (SceneComposedNodeItem *bloc = dynamic_cast<SceneComposedNodeItem*>(_parent))
        bloc->collisionResolv(port, oldPos);
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

void SceneHeaderNodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  event->ignore();
}

QColor SceneHeaderNodeItem::getValidColor()
{
  if (_isValid)
    return _sc;
  else
    return Resource::invalidNodeColor;
}
