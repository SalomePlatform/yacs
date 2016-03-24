// Copyright (C) 2015-2016  CEA/DEN, EDF R&D
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

#ifndef __BLOCPOINT_HXX__
#define __BLOCPOINT_HXX__

#include "YACSlibEngineExport.hxx"
#include "AbstractPoint.hxx"

#include <list>

namespace YACS
{
  namespace ENGINE
  {
    class YACSLIBENGINE_EXPORT BlocPoint : public AbstractPoint
    {
    protected:
      std::list<AbstractPoint *> _nodes;
    public:
      BlocPoint(const std::list<AbstractPoint *>& nodes, AbstractPoint *father);
      AbstractPoint *findPointWithNode(Node *node);
      AbstractPoint *getNodeAfter(Node *node);
      AbstractPoint *getNodeB4(Node *node);
      bool contains(Node *node);
      int getNumberOfNodes() const;
      const std::list<AbstractPoint *>& getListOfPoints() const { return _nodes; }
      virtual ~BlocPoint();
    };
  }
}


#endif
