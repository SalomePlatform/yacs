
#include "SceneObserverItem.hxx"
#include "QtGuiContext.hxx"
#include "ItemMimeData.hxx"
#include "Scene.hxx"

#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QPixmap>
#include <QBitmap>
#include <QPainter>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;


SceneObserverItem::SceneObserverItem(QGraphicsScene *scene, SceneItem *parent,
                                     QString label, Subject *subject)
  : SceneItem(scene, parent, label), GuiObserver()
{
  _subject = subject;
  _draging = false;
  _subject->attach(this);
  QtGuiContext::getQtCurrent()->_mapOfSceneItem[_subject]=this;
}

SceneObserverItem::~SceneObserverItem()
{
  QtGuiContext::getQtCurrent()->_mapOfSceneItem.erase(_subject);
}

void SceneObserverItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE(" SceneObserverItem::update " << event<< " " << type << " " << son);
}

void SceneObserverItem::select(bool isSelected)
{
  DEBTRACE("SceneObserverItem::select "  << _label.toStdString() << " " << isSelected);
  if (isSelected)
    {
//       _scene->clearSelection();
      setSelected(true);
      QtGuiContext::getQtCurrent()->setSelectedSubject(_subject);
    }
  else setSelected(false);
}

void SceneObserverItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  DEBTRACE("SceneObserverItem::mousePressEvent " << _label.toStdString()
           << " " << acceptedMouseButtons ());
  if (!_scene->isZooming())
    {
      _subject->select(true);
      if (_dragable && (event->button() == _dragButton))
        {
          setCursor(Qt::ClosedHandCursor);
          _draging = true;
        }
    }
}

/*!
 * creation of mime data if drag detected.
 * setData mime type must be coherent with SchemaModel::mimeTypes to allow drop 
 * on port item in tree view
 */
void SceneObserverItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  if (_draging)
    {
      //DEBTRACE("_draging");
      if (QLineF(event->screenPos(), 
                 event->buttonDownScreenPos(_dragButton)).length()
          < QApplication::startDragDistance())
        {
          return;
        }
      DEBTRACE("in drag");
      QDrag *drag = new QDrag(event->widget());
      ItemMimeData *mime = new ItemMimeData;
      drag->setMimeData(mime);
      mime->setSubject(_subject);
      mime->setData(getMimeFormat(), "_subject");
  
      QPixmap pixmap(34, 34);
      pixmap.fill(Qt::white);
      
      QPainter painter(&pixmap);
      painter.translate(15, 15);
      painter.setRenderHint(QPainter::Antialiasing);
      paint(&painter, 0, 0);
      painter.end();
      
      pixmap.setMask(pixmap.createHeuristicMask());
      
      drag->setPixmap(pixmap);
      drag->setHotSpot(QPoint(15, 20));
      
      drag->exec();
      setCursor(Qt::OpenHandCursor);
    }
}

void SceneObserverItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  if (_draging)
    {
      setCursor(Qt::ArrowCursor);
    }
  _draging = false;
}

void SceneObserverItem::activateSelection(bool selected)
{
  DEBTRACE("SceneObserverItem::activateSelection " << _label.toStdString()<< " " << selected);
  _subject->select(true);
}

Subject* SceneObserverItem::getSubject()
{
  return _subject;
}

QString SceneObserverItem::getMimeFormat()
{
  return "yacs/subject";
}
