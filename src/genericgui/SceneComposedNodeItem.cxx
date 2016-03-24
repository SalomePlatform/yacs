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

#define CHRONODEF
#include "chrono.hxx"

#include "SceneBlocItem.hxx"
#include "SceneComposedNodeItem.hxx"
#include "SceneElementaryNodeItem.hxx"
#include "SceneHeaderItem.hxx"
#include "SceneHeaderNodeItem.hxx"
#include "SceneInPortItem.hxx"
#include "SceneOutPortItem.hxx"
#include "SceneLinkItem.hxx"
#include "SceneDSLinkItem.hxx"
#include "SceneCtrlLinkItem.hxx"
#include "SceneProgressItem.hxx"
#include "LinkMatrix.hxx"
#include "LinkAStar.hxx"
#include "ItemMimeData.hxx"
#include "QtGuiContext.hxx"
#include "Menus.hxx"
#include "Message.hxx"
#include "guiObservers.hxx"
#include "GuiEditor.hxx"
#include "Scene.hxx"
#include "Switch.hxx"

#include <QPointF>
#include <QGraphicsSceneDragDropEvent>

#include <cassert>

#include "Resource.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SceneComposedNodeItem::SceneComposedNodeItem(QGraphicsScene *scene, SceneItem *parent,
                                             QString label, Subject *subject)
  : SceneNodeItem(scene, parent, label, subject)
{
  DEBTRACE("SceneComposedNodeItem::SceneComposedNodeItem " <<label.toStdString());

  _width  = 2*Resource::Corner_Margin + 2*Resource::DataPort_Width + Resource::Space_Margin;
  _height = Resource::Header_Height + Resource::DataPort_Height + Resource::Corner_Margin;

  _hiBrushColor = Resource::ComposedNode_hiBrush;
  _penColor     = Resource::ComposedNode_pen;
  _hiPenColor   = Resource::ComposedNode_hiPen;
  adjustColors();
  _dragOver = false;
  setAcceptDrops(true);
}

SceneComposedNodeItem::~SceneComposedNodeItem()
{
}

void SceneComposedNodeItem::adjustColors()
{
  _brushColor = Resource::ComposedNode_brush.darker(100 +5*_level);
  for (list<AbstractSceneItem*>::const_iterator it=_children.begin(); it!=_children.end(); ++it)
    {
      if (SceneComposedNodeItem *scnode = dynamic_cast<SceneComposedNodeItem*>(*it))
        scnode->adjustColors();
    }
 }

QRectF SceneComposedNodeItem::childrenBoundingRect() const
{
  QRectF ChildrenBRect =QRectF(x(), y(), 5, 5);
  if (_header) ChildrenBRect = _header->getMinimalBoundingRect();
  for (list<AbstractSceneItem*>::const_iterator it=_children.begin(); it!=_children.end(); ++it)
    ChildrenBRect = ChildrenBRect.united(childBoundingRect(*it));
  ChildrenBRect.setWidth (ChildrenBRect.width()  + Resource::Border_Margin);
  ChildrenBRect.setHeight(ChildrenBRect.height() + Resource::Border_Margin);
  return ChildrenBRect;
}

void SceneComposedNodeItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
  //DEBTRACE("SceneComposedNodeItem::paint " << _label.toStdString());
  // IMN (14/11/2013) below block commented
  // prevent bad resizing of scene in shrink mode
  /*  if (!isExpanded()) {
    _width  = 2*Resource::Corner_Margin + 2*Resource::DataPort_Width + Resource::Space_Margin;
    _height = getHeaderBottom() + Resource::Corner_Margin;
  };*/

  painter->save();
  painter->setPen(getPenColor());
  painter->setBrush(getBrushColor());
  painter->drawRect(QRectF(Resource::Border_Margin, Resource::Border_Margin, _width - 2*Resource::Border_Margin, _height - 2*Resource::Border_Margin));
  painter->restore();
}

void SceneComposedNodeItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SceneComposedNodeItem::update "<< eventName(event)<<" "<<type<<" "<<son);
  SceneNodeItem::update(event, type, son);
  AbstractSceneItem *item;
  switch (event)
    {
    case YACS::HMI::ADD:
      switch (type)
        {
        case YACS::HMI::BLOC:
          item =  new SceneBlocItem(_scene,
                                    this,
                                    son->getName().c_str(),
                                    son);
          item->addHeader();
          autoPosNewChild(item, _children, true);
          break;
        case YACS::HMI::FORLOOP:
        case YACS::HMI::WHILELOOP:
        case YACS::HMI::SWITCH:
        case YACS::HMI::FOREACHLOOP:
        case YACS::HMI::OPTIMIZERLOOP:
          item =  new SceneComposedNodeItem(_scene,
                                            this,
                                            son->getName().c_str(),
                                            son);
          item->addHeader();
          if ( !QtGuiContext::getQtCurrent()->isEdition()
            && (type == YACS::HMI::FORLOOP || type == YACS::HMI::FOREACHLOOP) )
            {
              item->addProgressItem();
            }
          autoPosNewChild(item, _children, true);
          break;
        case YACS::HMI::PYTHONNODE:
        case YACS::HMI::PYFUNCNODE:
        case YACS::HMI::CORBANODE:
        case YACS::HMI::CPPNODE:
        case YACS::HMI::SALOMENODE:
        case YACS::HMI::SALOMEPYTHONNODE:
        case YACS::HMI::XMLNODE:
        case YACS::HMI::PRESETNODE:
        case YACS::HMI::OUTNODE:
        case YACS::HMI::STUDYINNODE:
        case YACS::HMI::STUDYOUTNODE:
          item =  new SceneElementaryNodeItem(_scene,
                                              this,
                                              son->getName().c_str(),
                                              son);
          item->addHeader();
          autoPosNewChild(item, _children, true);
          break;
        case YACS::HMI::INPUTPORT:
        case YACS::HMI::INPUTDATASTREAMPORT:
          item =  new SceneInPortItem(_scene,
                                      _header,
                                      son->getName().c_str(),
                                      son);
          _header->autoPosNewPort(item);
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
                                       _header,
                                       son->getName().c_str(),
                                       son);
          _header->autoPosNewPort(item);
          _outPorts.push_back(item);
          if (Scene::_autoComputeLinks && !QtGuiContext::getQtCurrent()->isLoading())
            {
              YACS::HMI::SubjectProc* subproc = QtGuiContext::getQtCurrent()->getSubjectProc();
              SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[subproc];
              SceneComposedNodeItem *proc = dynamic_cast<SceneComposedNodeItem*>(item);
              proc->rebuildLinks();
            }
          break;
        default:
          DEBTRACE("SceneComposedNodeItem::update() ADD, type not handled:" << type);
        }
      break;
    case YACS::HMI::ADDLINK:
      switch (type)
        {
        case YACS::HMI::DATALINK:
          if (SubjectLink* slink = dynamic_cast<SubjectLink*>(son))
            {
              SubjectDataPort* soutp = slink->getSubjectOutPort();
              SubjectDataPort* sinp  = slink->getSubjectInPort();
              SceneItem * scout = QtGuiContext::getQtCurrent()->_mapOfSceneItem[soutp];
              SceneItem * scin  = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sinp];
              ScenePortItem* from = dynamic_cast<ScenePortItem*>(scout);
              ScenePortItem* to  = dynamic_cast<ScenePortItem*>(scin);
              if (dynamic_cast<SubjectInputDataStreamPort*>(sinp))
                {
                  SceneDSLinkItem* item = new SceneDSLinkItem(_scene,
                                             this,
                                             from, to,
                                             son->getName().c_str(),
                                             son);
                  item->updateShape();
                }
              else
                {
                  SceneLinkItem* item = new SceneLinkItem(_scene,
                                           this,
                                           from, to,
                                           son->getName().c_str(),
                                           son);
                  item->updateShape();
                }
              if (Scene::_autoComputeLinks && !QtGuiContext::getQtCurrent()->isLoading())
                {
                  YACS::HMI::SubjectProc* subproc = QtGuiContext::getQtCurrent()->getSubjectProc();
                  SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[subproc];
                  SceneComposedNodeItem *proc = dynamic_cast<SceneComposedNodeItem*>(item);
                  proc->rebuildLinks();
                }
            }
          break;
        }
      break;
    case YACS::HMI::ADDCONTROLLINK:
      switch (type)
        {
        case YACS::HMI::CONTROLLINK:
          if (SubjectControlLink* slink = dynamic_cast<SubjectControlLink*>(son))
            {
              SubjectNode* soutn = slink->getSubjectOutNode();
              SubjectNode* sinn  = slink->getSubjectInNode();
              SceneItem * scout = QtGuiContext::getQtCurrent()->_mapOfSceneItem[soutn];
              SceneItem * scin  = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sinn];
              SceneNodeItem* nodefrom = dynamic_cast<SceneNodeItem*>(scout);
              SceneNodeItem* nodeto  = dynamic_cast<SceneNodeItem*>(scin);
              if (!nodeto || !nodefrom) DEBTRACE("CONTROLLINK problem -----------------");
              ScenePortItem* from = nodefrom->getCtrlOutPortItem();
              ScenePortItem* to = nodeto->getCtrlInPortItem();
              if (!to || !from) DEBTRACE("CONTROLLINK problem -----------------");
              SceneCtrlLinkItem* item = new SceneCtrlLinkItem(_scene,
                                           this,
                                           from, to,
                                           son->getName().c_str(),
                                           son);
              item->updateShape();
              if (Scene::_autoComputeLinks && !QtGuiContext::getQtCurrent()->isLoading())
                {
                  YACS::HMI::SubjectProc* subproc = QtGuiContext::getQtCurrent()->getSubjectProc();
                  SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[subproc];
                  SceneComposedNodeItem *proc = dynamic_cast<SceneComposedNodeItem*>(item);
                  proc->rebuildLinks();
                }
            }
          break;
        }
      break;
    case YACS::HMI::REMOVE:
      //SceneObserverItem::update(event, type, son);
      break;
    case YACS::HMI::SETCASE:
      {
        SubjectSwitch *sSwitch = dynamic_cast<SubjectSwitch*>(_subject);
        if (sSwitch)
          {
            Switch *aSwitch = dynamic_cast<Switch*>(sSwitch->getNode());
            Node *node = aSwitch->edGetNode(type);
            if (node)
              {
                if (GuiContext::getCurrent()->_mapOfSubjectNode.count(node))
                  {
                    Subject* sub = GuiContext::getCurrent()->_mapOfSubjectNode[node];
                    if (QtGuiContext::getQtCurrent()->_mapOfSceneItem.count(sub))
                      {
                        SceneItem* item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sub];
                        SceneNodeItem *scnode = dynamic_cast<SceneNodeItem*>(item);
                        if (scnode) scnode->updateName();
                      }
                  }
              }
          }
      }
      break;
    case YACS::HMI::CUT:
      {
        SceneItem * sinode  = QtGuiContext::getQtCurrent()->_mapOfSceneItem[son];
        removeChildFromList(sinode);
      }
      break;
    case YACS::HMI::PASTE:
      {
        SceneItem * sinode  = QtGuiContext::getQtCurrent()->_mapOfSceneItem[son];
        sinode->setParent(this);
        sinode->setLevel();
        if (SceneComposedNodeItem *scnode = dynamic_cast<SceneComposedNodeItem*>(sinode))
          scnode->adjustColors();
        autoPosNewChild(sinode, _children, true);
      }
      break;
    case PROGRESS:
      {
        if (dynamic_cast<SubjectForLoop*>(son) || dynamic_cast<SubjectForEachLoop*>(son))
        {
          if (SceneProgressItem* spitem = getProgressItem())
            spitem->setProgress(son->getProgress().c_str());
        }
      }
      break;
    default:
       ;
//       DEBTRACE("SceneComposedNodeItem::update(), event not handled: "<< eventName(event));
    }
}

void SceneComposedNodeItem::autoPosNewChild(AbstractSceneItem *item,
                                            const std::list<AbstractSceneItem*> alreadySet,
                                            bool isNew)
{
  SceneItem *it = dynamic_cast<SceneItem*>(item);
  YASSERT(it);

  QRectF childrenBox;
  qreal xLeft = Resource::Corner_Margin;
  qreal yTop  = getHeaderBottom() + Resource::Space_Margin;
  for (list<AbstractSceneItem*>::const_iterator it=alreadySet.begin(); it!=alreadySet.end(); ++it)
    {
      childrenBox = childrenBox.united(childBoundingRect(*it));
      DEBTRACE((*it)->getLabel().toStdString());
      DEBTRACE("childrenBox valid " << childrenBox.right() << " " << childrenBox.bottom());
    }
  if (childrenBox.isValid())
    yTop = childrenBox.bottom() + 1.; // +1. to avoid collision with bottom (penwidth)
  //xLeft += childrenBox.right();
  DEBTRACE("left, top " << xLeft  << " " << yTop);
  QPointF topLeft(xLeft, yTop);
  if (isNew) _children.push_back(item);
  if (_eventPos.isNull())
    {
      //DEBTRACE("_eventPos.isNull");
      item->setTopLeft(topLeft);
    }
  else
    {
      //DEBTRACE("_eventPos " << _eventPos.x() << " " << _eventPos.y());
      item->setTopLeft(_eventPos);
    }
  collisionResolv(it, -it->boundingRect().bottomRight()); // as if the new item was coming from top left (previous position outside)
  if (Scene::_autoComputeLinks && !QtGuiContext::getQtCurrent()->isLoading()) rebuildLinks();
  _eventPos.setX(0);
  _eventPos.setY(0);
}

void SceneComposedNodeItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  ComposedNodeMenu m;
  m.popupMenu(caller, globalPos);
}

std::list<AbstractSceneItem*> SceneComposedNodeItem::getChildren()
{
  return _children;
}

void SceneComposedNodeItem::removeChildFromList(AbstractSceneItem* child)
{
  _children.remove(child);
}

void SceneComposedNodeItem::updateControlLinks(bool toExpand)
{
  //update control links
  std::list<SubjectControlLink*> lscl=dynamic_cast<SubjectNode*>(getSubject())->getSubjectControlLinks();
  for (std::list<SubjectControlLink*>::const_iterator it = lscl.begin(); it != lscl.end(); ++it) {
    SceneLinkItem* lk = dynamic_cast<SceneLinkItem*>(QtGuiContext::getQtCurrent()->_mapOfSceneItem[*it]);

    bool b1 = true, b2 = true;

    SceneNodeItem* no = lk->getFromNode();
    if (no) {
      SceneComposedNodeItem* scni = dynamic_cast<SceneComposedNodeItem*>(no);
      if (scni) {
        b1 = scni!=this;
      };
    };

    no = lk->getToNode();
    if (no) {
      SceneComposedNodeItem* scni = dynamic_cast<SceneComposedNodeItem*>(no);
      if (scni) {
        b2 = scni!=this;
      };
    };

    if (b1 && b2) {
      if (toExpand) {
        lk->show();
      } else {
        lk->hide();
      };
    };
  };
}

void SceneComposedNodeItem::reorganizeShrinkExpand(ShrinkMode theShrinkMode) {
  DEBTRACE("SceneComposedNodeItem::reorganizeShrinkExpand " << _expanded << " " << _label.toStdString());

  bool toExpand = true;
  if (theShrinkMode == CurrentNode) {
    // shrink/expand current node only
    toExpand = !isExpanded();

    updateControlLinks(toExpand);
    shrinkExpandRecursive(toExpand, true, theShrinkMode);

  } else {
    if (!isExpanded())
      return;
    // shrink/expand child nodes
    toExpand = !hasExpandedChildren(theShrinkMode == ElementaryNodes);
    for (list<AbstractSceneItem*>::const_iterator it=_children.begin(); it!=_children.end(); ++it) {
      SceneItem* item = dynamic_cast<SceneItem*>(*it);
      SceneNodeItem *sni = dynamic_cast<SceneNodeItem*>(item);
      item->shrinkExpandRecursive(toExpand, true, theShrinkMode);
    }
    _ancestorShrinked = !toExpand;
    _width = _expandedWidth;
    _height = _expandedHeight;
    _shownState = expandShown;
    adjustHeader();
    rebuildLinks();
  }

  if (Scene::_autoComputeLinks)
    {
      SubjectProc* subproc = QtGuiContext::getQtCurrent()->getSubjectProc();
      SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[subproc];
      SceneComposedNodeItem *proc = dynamic_cast<SceneComposedNodeItem*>(item);
      proc->rebuildLinks();
    }
}

bool SceneComposedNodeItem::hasExpandedChildren(bool recursively)
{
  bool res = false;
  for (list<AbstractSceneItem*>::const_iterator it=_children.begin(); it!=_children.end() && !res; ++it) {
    SceneItem* item = dynamic_cast<SceneItem*>(*it);
    SceneNodeItem *sni = dynamic_cast<SceneNodeItem*>(item);
    if (sni->isExpanded()) {
      res = true;
      if (recursively)
        if (SceneComposedNodeItem *scni = dynamic_cast<SceneComposedNodeItem*>(sni))
          res = scni->hasExpandedChildren(recursively);
    }
  }
  return res;
}

void SceneComposedNodeItem::shrinkExpandRecursive(bool toExpand, bool fromHere, ShrinkMode theShrinkMode)
{
  DEBTRACE("SceneComposedNodeItem::shrinkExpandRecursive " << isExpanding << " " << fromHere << " " << isExpanded() << " " << _label.toStdString());
  
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
    toChangeShrinkState = false;
    break;
  }
  if (toChangeShrinkState) {
    if (toExpand != isExpanded())
      setExpanded(toExpand);
  } else if (!isExpanded() && theShrinkMode == ElementaryNodes) {
    return;
  }

  updateControlLinks(toExpand);

  if (!toExpand)
    { // ---collapsing: hide first children , then resize
      for (list<AbstractSceneItem*>::const_iterator it=_children.begin(); it!=_children.end(); ++it)
        {
          SceneItem* item = dynamic_cast<SceneItem*>(*it);
          item->shrinkExpandRecursive(toExpand, false, theShrinkMode);
          if (theShrinkMode != ElementaryNodes) {
            item->hide();  
            DEBTRACE("------------------------------- Hide " << item->getLabel().toStdString());
            item->shrinkExpandLink(false);  
          }
        }

      if (toChangeShrinkState || theShrinkMode != ElementaryNodes) {
        if (_shownState == expandShown)
          {
             _expandedWidth = _width;
             _expandedHeight = _height;
          }

        if (fromHere)
          {
            _shownState = shrinkShown;
          }
        else
          {
            _ancestorShrinked = true;
            _shownState = shrinkHidden;
          }

        _width  = 2*Resource::Corner_Margin + 2*Resource::DataPort_Width + Resource::Space_Margin;
        if (_shownState == shrinkShown)
          _height = getHeaderBottom() + Resource::Corner_Margin;
        else
          _height = Resource::Header_Height + Resource::Corner_Margin;
      
        if (_shownState == shrinkHidden) // shrink of ancestor
          setPos(0 ,0);
        else
          setPos(_expandedPos);
        adjustHeader();
        if (_progressItem)
          _progressItem->adjustGeometry();
      }
    }
  else
    { // --- expanding: resize, then show children
      if (toChangeShrinkState)
        _ancestorShrinked = false;

      for (list<AbstractSceneItem*>::const_iterator it=_children.begin(); it!=_children.end(); ++it)
        {
          SceneItem* item = dynamic_cast<SceneItem*>(*it);
          item->shrinkExpandRecursive(isExpanded(), false, theShrinkMode); 
          if (theShrinkMode != ElementaryNodes) {
            if (isExpanded())
              {
                item->show();  
                DEBTRACE("------------------------------- Show " << item->getLabel().toStdString());
              }
            else
              {
                item->hide();  
                DEBTRACE("------------------------------- Hide " << item->getLabel().toStdString());
              }
            item->shrinkExpandLink(fromHere);  
          }
        }

      if (isExpanded())
        {
          _width = _expandedWidth;
          _height = _expandedHeight;
          _shownState = expandShown;
        }
      else
        {
          _shownState = shrinkShown;
          _width  = 2*Resource::Corner_Margin + 2*Resource::DataPort_Width + Resource::Space_Margin;
          _height = getHeaderBottom() + Resource::Corner_Margin;
        }
      setPos(_expandedPos);
      adjustHeader();
      if (_progressItem)
        _progressItem->adjustGeometry();
    }
}

void SceneComposedNodeItem::shrinkExpandLink(bool se) {
  DEBTRACE("SceneComposedNodeItem::shrinkExpandLink " << se << " "  << _label.toStdString());
  se = se && isExpanded();
  foreach (QGraphicsItem *child, childItems()) {
    if (SceneItem *sci = dynamic_cast<SceneItem*>(child)) {
      sci->shrinkExpandLink(se);
    };
  };
}

void SceneComposedNodeItem::reorganize()
{
  DEBTRACE("SceneComposedNodeItem::reorganize() " << _label.toStdString());
  list<AbstractSceneItem*> alreadySet;
  for (list<AbstractSceneItem*>::const_iterator it=_children.begin(); it!=_children.end(); ++it)
    {
      autoPosNewChild(*it, alreadySet);
      alreadySet.push_back(*it);
    }
}

void SceneComposedNodeItem::setShownState(shownState ss)
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
  adjustHeader();
  if (_progressItem)
    _progressItem->adjustGeometry();
}

void SceneComposedNodeItem::collisionResolv(SceneItem* child, QPointF oldPos)
{
  //DEBTRACE("SceneComposedNodeItem::collisionResolv " << _label.toStdString());
  for (list<AbstractSceneItem*>::const_iterator it=_children.begin(); it!=_children.end(); ++it)
    {
      SceneNodeItem *other = dynamic_cast<SceneNodeItem*>(*it);
      if (other && (other != child))
        {
          if (child->collidesWithItem(other))
            {
              //DEBTRACE("collision detected with " << other->getLabel().toStdString());
              QRectF otherBR = (other->mapToParent(other->boundingRect())).boundingRect();
              qreal oldX = oldPos.x();
              qreal oldY = oldPos.y();
              qreal newX = child->pos().x();
              qreal newY = child->pos().y();
              qreal othX = otherBR.left()+0.5;
              qreal othY = otherBR.top()+0.5;
              //DEBTRACE("oldX=" << oldX << " oldY=" << oldY << " newX=" << newX << " newY=" << newY);
              //DEBTRACE("otherLeft=" << otherBR.left() << " otherRight=" << otherBR.right() <<
              //         " otherTop=" << otherBR.top() << " otherBottom=" << otherBR.bottom());
              //DEBTRACE("width=" << child->boundingRect().width() <<
              //         " height=" << child->boundingRect().height());
              bool fromTop    = (((oldY + child->boundingRect().height()) <= otherBR.top()+1) &&
                                 ((newY + child->boundingRect().height()) >= otherBR.top()));
              bool fromBottom = (( oldY >= otherBR.bottom()) &&
                                 ( newY <= otherBR.bottom()));
              bool fromRight  = (( oldX >= otherBR.right()) &&
                                 ( newX <= otherBR.right()));
              bool fromLeft   = (((oldX+ child->boundingRect().width()) <= otherBR.left()+1) &&
                                 ((newX+ child->boundingRect().width()) >= otherBR.left()));
              //DEBTRACE("fromTop=" << fromTop << " fromBottom=" << fromBottom 
              //         << " fromRight=" << fromRight << " fromLeft=" << fromLeft);
              bool pushOther =false;
              bool blocThis = false;
              if (fromTop)
                {
                  othY = newY + child->boundingRect().height();
                  pushOther = true;
                  other->_blocY = false;
                }
              if (fromLeft)
                {
                  othX = newX+ child->boundingRect().width();
                  pushOther = true;
                  other->_blocX = false;
                }
              if (fromBottom)
                {
                  if (other->_blocY)
                    {
                      newY = otherBR.bottom() + 1;
                      blocThis = true;
                      _blocY = true;
                    }
                  else
                    {
                      othY = newY - otherBR.height();
                      if ( othY < Resource::Space_Margin + getHeaderBottom() )
                        {
                          othY = Resource::Space_Margin + getHeaderBottom();
                          other->_blocY = true;
                          newY = otherBR.bottom() + 1;
                          _blocY = true;
                          blocThis = true;
                        }
                      else
                        pushOther = true;
                    }
                }
              if (fromRight)
                {
                  if (other->_blocX)
                    {
                      newX = otherBR.right()+ 1;
                      blocThis = true;
                      _blocX = true;
                    }
                  else
                    {
                      othX = newX - otherBR.width();
                      if (othX < Resource::Space_Margin)
                        {
                          othX = Resource::Space_Margin;
                          other->_blocX = true;
                          newX = otherBR.right()+ 1;
                          _blocX = true;
                          blocThis = true;
                        }
                      else
                        pushOther = true;
                    }
                }
              //DEBTRACE("newX=" << newX << " newY=" << newY);
              if (blocThis) child->setTopLeft(QPointF(newX, newY));
              if (pushOther)
                {
                  other->setTopLeft(QPointF(othX, othY));
                }
            }
        }
    } 
}

void SceneComposedNodeItem::rebuildLinks()
{
  DEBTRACE("SceneComposedNodeItem::rebuildLinks " << QtGuiContext::_delayCalc);
  if (QtGuiContext::_delayCalc)
    return;
  CHRONO(1);
  CHRONO(2);
  LinkMatrix matrix(this);
  matrix.compute();
  CHRONOSTOP(2);
  CHRONO(3);
  list<linkdef> alist = matrix.getListOfDataLinkDef();
  list<linkdef> blist = matrix.getListOfCtrlLinkDef(); // add list operator ?
  for (list<linkdef>::const_iterator ii = blist.begin(); ii != blist.end(); ++ii)
    alist.push_back(*ii);

  CHRONOSTOP(3);
  CHRONO(4);
  LinkAStar astar(matrix);
  for (list<linkdef>::const_iterator it = alist.begin(); it != alist.end(); ++it)
    {
      linkdef ali = *it;
      DEBTRACE("from("<<ali.from.first<<","<<ali.from.second
               <<") to ("<<ali.to.first<<","<<ali.to.second
               <<") " << ali.item->getLabel().toStdString());
      if(ali.from.first<0||ali.from.second<0||ali.to.first<0||ali.to.second<0) continue;
      CHRONO(5);
      bool isPath = astar.computePath(LNode(ali.from), LNode(ali.to));
      CHRONOSTOP(5);
      CHRONO(6);
      if (! isPath) DEBTRACE("Link Path not found !");
      if (! isPath) continue;
      LNodePath ijPath = astar.givePath();
      if(Scene::_addRowCols)
        matrix.incrementCost(ijPath);
      LinkPath apath = matrix.getPath(ijPath);
//       DEBTRACE(apath.size());
      CHRONOSTOP(6);
      CHRONO(7);
      ali.item->setPath(apath);
    }
}

void SceneComposedNodeItem::arrangeNodes(bool isRecursive)
{
  DEBTRACE("SceneComposedItem::arrangeNodes " << isRecursive);

  bool isExtern = !QtGuiContext::_delayCalc;
  QtGuiContext::_delayCalc = true; // avoid rebuildLinks

  SubjectComposedNode *scnode = dynamic_cast<SubjectComposedNode*>(getSubject());
  YASSERT(scnode);
  ComposedNode *cnode = dynamic_cast<ComposedNode*>(scnode->getNode());
  YASSERT(cnode);

  if (isRecursive)
    {
      list<Node*> children = cnode->edGetDirectDescendants();
      for (list<Node*>::iterator it = children.begin(); it != children.end(); ++it)
        {
          DEBTRACE("child " << (*it)->getName());
          SceneComposedNodeItem *scni = 0;
          ComposedNode *cchild = dynamic_cast<ComposedNode*>(*it);
          if (cchild)
            {
              SubjectNode* sn = GuiContext::getCurrent()->_mapOfSubjectNode[cchild];
              SceneItem* sci = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sn];
              if (sci) scni = dynamic_cast<SceneComposedNodeItem*>(sci);
            }
          if (scni && (scni->getShownState() == expandShown))
            {
              DEBTRACE("call arrangeNode on child " << (*it)->getName());
              scni->arrangeNodes(isRecursive);
            }
        }
      arrangeChildNodes();
    }
  else
    arrangeChildNodes();

  if (isExtern)
    {
      QtGuiContext::_delayCalc = false; // allow rebuildLinks
      if (Scene::_autoComputeLinks)
        rebuildLinks();
    }
}

void SceneComposedNodeItem::arrangeChildNodes()
{
  reorganize();
}

void SceneComposedNodeItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
  bool accepted = false;
  const ItemMimeData *myData = dynamic_cast<const ItemMimeData*>(event->mimeData());
  if (myData)
    {
      if (myData->hasFormat("yacs/cataService") ||
          myData->hasFormat("yacs/cataNode")    ||
          myData->hasFormat("yacs/subjectNode"))
        {
          event->setAccepted(true);
          _dragOver = true;
          QGraphicsItem::update();
          return;
        }
    }
  event->setAccepted(accepted);
}

void SceneComposedNodeItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event);
    _dragOver = false;
    QGraphicsItem::update();
}

void SceneComposedNodeItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
  Q_UNUSED(event);
  _dragOver = false;
  QGraphicsItem::update();

  const ItemMimeData *myData = dynamic_cast<const ItemMimeData*>(event->mimeData());
  if (!myData) return;
  setEventPos(event->scenePos());
  if (myData->hasFormat("yacs/cataService") || myData->hasFormat("yacs/cataNode"))
    {
      SubjectComposedNode *cnode = dynamic_cast<SubjectComposedNode*>(getSubject());
      bool createNewComponentInstance=Resource::COMPONENT_INSTANCE_NEW;
      // by default getControl gives false. In this case we use the user preference COMPONENT_INSTANCE_NEW
      // to create the node. If getControl gives true we invert the user preference
      if(myData->getControl())
        createNewComponentInstance=!Resource::COMPONENT_INSTANCE_NEW;
      QtGuiContext::getQtCurrent()->getGMain()->_guiEditor->CreateNodeFromCatalog(myData, cnode,createNewComponentInstance);
    }
  else if(myData->hasFormat("yacs/subjectNode"))
    {
      Subject *sub = myData->getSubject();
      if (!sub) return;
      SubjectNode *node = dynamic_cast<SubjectNode*>(sub);
      if (!node) return;
      if (dynamic_cast<SubjectProc*>(node)) return; // --- do not reparent proc !
      SubjectComposedNode *cnode = dynamic_cast<SubjectComposedNode*>(getSubject());
      if (cnode)
        if (!node->reparent(cnode))
          Message mess;
    }
}

QColor SceneComposedNodeItem::getPenColor()
{
  if (_dragOver)
    return Resource::dragOver;
  if (isSelected())
    return _hiPenColor;
  else 
    return _penColor;
}

QColor SceneComposedNodeItem::getBrushColor()
{
  if (_dragOver)
    return _hiBrushColor;

  QColor color;
  if (isSelected())
    color = _hiBrushColor;
  else if (_emphasized)
    color = Resource::emphasizeBrushColor;
  else 
    color = _brushColor;

  if (_hover)
    color = hoverColor(color);
  return color;
}

void SceneComposedNodeItem::updateChildItems()
{
  SceneNodeItem::updateChildItems();
  if(!_header)
    return;
  foreach (QGraphicsItem *child, _header->childItems())
    {
      if (SceneItem *sci = dynamic_cast<SceneItem*>(child))
        {
           sci->updateLinks();
        }
    }
}

