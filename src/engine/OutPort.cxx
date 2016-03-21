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

#include "OutPort.hxx"
#include "InPort.hxx"
#include "ComposedNode.hxx"
#include <algorithm>
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

OutPort::OutPort(const OutPort& other, Node *newHelder):DataPort(other,newHelder),Port(other,newHelder)
{
}

OutPort::OutPort(const std::string& name, Node *node, TypeCode* type):DataPort(name,node,type),Port(node)
{
}

OutPort::~OutPort()
{
}

void OutPort::checkConsistency(LinkInfo& info) const
{
}

void OutPort::getAllRepresented(std::set<OutPort *>& represented) const
{
  represented.insert((OutPort *)this);
}

int OutPort::edGetNumberOfOutLinks() const
{
  return edSetInPort().size();
}

std::vector<DataPort *> OutPort::calculateHistoryOfLinkWith(InPort *end)
{
  if(!isAlreadyLinkedWith(end))
    throw Exception("ComposedNode::edRemoveLink : unexisting link");
  vector<DataPort *> ret;
  ComposedNode* lwstCmnAnctr=ComposedNode::getLowestCommonAncestor(getNode(),end->getNode());
  list<ComposedNode *> allAscendanceOfNodeStart=getNode()->getAllAscendanceOf(lwstCmnAnctr);
  list<ComposedNode *> allAscendanceOfNodeEnd=end->getNode()->getAllAscendanceOf(lwstCmnAnctr);

  // --- Part of test if the link from 'start' to 'end' really exist particulary all eventually intermediate ports created

  ComposedNode *iterS=getNode()->getFather();
  pair<OutPort *,OutPort *> currentPortO(this,this);
  ret.push_back(currentPortO.first);
  while(iterS!=lwstCmnAnctr)
    {
      iterS->getDelegateOf(currentPortO, end, allAscendanceOfNodeEnd);
      if(currentPortO.first!=ret.back())
        ret.push_back(currentPortO.first);
      iterS=iterS->_father;
    }
  iterS=end->getNode()->getFather();
  InPort *currentPortI=end;
  int i=0;
  while(iterS!=lwstCmnAnctr)
    {
      vector<DataPort *>::iterator iter2;
      iterS->getDelegateOf(currentPortI, this, allAscendanceOfNodeStart);
      if(currentPortI!=ret.back())
        {
          i++;
          ret.push_back(currentPortI);
        }
      iterS=iterS->_father;
    }
  vector<DataPort *>::iterator iter=ret.end(); iter-=i;
  reverse(iter,ret.end());
  return ret;
}
