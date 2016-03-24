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

#include "SceneDSLinkItem.hxx"
#include "Resource.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SceneDSLinkItem::SceneDSLinkItem(QGraphicsScene *scene, SceneItem *parent,
                                 ScenePortItem* from, ScenePortItem* to,
                                 QString label, Subject *subject)
  : SceneLinkItem(scene, parent, from, to, label, subject)
{
  _penColor     = Resource::stream_link_draw_color.darker(Resource::link_pen_darkness);
  _hiPenColor   = Resource::stream_link_select_color.darker(Resource::link_pen_darkness);
  _brushColor   = Resource::stream_link_draw_color;
  _hiBrushColor = Resource::stream_link_select_color;
}

SceneDSLinkItem::~SceneDSLinkItem()
{
}

QColor SceneDSLinkItem::getPenColor()
{
  _penColor     = Resource::stream_link_draw_color.darker(Resource::link_pen_darkness);
  _hiPenColor   = Resource::stream_link_select_color.darker(Resource::link_pen_darkness);
  return SceneObserverItem::getPenColor();
}

QColor SceneDSLinkItem::getBrushColor()
{
  _brushColor   = Resource::stream_link_draw_color;
  _hiBrushColor = Resource::stream_link_select_color;
  return SceneObserverItem::getBrushColor();
}
