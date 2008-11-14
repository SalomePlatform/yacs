
#include "SceneInPortItem.hxx"
#include "SceneOutPortItem.hxx"
#include "SceneNodeItem.hxx"
#include "ItemMimeData.hxx"
#include "Menus.hxx"
#include "QtGuiContext.hxx"

#include <QGraphicsSceneDragDropEvent>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;


SceneInPortItem::SceneInPortItem(QGraphicsScene *scene, SceneItem *parent,
                                 QString label, Subject *subject)
  : SceneDataPortItem(scene, parent, label, subject)
{
  _dragOver = false;
  setAcceptDrops(true);
}

SceneInPortItem::~SceneInPortItem()
{
  if (SceneNodeItem* parent = getParent())
    parent->removeInPortFromList(this);
}

void SceneInPortItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  InPortMenu m;
  m.popupMenu(caller, globalPos);
}

void SceneInPortItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
  bool accepted = false;
  const ItemMimeData *myData = dynamic_cast<const ItemMimeData*>(event->mimeData());
  if (myData && myData->hasFormat("yacs/subjectOutPort"))
    {
      Subject *sub = myData->getSubject();
      if (sub && (dynamic_cast<SubjectOutputPort*>(sub) ||
                  dynamic_cast<SubjectOutputDataStreamPort*>(sub))) 
        {
          event->setAccepted(true);
          _dragOver = true;
          QGraphicsItem::update();
          return;
        }
    }
  event->setAccepted(accepted);
}

void SceneInPortItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event);
    _dragOver = false;
    QGraphicsItem::update();
}

void SceneInPortItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
  Q_UNUSED(event);
  _dragOver = false;
  QGraphicsItem::update();

  const ItemMimeData *myData = dynamic_cast<const ItemMimeData*>(event->mimeData());
  if (!myData) return;
  if(!myData->hasFormat("yacs/subjectOutPort")) return;
  Subject *sub = myData->getSubject();
  if (!sub) return;
  SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sub];
  if (!item) return;
  SceneOutPortItem* outItem = dynamic_cast<SceneOutPortItem*>(item);
  if (!outItem) return;

  Subject *subFrom = outItem->getSubject();
  Subject *subTo = this->getSubject();
  SubjectDataPort* from = dynamic_cast<SubjectDataPort*>(subFrom);
  SubjectDataPort* to = dynamic_cast<SubjectDataPort*>(subTo);
  if (from && to)
    SubjectDataPort::tryCreateLink(from, to);
}

QColor SceneInPortItem::getPenColor()
{
  if (_dragOver)
    return QColor(255,0,0);
  if (isSelected())
    return _hiPenColor;
  else 
    return _penColor;
}

QColor SceneInPortItem::getBrushColor()
{
  if (_dragOver)
    return _hiBrushColor;
  if (isSelected())
    return _hiBrushColor;
  else 
    return _brushColor;
}

