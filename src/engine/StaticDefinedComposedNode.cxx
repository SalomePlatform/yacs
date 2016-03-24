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

#include "StaticDefinedComposedNode.hxx"
#include "OutPort.hxx"
#include "InPort.hxx"

using namespace std;
using namespace YACS::ENGINE;

StaticDefinedComposedNode::StaticDefinedComposedNode(const std::string& name):ComposedNode(name)
{
}

StaticDefinedComposedNode::StaticDefinedComposedNode(const StaticDefinedComposedNode& other, ComposedNode *father):ComposedNode(other,father)
{
}

bool StaticDefinedComposedNode::isPlacementPredictableB4Run() const
{
  return true;
}

bool StaticDefinedComposedNode::isMultiplicitySpecified(unsigned& value) const
{
  value=1;
  return true;
}

void StaticDefinedComposedNode::forceMultiplicity(unsigned value)
{
  //no sense for this class
}

void StaticDefinedComposedNode::selectRunnableTasks(std::vector<Task *>& tasks)
{
}

void StaticDefinedComposedNode::checkControlDependancy(OutPort *start, InPort *end, bool cross,
                                                       std::map < ComposedNode *,  std::list < OutPort * >, SortHierarc >& fw,
                                                       std::vector<OutPort *>& fwCross,
                                                       std::map< ComposedNode *, std::list < OutPort *>, SortHierarc >& bw,
                                                       LinkInfo& info) const
{
  if(start->getNode()==end->getNode())
    bw[(ComposedNode *)this].push_back(start);
  else
    throw Exception("Internal error occured - dealing an unexpected link !");
}
