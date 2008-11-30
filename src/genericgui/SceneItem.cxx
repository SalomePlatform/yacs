
#include "SceneItem.hxx"
#include "Scene.hxx"
#include "SceneNodeItem.hxx"
#include "SceneHeaderNodeItem.hxx"
#include "SceneProcItem.hxx"
#include "SceneComposedNodeItem.hxx"
#include "GuiEditor.hxx"

#include "QtGuiContext.hxx"
#include "Menus.hxx"
#include <QGraphicsSceneHoverEvent>
#include <QPointF>

#include <cassert>
#include <cmath>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

// ============================================================================

RootSceneItem::RootSceneItem(YACS::HMI::Subject *context)
{
  _context=context;
  _context->attach(this);
}

RootSceneItem::~RootSceneItem()
{
}

void RootSceneItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("RootSceneItem::update "<<eventName(event)<<" "<<type<<" "<<son);
  GuiEditor *guiEditor = 0;
  switch (event)
    {
    case YACS::HMI::NEWROOT:
      setNewRoot(son);
      break;
    case YACS::HMI::ENDLOAD:
      guiEditor = QtGuiContext::getQtCurrent()->getGMain()->_guiEditor;
      guiEditor->rebuildLinks();
      break;
//     default:
//       DEBTRACE("RootSceneItem::update(), event not handled: "<< eventName(event));
    }
}

void RootSceneItem::setNewRoot(YACS::HMI::Subject *root)
{
  DEBTRACE("RootSceneItem::setNewRoot");
  _root = root;
  QString name = _root->getName().c_str();
  QGraphicsScene* scene = QtGuiContext::getQtCurrent()->getScene();
  SceneProcItem *procItem = new SceneProcItem(scene, 0, name, root);
  scene->addItem(procItem);
  procItem->addHeader();
}

// ============================================================================

AbstractSceneItem::AbstractSceneItem(QGraphicsScene *scene, SceneItem *parent,
                                     QString label)
{
  _scene = dynamic_cast<Scene*>(scene);
  _parent = parent;
  _label = label;
  _level = 1;
  _margin = 4;
  _nml = 5;
  _width = 6;
  _height = 4;
  _penColor = QColor(0,0,128);
  _hiPenColor = QColor(0,0,190);
  _brushColor = QColor(128,128,128);
  _hiBrushColor = QColor(190,190,190);
  _hasHeader = false;
  _hasNml= false;
  _optimize = true; // to be set individually or globally by user (shrink items)
  _dragable = false;
  _dragButton = Qt::LeftButton;
  if (_parent) 
    _level = _parent->getLevel() +1;
  DEBTRACE("AbstractSceneItem::AbstractSceneItem "<<label.toStdString()
           <<" "<<this<<" "<<_parent<<" "<< _level); 
}

AbstractSceneItem::~AbstractSceneItem()
{
}

int AbstractSceneItem::getLevel()
{
  return  _level;
}

qreal AbstractSceneItem::getMargin()
{
  return  _margin;
}

void AbstractSceneItem::reorganize()
{
}

QString AbstractSceneItem::getLabel()
{
  return _label;
}

void AbstractSceneItem::addHeader()
{
}

qreal AbstractSceneItem::getHeaderBottom()
{
  return 0;
}
qreal AbstractSceneItem::getWidth()
{
  return _width;
}

qreal AbstractSceneItem::getHeight()
{
  return _height;
}

qreal AbstractSceneItem::getInternWidth()
{
  return _width -2*_margin -2*_hasNml*_nml;
}

qreal AbstractSceneItem::getInternHeight()
{
  return _height -2*_hasNml*_nml;
}

QRectF AbstractSceneItem::childBoundingRect(AbstractSceneItem *child) const
{
  QGraphicsItem *item = dynamic_cast<QGraphicsItem*>(child);
  assert(item);
  return (item->mapToParent(item->boundingRect())).boundingRect();
}

void AbstractSceneItem::activateSelection(bool selected)
{
  if (_parent) _parent->activateSelection(selected);
}

void AbstractSceneItem::setGeometryOptimization(bool optimize)
{
  _optimize = optimize;
}
// ============================================================================

SceneItem::SceneItem(QGraphicsScene *scene, SceneItem *parent,
                     QString label)
  : QGraphicsItem(parent), AbstractSceneItem(scene, parent, label)
{
  setToolTip(_label);
  DEBTRACE("SceneItem::SceneItem "<<label.toStdString()<<" "<<this<<" "<<_parent<<" "<< _level); 
  setFlag(QGraphicsItem::ItemIsSelectable);
  setAcceptsHoverEvents(true);
}

SceneItem::~SceneItem()
{
}

QRectF SceneItem::boundingRect() const
{
//   DEBTRACE("SceneItem::boundingRect " <<_label.toStdString()
//            <<" "<<_width<<" "<< _height);
  qreal penWidth = 1;
  return QRectF(- penWidth/2, - penWidth/2,
                _width + penWidth/2, _height + penWidth/2);
}

QRectF SceneItem::childrenBoundingRect() const
{
  return QGraphicsItem::childrenBoundingRect();
}

void SceneItem::paint(QPainter *painter,
                      const QStyleOptionGraphicsItem *option,
                      QWidget *widget)
{
  //DEBTRACE("SceneItem::paint");
  painter->save();
  painter->setPen(getPenColor());
  painter->setBrush(getBrushColor());
  painter->drawRoundRect(QRectF(0, 0, _width, _height), 33*_height/_width, 33);
  painter->restore();
}

void SceneItem::setTopLeft(QPointF topLeft)
{
  setPos(topLeft);
  if (_parent)
    _parent->checkGeometryChange();
}

void SceneItem::checkGeometryChange()
{
  QRectF childrenBox = childrenBoundingRect();
  qreal newWidth = childrenBox.width() + 2*_margin;
  qreal newHeight =  childrenBox.height() + 2*_margin;
  SceneNodeItem *aNode = dynamic_cast<SceneNodeItem*>(this);
  if (aNode)
    {
      newWidth  += 2*_nml;
      newHeight += 2*_nml;
    }
  bool resize = false;
  bool wider = (newWidth > _width + 0.5);
  qreal deltaW = 0;
  bool higher = (newHeight > _height + 0.5);
  qreal deltaH = 0;
  bool changeWidth = (fabs(newWidth - _width) > 0.5);

  if (wider || (_optimize && (newWidth < _width)))
    {
      deltaW = newWidth - _width;
      _width = newWidth;
      resize = true;
    }
  if (higher || (_optimize && (newHeight < _height)))
    {
      deltaH = newHeight - _height;
      _height = newHeight;
      resize = true;
    }
//   DEBTRACE("SceneItem::checkGeometryChange "<<_label.toStdString() <<
//            " " << wider << " " << higher << " " << changeWidth <<  " " << resize);
  if (aNode)
    {
      if (changeWidth) aNode->adjustHeader();
      if (wider || higher)
        {
          QPointF oldPos(pos().x() - deltaW, pos().y() - deltaH);
          if (SceneComposedNodeItem *bloc = dynamic_cast<SceneComposedNodeItem*>(_parent))
            bloc->collisionResolv(aNode, oldPos);
        }  
    }
  if (resize)
    { 
//       DEBTRACE("SceneItem::checkGeometryChange "<<_label.toStdString()<<" "<<_width<<" "<<_height);
      prepareGeometryChange();
      if (_parent)
        _parent->checkGeometryChange();
    }
}

// /*!
//  * When Zooming, filter all mouse events to items: 
//  * do not work, scene do not receive...
//  */
// bool SceneItem::sceneEvent(QEvent *event)
// {
//   if (_scene->isZooming())
//     return false;
//   else
//     return QGraphicsItem::sceneEvent(event);
// }

void SceneItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  DEBTRACE("SceneItem::mousePressEvent " << _label.toStdString()
           << " " << acceptedMouseButtons () << " " << _scene->isZooming());
  if (!_scene->isZooming()) activateSelection(true);
}

QColor SceneItem::getPenColor()
{
  if (isSelected())
    return _hiPenColor;
  else 
    return _penColor;
}

void SceneItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  update();
}

void SceneItem::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
}

void SceneItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  update();
}

QColor SceneItem::getBrushColor()
{
  if (isSelected())
    return _hiBrushColor;
  else 
    return _brushColor;
}

void SceneItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  MenusBase m;
  m.popupMenu(caller, globalPos);
}

