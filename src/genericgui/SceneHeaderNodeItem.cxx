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

QColor SceneHeaderNodeItem::_editedNodeBrushColor = QColor();
QColor SceneHeaderNodeItem::_normalNodeBrushColor = QColor();
QColor SceneHeaderNodeItem::_runNodeBrushColor    = QColor();
QColor SceneHeaderNodeItem::_validNodeColor       = QColor();
QColor SceneHeaderNodeItem::_invalidNodeColor     = QColor();

QColor SceneHeaderNodeItem::_NOTYETINITIALIZED    = QColor();
QColor SceneHeaderNodeItem::_INITIALISED          = QColor();
QColor SceneHeaderNodeItem::_RUNNING              = QColor();
QColor SceneHeaderNodeItem::_WAITINGTASKS         = QColor();
QColor SceneHeaderNodeItem::_PAUSED               = QColor();
QColor SceneHeaderNodeItem::_FINISHED             = QColor();
QColor SceneHeaderNodeItem::_STOPPED              = QColor();
QColor SceneHeaderNodeItem::_UNKNOWN              = QColor();

QColor SceneHeaderNodeItem::_UNDEFINED            = QColor();
QColor SceneHeaderNodeItem::_INVALID              = QColor();
QColor SceneHeaderNodeItem::_READY                = QColor();
QColor SceneHeaderNodeItem::_TOLOAD               = QColor();
QColor SceneHeaderNodeItem::_LOADED               = QColor();
QColor SceneHeaderNodeItem::_TOACTIVATE           = QColor();
QColor SceneHeaderNodeItem::_ACTIVATED            = QColor();
QColor SceneHeaderNodeItem::_DESACTIVATED         = QColor();
QColor SceneHeaderNodeItem::_DONE                 = QColor();
QColor SceneHeaderNodeItem::_SUSPENDED            = QColor();
QColor SceneHeaderNodeItem::_LOADFAILED           = QColor();
QColor SceneHeaderNodeItem::_EXECFAILED           = QColor();
QColor SceneHeaderNodeItem::_PAUSE                = QColor();
QColor SceneHeaderNodeItem::_INTERNALERR          = QColor();
QColor SceneHeaderNodeItem::_DISABLED             = QColor();
QColor SceneHeaderNodeItem::_FAILED               = QColor();
QColor SceneHeaderNodeItem::_ERROR                = QColor();
QColor SceneHeaderNodeItem::_DEFAULT              = QColor();

SceneHeaderNodeItem::SceneHeaderNodeItem(QGraphicsScene *scene, SceneItem *parent,
                                         QString label)
  : SceneHeaderItem(scene, parent, label)
{
  _header = 0;
  _controlIn = 0;
  _controlOut = 0;
  _label = "H_" + _label;

  if (_editedNodeBrushColor == _normalNodeBrushColor) {
    SuitWrapper* wrapper = QtGuiContext::getQtCurrent()->getWrapper();

    _editedNodeBrushColor = wrapper->colorValue("STATE:EDITED"           , QColor(255, 255, 190));
    _normalNodeBrushColor = wrapper->colorValue("STATE:NORMAL"           , QColor(230, 235, 255));
    _runNodeBrushColor    = wrapper->colorValue("STATE:RUN"              , QColor(205, 218, 255));
    _validNodeColor       = wrapper->colorValue("STATE:VALID"            , QColor(128, 255, 128));
    _invalidNodeColor     = wrapper->colorValue("STATE:INVALID"          , QColor(255, 128, 128));

    _NOTYETINITIALIZED    = wrapper->colorValue("STATE:NOTYETINITIALIZED",  45, 50, 255   );
    _INITIALISED          = wrapper->colorValue("STATE:INITIALISED"      ,  90, 50, 255   );
    _RUNNING              = wrapper->colorValue("STATE:RUNNING"          , 135, 50, 255   );
    _WAITINGTASKS         = wrapper->colorValue("STATE:WAITINGTASKS"     , 180, 50, 255   );
    _PAUSED               = wrapper->colorValue("STATE:PAUSED"           , 225, 50, 255   );
    _FINISHED             = wrapper->colorValue("STATE:FINISHED"         , 270, 50, 255   );
    _STOPPED              = wrapper->colorValue("STATE:STOPPED"          , 315, 50, 255   );
    _UNKNOWN              = wrapper->colorValue("STATE:UNKNOWN"          , 360, 50, 255   );

    _UNDEFINED            = wrapper->colorValue("STATE:UNDEFINED"        , Qt::lightGray  );
    _INVALID              = wrapper->colorValue("STATE:INVALID"          , Qt::red        );
    _READY                = wrapper->colorValue("STATE:READY"            , Qt::gray       );
    _TOLOAD               = wrapper->colorValue("STATE:TOLOAD"           , Qt::darkYellow );
    _LOADED               = wrapper->colorValue("STATE:LOADED"           , Qt::darkMagenta);
    _TOACTIVATE           = wrapper->colorValue("STATE:TOACTIVATE"       , Qt::darkCyan   );
    _ACTIVATED            = wrapper->colorValue("STATE:ACTIVATED"        , Qt::darkBlue   );
    _DESACTIVATED         = wrapper->colorValue("STATE:DESACTIVATED"     , Qt::gray       );
    _DONE                 = wrapper->colorValue("STATE:DONE"             , Qt::darkGreen  );
    _SUSPENDED            = wrapper->colorValue("STATE:SUSPENDED"        , Qt::gray       );
    _LOADFAILED           = wrapper->colorValue("STATE:LOADFAILED"       , 320, 255, 255  );
    _EXECFAILED           = wrapper->colorValue("STATE:EXECFAILED"       ,  20, 255, 255  );
    _PAUSE                = wrapper->colorValue("STATE:PAUSE"            , 180, 255, 255  );
    _INTERNALERR          = wrapper->colorValue("STATE:INTERNALERR"      , 340, 255, 255  );
    _DISABLED             = wrapper->colorValue("STATE:DISABLED"         ,  40, 255, 255  );
    _FAILED               = wrapper->colorValue("STATE:FAILED"           ,  20, 255, 255  );
    _ERROR                = wrapper->colorValue("STATE:ERROR"            ,   0, 255, 255  );
    _DEFAULT              = wrapper->colorValue("STATE:DEFAULT"          , Qt::lightGray  );
  };

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
    _brushColor = _normalNodeBrushColor;
  else
    _brushColor = _runNodeBrushColor;
  _execState = YACS::UNDEFINED;
  _sc = _validNodeColor;
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
    _brushColor = _editedNodeBrushColor;
  else
    {
      if (QtGuiContext::getQtCurrent()->isEdition())
        _brushColor = _normalNodeBrushColor;
      else
        _brushColor = _runNodeBrushColor;
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
      case YACS::NOTYETINITIALIZED: _sc = _NOTYETINITIALIZED; _stateDef = "Not Yet Initialized"; break;
      case YACS::INITIALISED:       _sc = _INITIALISED      ; _stateDef = "Initialized"        ; break;
      case YACS::RUNNING:           _sc = _RUNNING          ; _stateDef = "Running"            ; break;
      case YACS::WAITINGTASKS:      _sc = _WAITINGTASKS     ; _stateDef = "Waiting Tasks"      ; break;
      case YACS::PAUSED:            _sc = _PAUSED           ; _stateDef = "Paused"             ; break;
      case YACS::FINISHED:          _sc = _FINISHED         ; _stateDef = "Finished"           ; break;
      case YACS::STOPPED:           _sc = _STOPPED          ; _stateDef = "Stopped"            ; break;
      default:                      _sc = _UNKNOWN          ; _stateDef = "Unknown Status"     ;
      }
  else
    switch (_execState)
      {
      case YACS::UNDEFINED:         _sc = _UNDEFINED        ; _stateDef = "UNDEFINED"          ; break;
      case YACS::INVALID:           _sc = _INVALID          ; _stateDef = "INVALID"            ; break;
      case YACS::READY:             _sc = _READY            ; _stateDef = "READY"              ; break;
      case YACS::TOLOAD:            _sc = _TOLOAD           ; _stateDef = "TOLOAD"             ; break;
      case YACS::LOADED:            _sc = _LOADED           ; _stateDef = "LOADED"             ; break;
      case YACS::TOACTIVATE:        _sc = _TOACTIVATE       ; _stateDef = "TOACTIVATE"         ; break;
      case YACS::ACTIVATED:         _sc = _ACTIVATED        ; _stateDef = "ACTIVATED"          ; break;
      case YACS::DESACTIVATED:      _sc = _DESACTIVATED     ; _stateDef = "DESACTIVATED"       ; break;
      case YACS::DONE:              _sc = _DONE             ; _stateDef = "DONE"               ; break;
      case YACS::SUSPENDED:         _sc = _SUSPENDED        ; _stateDef = "SUSPENDED"          ; break;
      case YACS::LOADFAILED:        _sc = _LOADFAILED       ; _stateDef = "LOADFAILED"         ; break;
      case YACS::EXECFAILED:        _sc = _EXECFAILED       ; _stateDef = "EXECFAILED"         ; break;
      case YACS::PAUSE:             _sc = _PAUSE            ; _stateDef = "PAUSE"              ; break;
      case YACS::INTERNALERR:       _sc = _INTERNALERR      ; _stateDef = "INTERNALERR"        ; break;
      case YACS::DISABLED:          _sc = _DISABLED         ; _stateDef = "DISABLED"           ; break;
      case YACS::FAILED:            _sc = _FAILED           ; _stateDef = "FAILED"             ; break;
      case YACS::ERROR:             _sc = _ERROR            ; _stateDef = "ERROR"              ; break;
      default:                      _sc = _DEFAULT          ; _stateDef = "---"                ;
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
    return _invalidNodeColor;
}
