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

#ifndef __ABSTRACTPOINT_HXX__
#define __ABSTRACTPOINT_HXX__

#include "YACSlibEngineExport.hxx"

#include <list>
#include <vector>
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Node;
    class InGate;
    class OutGate;
    class BlocPoint;
    class ForkBlocPoint;
    class LinkedBlocPoint;
    
    class YACSLIBENGINE_EXPORT AbstractPoint
    {
    public:
      AbstractPoint(AbstractPoint *father):_father(father) { }
      AbstractPoint *getFather() const { return _father; }
      AbstractPoint *getGodFather();
      bool amIGod() { return getGodFather()==0; }
      void setFather(AbstractPoint *father) { _father=father; }
      //
      bool isBegin();
      bool isLast();
      bool isSimplyLinkedBeforeAfter(BlocPoint *sop);
      bool isSimplyLinkedAfterNullBefore(BlocPoint *sop);
      bool isSimplyLinkedBeforeNullAfter(BlocPoint *sop);
      //
      LinkedBlocPoint *tryAsLink(BlocPoint *sop);
      ForkBlocPoint *tryAsFork(BlocPoint *sop);
      ForkBlocPoint *tryAsForkBis(BlocPoint *sop);
      ForkBlocPoint *tryAsForkTer(BlocPoint *sop);
      //
      virtual Node *getFirstNode() = 0;
      virtual Node *getLastNode() = 0;
      virtual AbstractPoint *findPointWithNode(Node *node) = 0;
      virtual bool contains(Node *node) = 0;
      virtual int getNumberOfNodes() const = 0;
      virtual int getMaxLevelOfParallelism() const = 0;
      virtual std::string getRepr() const = 0;
      virtual ~AbstractPoint();
    public:
      static bool IsGatherB4Ext(Node *node);
      bool isSimplyLinkedAfter(BlocPoint *sop, Node *node);
      static bool IsSimplyLinkedAfterExt(Node *node);
      static bool IsScatterAfterExt(Node *node);
      bool isSimplyLinkedBefore(BlocPoint *sop, Node *node);
      static bool IsSimplyLinkedBeforeExt(Node *node);
      static bool IsNoLinksBefore(Node *node);
      static bool IsNoLinksAfter(Node *node);
      static Node *GetNodeB4(Node *node);
      static Node *GetNodeAfter(Node *node);
      static AbstractPoint *GetDirectSonOf(AbstractPoint *refFather, AbstractPoint *sonOrLittleSon);
      static bool IsCommonDirectSonOf(AbstractPoint *refFather, const std::list<OutGate *>& outgs, AbstractPoint *&ret);
      static bool IsCommonDirectSonOf(AbstractPoint *refFather, const std::list<InGate *>& ings, AbstractPoint *&ret);
    protected:
      AbstractPoint *_father;
    };
  }
}


#endif
