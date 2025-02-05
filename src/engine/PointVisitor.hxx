// Copyright (C) 2015-2025  CEA, EDF
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

#pragma once

#include "YACSlibEngineExport.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class NotSimpleCasePoint;
    class ElementaryPoint;
    class LinkedBlocPoint;
    class ForkBlocPoint;
    
    class YACSLIBENGINE_EXPORT PointVisitor
    {
    public:
      virtual void beginForkBlocPoint(ForkBlocPoint *pt) = 0;
      virtual void endForkBlocPoint(ForkBlocPoint *pt) = 0;
      virtual void beginLinkedBlocPoint(LinkedBlocPoint *pt) = 0;
      virtual void endLinkedBlocPoint(LinkedBlocPoint *pt) = 0;
      virtual void beginElementaryPoint(ElementaryPoint *pt) = 0;
      virtual void endElementaryPoint(ElementaryPoint *pt) = 0;
      virtual void beginNotSimpleCasePoint(NotSimpleCasePoint *pt) = 0;
      virtual void endNotSimpleCasePoint(NotSimpleCasePoint *pt) = 0;
    };
  }
}
