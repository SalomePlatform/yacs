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
#ifndef _SCENEPORTITEM_HXX_
#define _SCENEPORTITEM_HXX_

#include "SceneObserverItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SceneTextItem;

    class ScenePortItem
    {
    public:
      ScenePortItem(QString label);
      virtual ~ScenePortItem();
      virtual void setText(QString label)=0;
      static int getPortWidth();
      static int getPortHeight();
    protected:
      static const int _portWidth;
      static const int _portHeight;
      SceneTextItem* _text;
    };
  }
}

#endif