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

#include "InPort.hxx"
#include "OutPort.hxx"
#include "ComposedNode.hxx"
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

InPort::InPort(const InPort& other, Node *newHelder):
  DataPort(other,newHelder),Port(other,newHelder)
{
}

InPort::InPort(const std::string& name, Node *node, TypeCode* type):
  DataPort(name,node,type),Port(node)
{
}

InPort::~InPort()
{
}

//! Returns number of \b physical backlinks \b NOT number of user backlinks.
int InPort::edGetNumberOfLinks() const
{
  return _backLinks.size();
}

void InPort::edRemoveAllLinksLinkedWithMe() throw(YACS::Exception)
{
  set<OutPort *> temp(_backLinks);//edRemoveLink called after causes invalidation of set iterator.
  for(set<OutPort *>::iterator iter=temp.begin();iter!=temp.end();iter++)
    {
      set<OutPort *> trueBackOutputs;
      (*iter)->getAllRepresented(trueBackOutputs);
      for(set<OutPort *>::iterator iter2=trueBackOutputs.begin();iter2!=trueBackOutputs.end();iter2++)
        _node->getRootNode()->edRemoveLink(*iter2,this);
    }
  _backLinks.clear();
  modified();
}

//! Returns \b physical backlinks \b NOT user backlinks.
std::set<OutPort *> InPort::edSetOutPort() const
{
  return _backLinks;
}

void InPort::edNotifyReferencedBy(OutPort *fromPort)
{
  _backLinks.insert(fromPort);
  modified();
}

void InPort::edNotifyDereferencedBy(OutPort *fromPort)
{
  _backLinks.erase(fromPort);
  modified();
}

void InPort::getAllRepresentants(std::set<InPort *>& repr) const
{
  repr.insert((InPort *)this);
}
