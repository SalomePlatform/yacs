
#include "SceneComposedNodeItem.hxx"
#include "SceneElementaryNodeItem.hxx"
#include "SceneHeaderItem.hxx"
#include "SceneHeaderNodeItem.hxx"
#include "SceneInPortItem.hxx"
#include "SceneOutPortItem.hxx"
#include "SceneLinkItem.hxx"
#include "SceneCtrlLinkItem.hxx"
#include "LinkMatrix.hxx"
#include "LinkAStar.hxx"
#include "ItemMimeData.hxx"
#include "QtGuiContext.hxx"
#include "Menus.hxx"
#include "guiObservers.hxx"
#include "GuiEditor.hxx"
#include "Scene.hxx"

#include <QPointF>
#include <QGraphicsSceneDragDropEvent>

#include <cassert>

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
  _brushColor   = QColor(213,213,213);
  _hiBrushColor = QColor(225,225,225);
  _penColor     = QColor(120,120,120);
  _hiPenColor   = QColor( 60, 60, 60);
  _dragOver = false;
  setAcceptDrops(true);
}

SceneComposedNodeItem::~SceneComposedNodeItem()
{
}

QRectF SceneComposedNodeItem::childrenBoundingRect() const
{
  QRectF ChildrenBRect =QRectF(x(), y(), 5, 5);
  if (_header) ChildrenBRect = _header->getMinimalBoundingRect();
  for (list<AbstractSceneItem*>::const_iterator it=_children.begin(); it!=_children.end(); ++it)
    ChildrenBRect = ChildrenBRect.united(childBoundingRect(*it));
  return ChildrenBRect;
}

void SceneComposedNodeItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
  //DEBTRACE("SceneComposedNodeItem::paint " << _label.toStdString());
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

void SceneComposedNodeItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SceneComposedNodeItem::update "<< event<<" "<<type<<" "<<son);
  SceneNodeItem::update(event, type, son);
  AbstractSceneItem *item;
  switch (event)
    {
    case YACS::HMI::ADD:
      switch (type)
        {
        case YACS::HMI::BLOC:
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
          break;
        case YACS::HMI::OUTPUTPORT:
        case YACS::HMI::OUTPUTDATASTREAMPORT:
          item =  new SceneOutPortItem(_scene,
                                       _header,
                                       son->getName().c_str(),
                                       son);
          _header->autoPosNewPort(item);
          _outPorts.push_back(item);
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
              item = new SceneLinkItem(_scene,
                                       this,
                                       from, to,
                                       son->getName().c_str(),
                                       son);
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
              item = new SceneCtrlLinkItem(_scene,
                                           this,
                                           from, to,
                                           son->getName().c_str(),
                                           son);
            }
          break;
        }
      break;
    case YACS::HMI::REMOVE:
      //SceneObserverItem::update(event, type, son);
      reorganize();
      break;
    default:
      DEBTRACE("SceneComposedNodeItem::update(), event not handled: << event");
    }
}

void SceneComposedNodeItem::autoPosNewChild(AbstractSceneItem *item,
                                            const std::list<AbstractSceneItem*> alreadySet,
                                            bool isNew)
{
  QRectF childrenBox;
  qreal xLeft = _margin + _nml;
  qreal yTop  = getHeaderBottom() + _margin + _nml;
  for (list<AbstractSceneItem*>::const_iterator it=alreadySet.begin(); it!=alreadySet.end(); ++it)
    {
      childrenBox = childrenBox.united(childBoundingRect(*it));
      DEBTRACE((*it)->getLabel().toStdString());
      DEBTRACE("childrenBox valid " << childrenBox.right() << " " << childrenBox.bottom());
    }
  if (childrenBox.isValid())
    xLeft += childrenBox.right();
  DEBTRACE("left, top " << xLeft  << " " << yTop);
  QPointF topLeft(xLeft, yTop);
  if (isNew) _children.push_back(item);
  item->setTopLeft(topLeft);
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
              //oldPos = mapFromScene(oldPos);
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
                  //newY = otherBR.top() - child->boundingRect().height() -_margin;
                  othY = newY + child->boundingRect().height();
                  pushOther = true;
                }
              if (fromLeft)
                {
                  //newX = otherBR.left() -child->boundingRect().width() - _margin;
                  othX = newX+ child->boundingRect().width();
                  pushOther = true;
                }
              if (fromBottom)
                {
                  newY = otherBR.bottom() + 1;
                  blocThis = true;
                }
              if (fromRight)
                {
                  newX = otherBR.right()+ 1;
                  blocThis = true;
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
  LinkMatrix matrix(this);
  matrix.compute();
  list<linkdef> alist = matrix.getListOfDataLinkDef();
  list<linkdef> blist = matrix.getListOfCtrlLinkDef(); // add list operator ?
  for (list<linkdef>::const_iterator ii = blist.begin(); ii != blist.end(); ++ii)
    alist.push_back(*ii);

  LinkAStar astar(matrix);
  for (list<linkdef>::const_iterator it = alist.begin(); it != alist.end(); ++it)
    {
      linkdef ali = *it;
//       DEBTRACE("from("<<ali.from.first<<","<<ali.from.second
//                <<") to ("<<ali.to.first<<","<<ali.to.second
//                <<") " << ali.item->getLabel().toStdString());
      bool isPath = astar.computePath(LNode(ali.from), LNode(ali.to));
      if (! isPath) DEBTRACE("Link Path not found !");
      LinkPath apath = matrix.getPath(astar.givePath());
//       DEBTRACE(apath.size());
      ali.item->setPath(apath);
    }

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
  if (myData->hasFormat("yacs/cataService") || myData->hasFormat("yacs/cataNode"))
    {
      SubjectComposedNode *cnode = dynamic_cast<SubjectComposedNode*>(getSubject());
      QtGuiContext::getQtCurrent()->getGMain()->_guiEditor->CreateNodeFromCatalog(myData, cnode);
    }
  else if(myData->hasFormat("yacs/subjectNode"))
    {
      Subject *sub = myData->getSubject();
      if (!sub) return;
      SubjectNode *node = dynamic_cast<SubjectNode*>(sub);
      if (!node) return;
      SubjectComposedNode *cnode = dynamic_cast<SubjectComposedNode*>(getSubject());
      if (cnode) node->reparent(cnode);
    }
}

QColor SceneComposedNodeItem::getPenColor()
{
  if (_dragOver)
    return QColor(255,0,0);
  if (isSelected())
    return _hiPenColor;
  else 
    return _penColor;
}

QColor SceneComposedNodeItem::getBrushColor()
{
  if (_dragOver)
    return _hiBrushColor;
  if (isSelected())
    return _hiBrushColor;
  else 
    return _brushColor;
}
