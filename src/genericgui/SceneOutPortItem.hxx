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

#ifndef _SCENEOUTPORTITEM_HXX_
#define _SCENEOUTPORTITEM_HXX_

#include "SceneDataPortItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SceneOutPortItem: public SceneDataPortItem
    {
    public:
      SceneOutPortItem(QGraphicsScene *scene, SceneItem *parent,
                       QString label, Subject *subject);
      virtual ~SceneOutPortItem();
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
    protected:
      virtual QString getMimeFormat();
    };
  }
}

#endif
