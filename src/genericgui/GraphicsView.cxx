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

#include "GraphicsView.hxx"
#include "SchemaModel.hxx"
#include "SceneItem.hxx"
#include "SceneTextItem.hxx"
#include "Scene.hxx"
#include "QtGuiContext.hxx"
#include "guiObservers.hxx"

#include <cmath>
#include <QMenu>
#include <QGraphicsView>
#include <QWheelEvent>

#include <cmath>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;

GraphicsView::GraphicsView(QWidget *parent)
  : WrapGraphicsView(parent)
{
  _zooming = false;
  _fittingArea = false;
  _panning = false;
  _rect = 0;
  setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}

GraphicsView::~GraphicsView()
{
}

void GraphicsView::onViewFitAll()
{
  DEBTRACE("GraphicsView::onViewFitAll");
  SubjectProc *sProc = QtGuiContext::getQtCurrent()->getSubjectProc();
  SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sProc];
  //SceneProcItem *procItem = dynamic_cast<SceneProcItem*>(item);
  fitInView(item->boundingRect(),  Qt::KeepAspectRatio);
}

void GraphicsView::onViewFitArea()
{
  DEBTRACE("GraphicsView::onViewFitArea");
  Scene* myScene = dynamic_cast<Scene*>(scene());
  myScene->setZoom(true);
  _fittingArea = true;
}

void GraphicsView::onViewZoom()
{
  DEBTRACE("GraphicsView::onViewZoom");
  Scene* myScene = dynamic_cast<Scene*>(scene());
  myScene->setZoom(true);
  _zooming =true;
  _scale = 1;
}

void GraphicsView::onViewPan()
{
  DEBTRACE("GraphicsView::onViewPan");
  Scene* myScene = dynamic_cast<Scene*>(scene());
  myScene->setZoom(true);
  _panning = true;
  //setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  setTransformationAnchor(QGraphicsView::NoAnchor);
}

void GraphicsView::onViewGlobalPan()
{
  DEBTRACE("GraphicsView::onViewGlobalPan");
}

void GraphicsView::onViewReset()
{
  DEBTRACE("GraphicsView::onViewReset");
  resetTransform();
}


void GraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
  QGraphicsItem *qgitem = itemAt(event->pos());
  if (qgitem)
    {
      SceneItem *item = dynamic_cast<SceneItem*>(qgitem);
      if (item)
        {
          QPointF point = mapToScene(event->pos());
          item->setEventPos(point);
          item->popupMenu(this, event->globalPos());
        }
      else
        {
          SceneTextItem *item = dynamic_cast<SceneTextItem*>(qgitem);
          if (item)
            {
              QPointF point = mapToScene(event->pos());
              item->setEventPos(point);
              item->popupMenu(this, event->globalPos());
            }
        }
    }
}

void GraphicsView::mouseMoveEvent(QMouseEvent *e)
{
  WrapGraphicsView::mouseMoveEvent(e);
  if (e->buttons()==Qt::LeftButton)
    {
      QPoint current = e->pos();
      if (_zooming)
        {
          qreal currentX = e->globalX();
          qreal currentY = e->globalY();
          qreal delta = sqrt((currentX - _prevX)*(currentX - _prevX) + (currentY - _prevY)*(currentY - _prevY));
          if ((currentX*currentX + currentY*currentY) < (_prevX*_prevX + _prevY*_prevY))
            delta = -delta;
          _prevX = currentX;
          _prevY = currentY;
          //       if (delta < -30) delta = -30;
          //       if (delta >  30) delta =  30;
          double deltax = delta/900.;
          double zoom = exp(deltax);
          _scale = _scale*zoom;
          scale(zoom,zoom);
          //DEBTRACE("move zooming " << delta << " " << deltax << " " << zoom);
        }
      else if (_panning)
        {
          translate(current.x() - _prevPos.x(), current.y() - _prevPos.y());
          //DEBTRACE(current.x()<<"-"<<_prevPos.x()<<" "<<current.y()<<"-"<<_prevPos.y());
          _prevPos = current;
        }
      else if (_fittingArea)
        {
          if (!_rect)
            {
              _rect = scene()->addRect(QGraphicsView::mapToScene(QRect(_prevPos, current)).boundingRect());
              _rect->setZValue(100000);
              _rect->setParentItem(0);
            }
          else
            {
              _rect->setRect(QGraphicsView::mapToScene(QRect(_prevPos, current)).boundingRect());
              _rect->show();
            }
        }
    }
}

void GraphicsView::mousePressEvent(QMouseEvent *e)
{
  DEBTRACE("GraphicsView::mousePressEvent");
  if (QtGuiContext::getQtCurrent()->getView() != this)
    {
      DEBTRACE("Switch context before selection");
      QtGuiContext::getQtCurrent()->getGMain()->switchContext(this->parentWidget());
    }
  WrapGraphicsView::mousePressEvent(e);
  if (_zooming)
    {
      _prevX = e->globalX();
      _prevY = e->globalY();
    }
  else if (_fittingArea)
    {
      _prevPos = e->pos();
    }
    else if (_panning)
      {
        _prevPos = e->pos();
        //setDragMode(QGraphicsView::ScrollHandDrag);
      }
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *e)
{
  _zooming = false;
  _panning = false;
  setDragMode(QGraphicsView::NoDrag);
  setTransformationAnchor(QGraphicsView::AnchorViewCenter);
  if( _fittingArea)
    {
      _fittingArea = false;
      QPoint current = e->pos();
      fitInView(QGraphicsView::mapToScene(QRect(_prevPos, current)).boundingRect(), Qt::KeepAspectRatio);
      if (_rect)
        _rect->hide();
    }

  QTransform q = transform();
  DEBTRACE(q.m11()<<" "<<q.m12()<<" "<<q.m21()<<" "<<q.m22()<<" "<<q.dx()<<" "<<q.dy());
  WrapGraphicsView::mouseReleaseEvent(e);
}

void GraphicsView::wheelEvent(QWheelEvent *e)
{
  DEBTRACE("GraphicsView::wheelEvent " << e->delta());
  double zoom = exp(0.1*e->delta()/120);
  _scale = _scale*zoom;
  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  scale(zoom,zoom);
}

void GraphicsView::onZoomToBloc()
{
  Subject *sub = QtGuiContext::getQtCurrent()->getSelectedSubject();
  SubjectNode *snode = dynamic_cast<SubjectNode*>(sub);
  if (!snode)
    return;
  SubjectComposedNode *scnode = dynamic_cast<SubjectComposedNode*>(sub);
  if (!scnode)
    scnode = dynamic_cast<SubjectComposedNode*>(snode->getParent());
  YASSERT(scnode);
  SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[scnode];
  YASSERT(item);
  QRectF rect = item->mapToScene(item->boundingRect().toRect()).boundingRect();
  fitInView(rect.toRect(), Qt::KeepAspectRatio);
}

void GraphicsView::onCenterOnNode()
{
  Subject *sub =QtGuiContext::getQtCurrent()->getSelectedSubject();
  SubjectNode *snode = dynamic_cast<SubjectNode*>(sub);
  if (!snode)
    return;
  SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sub];
  centerOn(item);
}
