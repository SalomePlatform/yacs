// Copyright (C) 2015  EDF R&D
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

#ifndef __BAGPOINT_HXX__
#define __BAGPOINT_HXX__

#include "YACSlibEngineExport.hxx"
#include "BlocPoint.hxx"

#include <list>

namespace YACS
{
  namespace ENGINE
  {
    class YACSLIBENGINE_EXPORT BagPoint : public BlocPoint
    {
    public:
      BagPoint(const std::list<AbstractPoint *>& nodes, AbstractPoint *father);
    public://overloading
      Node *getFirstNode();
      Node *getLastNode();
      int getMaxLevelOfParallelism() const;
      std::string getRepr() const;
    public:
      int size() const { return (int)_nodes.size(); }
      void replaceInMe(BlocPoint *aSet);
      void deal1(bool& somethingDone);
      void deal2(bool& somethingDone);
      void deal2Bis(bool& somethingDone);
      void deal2Ter(bool& somethingDone);
    };
  }
}


#endif