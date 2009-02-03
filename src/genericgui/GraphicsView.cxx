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
#include "GraphicsView.hxx"
#include "SchemaModel.hxx"
#include "SceneItem.hxx"
#include "Scene.hxx"
#include "QtGuiContext.hxx"

#include <cmath>
#include <QMenu>
#include <QGraphicsView>

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
      AbstractSceneItem *item = dynamic_cast<AbstractSceneItem*>(qgitem);
      if (item)
        item->popupMenu(this, event->globalPos());
    }
}

void GraphicsView::mouseMoveEvent(QMouseEvent *e)
{
  WrapGraphicsView::mouseMoveEvent(e);
  if (e->buttons()==Qt::LeftButton)
    {
      if (_zooming)
        {
          qreal currentX = e->globalX();
          qreal delta = currentX - _prevX;
          _prevX = currentX;
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
          QPoint current = e->pos();
          translate(current.x() - _prevPos.x(), current.y() - _prevPos.y());
          //DEBTRACE(current.x()<<"-"<<_prevPos.x()<<" "<<current.y()<<"-"<<_prevPos.y());
          _prevPos = current;
        }
    }
}

void GraphicsView::mousePressEvent(QMouseEvent *e)
{
  WrapGraphicsView::mousePressEvent(e);
  if (_zooming)
    {
      _prevX = e->globalX();
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
      fitInView(QRect(_prevPos, current),  Qt::KeepAspectRatio);
    }

  QTransform q = transform();
  DEBTRACE(q.m11()<<" "<<q.m12()<<" "<<q.m21()<<" "<<q.m22()<<" "<<q.dx()<<" "<<q.dy());
  WrapGraphicsView::mouseReleaseEvent(e);
}
