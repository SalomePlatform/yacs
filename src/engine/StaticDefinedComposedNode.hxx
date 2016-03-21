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

#ifndef __STATICDEFINEDCOMPOSEDNODE_HXX__
#define __STATICDEFINEDCOMPOSEDNODE_HXX__

#include "YACSlibEngineExport.hxx"
#include "ComposedNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    /*!
     * Abstract class, that factorizes all the treatments relative to resource management for ComposedNode that have
     * their connectivity fully defined before launching ; which is not always the case for classes inheriting from DynParaLoop.
     */
    class YACSLIBENGINE_EXPORT StaticDefinedComposedNode : public ComposedNode
    {
    protected:
      StaticDefinedComposedNode(const std::string& name);
      StaticDefinedComposedNode(const StaticDefinedComposedNode& other, ComposedNode *father);
    public:
      bool isPlacementPredictableB4Run() const;
      bool isMultiplicitySpecified(unsigned& value) const;
      void forceMultiplicity(unsigned value);
      void selectRunnableTasks(std::vector<Task *>& tasks);
    protected:
      void checkControlDependancy(OutPort *start, InPort *end, bool cross,
                                  std::map < ComposedNode *,  std::list < OutPort * >, SortHierarc >& fw,
                                  std::vector<OutPort *>& fwCross,
                                  std::map< ComposedNode *, std::list < OutPort *>, SortHierarc >& bw,
                                  LinkInfo& info) const;
    };
  }
}

#endif
