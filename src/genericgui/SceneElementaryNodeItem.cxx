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

#include "SceneElementaryNodeItem.hxx"
#include "SceneInPortItem.hxx"
#include "SceneOutPortItem.hxx"
#include "SceneComposedNodeItem.hxx"
#include "guiObservers.hxx"
#include "commandsProc.hxx"
#include "Scene.hxx"
#include "ElementaryNode.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"

#include "QtGuiContext.hxx"
#include "Menus.hxx"

#include "Resource.hxx"

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SceneElementaryNodeItem::SceneElementaryNodeItem(QGraphicsScene *scene, SceneItem *parent,
                                             QString label, Subject *subject)
  : SceneNodeItem(scene, parent, label, subject)
{
  _maxPorts = 0;
  _width  = 2*Resource::Corner_Margin + 2*Resource::DataPort_Width + Resource::Space_Margin;
  _height = Resource::Header_Height + Resource::Border_Margin;

  _brushColor   = Resource::ElementaryNode_brush;
  _hiBrushColor = Resource::ElementaryNode_hiBrush;
  _penColor     = Resource::ElementaryNode_pen;
  _hiPenColor   = Resource::ElementaryNode_hiPen;
}

SceneElementaryNodeItem::~SceneElementaryNodeItem()
{
}

//! SceneElementaryNodeItem cannot be resized (only ComposedNodeItem can)
void SceneElementaryNodeItem::setWidth(qreal width)
{
}

//! SceneElementaryNodeItem cannot be resized (only ComposedNodeItem can)
void SceneElementaryNodeItem::setHeight(qreal height)
{
}

void SceneElementaryNodeItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
  //DEBTRACE("SceneElementaryNodeItem::paint");
  painter->save();
  QPen pen(getPenColor());
  pen.setWidth(Resource::Thickness);
  painter->setPen(pen);
  painter->setBrush(getBrushColor());
  int w = _width  - 2*Resource::Border_Margin;
  int h = _height - 2*Resource::Border_Margin;
  painter->drawRect(QRectF(Resource::Border_Margin, Resource::Border_Margin, w, h));
  painter->restore();
}

void SceneElementaryNodeItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SceneElementaryNodeItem::update "<< eventName(event)<<" "<<type<<" "<<son);
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
          if (Scene::_autoComputeLinks && !QtGuiContext::getQtCurrent()->isLoading())
            {
              YACS::HMI::SubjectProc* subproc = QtGuiContext::getQtCurrent()->getSubjectProc();
              SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[subproc];
              SceneComposedNodeItem *proc = dynamic_cast<SceneComposedNodeItem*>(item);
              proc->rebuildLinks();
            }
          break;
        case YACS::HMI::OUTPUTPORT:
        case YACS::HMI::OUTPUTDATASTREAMPORT:
          item =  new SceneOutPortItem(_scene,
                                       this,
                                       son->getName().c_str(),
                                       son);
          autoPosNewPort(item, _outPorts.size());
          _outPorts.push_back(item);
          if (Scene::_autoComputeLinks && !QtGuiContext::getQtCurrent()->isLoading())
            {
              YACS::HMI::SubjectProc* subproc = QtGuiContext::getQtCurrent()->getSubjectProc();
              SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[subproc];
              SceneComposedNodeItem *proc = dynamic_cast<SceneComposedNodeItem*>(item);
              proc->rebuildLinks();
            }
           break;
//         default:
//           DEBTRACE("SceneElementaryNodeItem::update() ADD, type not handled:" << type);
        }
      break;
    case YACS::HMI::REMOVE:
    case YACS::HMI::SYNCHRO:
      reorganize();
      break;
//     default:
//       DEBTRACE("SceneElementaryNodeItem::update(), event not handled:" << eventName(event));
    }
}

void SceneElementaryNodeItem::autoPosNewPort(AbstractSceneItem *item, int nbPorts) {
  DEBTRACE("SceneElementaryNodeItem::autoPosNewPort "<< _label.toStdString());
  SceneInPortItem*   inPortItem = dynamic_cast<SceneInPortItem*>(item);
  SceneOutPortItem* outPortItem = dynamic_cast<SceneOutPortItem*>(item);

  bool toShow = (_shownState == expandShown);
  if (toShow) {
    qreal x;
    if (inPortItem) {
      x = Resource::Corner_Margin;
      inPortItem->show();
    } else {
      x = Resource::Corner_Margin + Resource::DataPort_Width + Resource::Space_Margin;
      outPortItem->show();
    };
    qreal y = Resource::Header_Height + nbPorts * (Resource::DataPort_Height + Resource::Space_Margin);

    if (_maxPorts <= nbPorts) {
      _maxPorts = nbPorts+1;
      _height  = Resource::Header_Height + Resource::Border_Margin;
      _height += _maxPorts * (Resource::DataPort_Height + Resource::Space_Margin);
      _incHeight = _height; // must just be more than the actual increment of height
      DEBTRACE("SceneElementaryNodeItem::autoPosNewPort _height=" << _height);
    };

    item->setTopLeft(QPointF(x, y));

  } else {
    _height = Resource::Header_Height + Resource::Border_Margin;
    qreal y = Resource::Corner_Margin;
    if (inPortItem) {
      item->setTopLeft(QPointF(Resource::Corner_Margin, y));
      inPortItem->hide();
    } else {
      item->setTopLeft(QPointF(Resource::Corner_Margin + Resource::DataPort_Width + Resource::Space_Margin, y));
      outPortItem->hide();
    };
  };
}

void SceneElementaryNodeItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  ElementaryNodeMenu m;
  m.popupMenu(caller, globalPos);
}

void SceneElementaryNodeItem::reorganizeShrinkExpand(ShrinkMode theShrinkMode)
{
  DEBTRACE("SceneElementaryNodeItem::reorganizeShrinkExpand " << isExpanded() << " "  << _label.toStdString());
  if (theShrinkMode != CurrentNode)
    return;
  shrinkExpandRecursive(!isExpanded(), true, theShrinkMode);
  if (Scene::_autoComputeLinks)
    {
      SubjectProc* subproc = QtGuiContext::getQtCurrent()->getSubjectProc();
      SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[subproc];
      SceneComposedNodeItem *proc = dynamic_cast<SceneComposedNodeItem*>(item);
      proc->rebuildLinks();
    }
}

void SceneElementaryNodeItem::shrinkExpandRecursive(bool toExpand, bool fromHere, ShrinkMode theShrinkMode)
{
  DEBTRACE("SceneElementaryNodeItem::shrinkExpandRecursive " << toExpand << " " << fromHere << " "  << isExpanded() << " " << _label.toStdString());
  
  bool toChangeShrinkState = false;
  switch (theShrinkMode) {
  case CurrentNode:
    if (fromHere)
      toChangeShrinkState = true;
    break;
  case ChildrenNodes:
    if (fromHere)
      toChangeShrinkState = true;
    break;
  case ElementaryNodes:
    toChangeShrinkState = true;
    break;
  }
  if (toChangeShrinkState && toExpand != isExpanded())
    setExpanded(toExpand);

  if (toExpand) {
    if (toChangeShrinkState) {
      _ancestorShrinked = false;
      _shownState = expandShown;
    } else {
      if (isExpanded())
        _shownState = expandShown;
      else
        _shownState = shrinkShown;
    }
  } else {
    if (fromHere || theShrinkMode==ElementaryNodes) {
      _shownState = shrinkShown;
    } else {
      _ancestorShrinked = true;
      _shownState = shrinkHidden;
    }
  }

  if (_shownState == shrinkHidden) // shrink of ancestor
    setPos(0, 0);
  else
    setPos(_expandedPos);

  reorganize();
}

void SceneElementaryNodeItem::reorganize()
{
  DEBTRACE("SceneElementaryNodeItem::reorganize() " << _label.toStdString());

  SubjectNode* snode = dynamic_cast<SubjectNode*>(_subject);
  ElementaryNode* father = dynamic_cast<ElementaryNode*>(snode->getNode());
  YASSERT(father);

  _maxPorts = 0;

  list<InPort*> plisti = father->getSetOfInPort();
  list<InPort*>::iterator iti = plisti.begin();
  int nbPorts = 0;
  for (; iti != plisti.end(); ++iti)
    {
      Subject *sub = QtGuiContext::getQtCurrent()->_mapOfSubjectDataPort[(*iti)];
      SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sub];
      autoPosNewPort(item, nbPorts);
      nbPorts++;
    }

  list<OutPort*> plisto = father->getSetOfOutPort();
  list<OutPort*>::iterator ito = plisto.begin();
  nbPorts = 0;
  for (; ito != plisto.end(); ++ito)
    {
      Subject *sub = QtGuiContext::getQtCurrent()->_mapOfSubjectDataPort[(*ito)];
      SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sub];
      autoPosNewPort(item, nbPorts);
      nbPorts++;
    }
  updateLinks();
}

void SceneElementaryNodeItem::setShownState(shownState ss)
{
  _shownState = ss;
  if (_shownState == shrinkHidden)
    {
      _ancestorShrinked = true;
      hide();
    }
  else
    {
      _ancestorShrinked = false;
      show();
    }
  reorganize();
}

