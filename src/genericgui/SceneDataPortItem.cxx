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

#include "SceneDataPortItem.hxx"
#include "SceneTextItem.hxx"
#include "SceneNodeItem.hxx"
#include "SceneLinkItem.hxx"
#include "Scene.hxx"

#include "QtGuiContext.hxx"

#include "Resource.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;


SceneDataPortItem::SceneDataPortItem(QGraphicsScene *scene, SceneItem *parent,
                                     QString label, Subject *subject)
  : SceneObserverItem(scene, parent, label, subject), ScenePortItem(label)
{
  setText(label);
  _width        = Resource::DataPort_Width;
  _height       = Resource::DataPort_Height;
  switch (getSubject()->getType())
    {
    case INPUTPORT:
    case OUTPUTPORT:
      _brushColor   = Resource::DataPort_brush;
      _hiBrushColor = Resource::DataPort_hiBrush;
      _penColor     = Resource::DataPort_pen;
      _hiPenColor   = Resource::DataPort_hiPen;
      break;
    case INPUTDATASTREAMPORT:
    case OUTPUTDATASTREAMPORT:
      _brushColor   = Resource::DataStreamPort_brush;
      _hiBrushColor = Resource::DataStreamPort_hiBrush;
      _penColor     = Resource::DataStreamPort_pen;
      _hiPenColor   = Resource::DataStreamPort_hiPen;
      break;
    }
}

SceneDataPortItem::~SceneDataPortItem()
{
  DEBTRACE("ScenePortItem::~SceneDataPortItem");
}

void SceneDataPortItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
  DEBTRACE("ScenePortItem::paint");
  painter->save();

  QPen pen(getPenColor());
  pen.setWidth(Resource::Thickness);
  painter->setPen(pen);
  painter->setBrush(getBrushColor());
  QRectF rect(0, 0, Resource::DataPort_Width, Resource::DataPort_Height);
  painter->drawRoundedRect(rect, Resource::Radius, Resource::Radius);

  painter->restore();
}

void SceneDataPortItem::setText(QString label)
{
  if (!_text)
    _text = new SceneTextItem(_scene,
                              this,
                              label );
}

void SceneDataPortItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SceneDataPortItem::update "<< eventName(event)<<" "<<type<<" "<<son);
  SceneObserverItem::update(event, type, son);
  switch (event)
    {
    case YACS::HMI::RENAME:
      _text->setPlainTextTrunc(son->getName().c_str());
      QGraphicsItem::update();
      break;
    case YACS::HMI::REMOVE:
      SceneObserverItem::update(event, type, son);
      break;
    }
}

SceneNodeItem* SceneDataPortItem::getParent()
{
  if (_parent)
    return dynamic_cast<SceneNodeItem*>(_parent);
  else
    return 0;
}

void SceneDataPortItem::updateChildItems()
{
  DEBTRACE("SceneDataPortItem::updateChildItems " << _label.toStdString());
  if(SubjectDataPort* sdp=dynamic_cast<SubjectDataPort*>(_subject))
    {
      std::list<SubjectLink*> lsl=sdp->getListOfSubjectLink();
      for (std::list<SubjectLink*>::const_iterator it = lsl.begin(); it != lsl.end(); ++it)
        {
          SceneLinkItem* item = dynamic_cast<SceneLinkItem*>(QtGuiContext::getQtCurrent()->_mapOfSceneItem[*it]);
          item->updateShape();
        }
    }
}

void SceneDataPortItem::updateLinks()
{
  DEBTRACE("SceneDataPortItem::updateLinks " << _label.toStdString());
  if(SubjectDataPort* sdp=dynamic_cast<SubjectDataPort*>(_subject))
    {
      std::list<SubjectLink*> lsl=sdp->getListOfSubjectLink();
      for (std::list<SubjectLink*>::const_iterator it = lsl.begin(); it != lsl.end(); ++it)
        {
          SceneLinkItem* item = dynamic_cast<SceneLinkItem*>(QtGuiContext::getQtCurrent()->_mapOfSceneItem[*it]);
          item->updateShape();
        }
    }
}

void SceneDataPortItem::shrinkExpandLink(bool se)
{
  if(SubjectDataPort* sdp=dynamic_cast<SubjectDataPort*>(_subject))
    {
      std::list<SubjectLink*> lsl=sdp->getListOfSubjectLink();
      for (std::list<SubjectLink*>::const_iterator it = lsl.begin(); it != lsl.end(); ++it)
        {
          SceneLinkItem* item = dynamic_cast<SceneLinkItem*>(QtGuiContext::getQtCurrent()->_mapOfSceneItem[*it]);
          if (se) {
            item->show();
          } else {
            item->hide();
          };
        }
    }
}
