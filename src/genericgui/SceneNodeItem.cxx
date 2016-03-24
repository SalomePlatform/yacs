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

#include "SceneNodeItem.hxx"
#include "SceneComposedNodeItem.hxx"
#include "SceneProcItem.hxx"
#include "SceneHeaderNodeItem.hxx"
#include "SceneProgressItem.hxx"
#include "SceneInPortItem.hxx"
#include "SceneOutPortItem.hxx"
#include "SceneCtrlInPortItem.hxx"
#include "SceneCtrlOutPortItem.hxx"
#include "SceneLinkItem.hxx"
#include "Scene.hxx"
#include "QtGuiContext.hxx"
#include "Menus.hxx"
#include "GuiEditor.hxx"
#include "InPort.hxx"
#include "OutPort.hxx"

#include "Switch.hxx"
#include "Resource.hxx"

#include <QGraphicsSceneHoverEvent>
#include <QPointF>

#include <cmath>
#include <sstream>
#include <cassert>
#include <vector>

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
  _progressItem = 0;
  _brushColor = Resource::Scene_pen;
  _moving = false;
  _moved = false;
  _blocX = false;
  _blocY = false;
  _dragable = true;
  _dragButton = Qt::MidButton;
  _execState = YACS::UNDEFINED;
  _expanded = true;
  _expandedPos = QPointF(0,0);
  _expandedWidth = _width;
  _expandedHeight = _height;
  _shownState = expandShown;
}

SceneNodeItem::~SceneNodeItem()
{
  if (SceneComposedNodeItem* parent = getParent())
    parent->removeChildFromList(this);
}

void SceneNodeItem::setWidth(qreal width)
{
  if (width != _width)
    {
      prepareGeometryChange();
      _width = width;
      _expandedWidth = _width;
      adjustHeader();
      QGraphicsItem::update();
    }
}

void SceneNodeItem::setHeight(qreal height)
{
  if (height != _height)
    {
      prepareGeometryChange();
      _height = height;
      _expandedHeight = _height;
      QGraphicsItem::update();
    }
}

void SceneNodeItem::addHeader()
{
  DEBTRACE("SceneNodeItem::addHeader " << _label.toStdString());
  if (!_hasHeader)
    {
      _hasHeader = true;
      _header = new SceneHeaderNodeItem(_scene,
                                        this,
                                        getHeaderLabel());
      updateState();
      checkGeometryChange();
    }
}

SceneHeaderItem* SceneNodeItem::getHeader()
{
  return _header;
}

void SceneNodeItem::addProgressItem()
{
  DEBTRACE("SceneNodeItem::addProgressItem ");
  if (!_progressItem)
    {
      _progressItem = new SceneProgressItem(_scene,
                                        this,
                                        "progress");
      _progressItem->setText("0");
      updateState();
      checkGeometryChange();
    }
}

void SceneNodeItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
  //DEBTRACE("SceneNodeItem::paint");
}

void SceneNodeItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SceneNodeItem::update "<< eventName(event)<<" "<<type<<" "<<son);
  SceneObserverItem::update(event, type, son);
  SubjectNode *snode = 0;
  Node *node = 0;
  switch (event)
    {
    case YACS::HMI::RENAME:
      DEBTRACE("SceneNodeItem::update RENAME " << _subject->getName());
      updateName();
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

//! generic method to compute a graph for child nodes. implemented in some derived classes
void SceneNodeItem::arrangeNodes(bool isRecursive)
{
}

void SceneNodeItem::arrangeChildNodes()
{
}

void SceneNodeItem::reorganizeShrinkExpand(ShrinkMode theShrinkMode)
{
}

qreal SceneNodeItem::getHeaderBottom()
{
  if (_hasHeader) {
    return _header->getHeaderBottom();
  } else {
    return 0;
  };
}

void SceneNodeItem::autoPosNewPort(AbstractSceneItem *item, int nbPorts) {
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

void SceneNodeItem::updateName()
{
  if (_header) _header->setText(getHeaderLabel());
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
  if (_moved)
    if (Resource::ensureVisibleWhenMoved)
      QtGuiContext::getQtCurrent()->getView()->ensureVisible(this);
  _moved = false;
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
          if (aPos != oldPos)
            _moved = true;
          if (aPos.x() > oldPos.x()) _blocX = false;
          if (aPos.y() > oldPos.y()) _blocY = false;
          if (aPos.x() < Resource::Border_Margin)
            {
              aPos.setX(Resource::Border_Margin);
              _blocX = true;
            }
          if ( aPos.y() < bloc->getHeaderBottom() )
            {
              aPos.setY(bloc->getHeaderBottom());
              _blocY = true;
            }
          setTopLeft(aPos);
          bloc->collisionResolv(this, oldPos);
        }
    }
}

void SceneNodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  DEBTRACE("SceneNodeItem::mouseDoubleClickEvent");
  QtGuiContext::getQtCurrent()->getGMain()->_guiEditor->shrinkExpand(QApplication::keyboardModifiers());
}

void SceneNodeItem::setTopLeft(QPointF topLeft)
{
  if (_shownState == shrinkHidden) {
    _expandedPos = topLeft;
    return;
  }
  QPointF oldPos = pos();
  setPos(topLeft);

  //update links
  updateLinks();

  if (_parent)
    {
      if (SceneComposedNodeItem *bloc = dynamic_cast<SceneComposedNodeItem*>(_parent))
        bloc->collisionResolv(this, oldPos);
      _parent->checkGeometryChange();
    }
  _expandedPos = pos();
}

void SceneNodeItem::adjustHeader()
{
  if (_header) _header->adjustGeometry();
}

 void SceneNodeItem::updateState()
{
  SubjectNode *snode = dynamic_cast<SubjectNode*>(_subject);
  YASSERT(snode);
  Node *node = snode->getNode();
  YASSERT(node);
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

QString SceneNodeItem::getHeaderLabel()
{
  DEBTRACE("SceneNodeItem::getHeaderLabel");
  QString extLabel = _subject->getName().c_str();

  SceneObserverItem *soi = 0;
  SubjectSwitch* sswi = 0;

  if ((_parent)
      && (soi = dynamic_cast<SceneObserverItem*>(_parent))
      && (sswi = dynamic_cast<SubjectSwitch*>(soi->getSubject())))
    {
      Switch *aswi = dynamic_cast<Switch*>(sswi->getNode());
      SubjectNode *sno = dynamic_cast<SubjectNode*>(_subject);
      int idcase = aswi->getRankOfNode(sno->getNode());
      stringstream caseid;
      if (idcase == Switch::ID_FOR_DEFAULT_NODE)
        caseid << "default";
      else
        caseid << idcase;
      extLabel += " (";
      //extLabel += aswi->getCaseId(sno->getNode()).c_str();
      extLabel += caseid.str().c_str();
      extLabel += ")";
      DEBTRACE(extLabel.toStdString());
    }

  return extLabel;
}

void SceneNodeItem::updateLinks()
{
  //update control links
  std::list<SubjectControlLink*> lscl=dynamic_cast<SubjectNode*>(_subject)->getSubjectControlLinks();
  for (std::list<SubjectControlLink*>::const_iterator it = lscl.begin(); it != lscl.end(); ++it)
    {
      SceneLinkItem* item = dynamic_cast<SceneLinkItem*>(QtGuiContext::getQtCurrent()->_mapOfSceneItem[*it]);
      item->updateShape();
    }

  //update data links through child items update (SceneDataPortItem)
  updateChildItems();
}

void SceneNodeItem::updateChildItems()
{
  foreach (QGraphicsItem *child, childItems())
    {
      if (SceneItem *sci = dynamic_cast<SceneItem*>(child))
        {
           sci->updateLinks();
        }
    }
}

void SceneNodeItem::shrinkExpandLink(bool se)
{
  foreach (QGraphicsItem *child, childItems())
    {
      if (SceneItem *sci = dynamic_cast<SceneItem*>(child))
        {
           sci->shrinkExpandLink(se);
        }
    }
}

void SceneNodeItem::showOutScopeLinks()
{
  SubjectNode *snode = dynamic_cast<SubjectNode*>(_subject);
  YASSERT(snode);
  Node *node = snode->getNode();
  YASSERT(node);
  vector<pair<OutPort *, InPort *> > listLeaving  = node->getSetOfLinksLeavingCurrentScope();
  vector<pair<InPort *, OutPort *> > listIncoming = node->getSetOfLinksComingInCurrentScope();
  vector<pair<OutPort *, InPort *> > outScope = listLeaving;
  vector<pair<InPort *, OutPort *> >::iterator it1;
  for (it1 = listIncoming.begin(); it1 != listIncoming.end(); ++it1)
    {
      pair<OutPort *, InPort *> outin = pair<OutPort *, InPort *>((*it1).second, (*it1).first);
      outScope.push_back(outin);
    }
  vector<pair<OutPort*, InPort*> >::const_iterator it = outScope.begin();
  for( ; it != outScope.end(); ++it)
  {
    YASSERT(QtGuiContext::getQtCurrent()->_mapOfSubjectLink.count(*it));
    SubjectLink* slink = QtGuiContext::getQtCurrent()->_mapOfSubjectLink[*it];
    YASSERT(QtGuiContext::getQtCurrent()->_mapOfSceneItem.count(slink));
    SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[slink];
    item->show();
  }
}

void SceneNodeItem::setShownState(shownState ss)
{
  _shownState = ss;
}

bool SceneNodeItem::hasProgressBar() const
{
  return _progressItem != 0;
}
