// Copyright (C) 2006-2019  CEA/DEN, EDF R&D
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
#include <algorithm>

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

void InPort::edRemoveAllLinksLinkedWithMe() 
{
  set< std::pair<OutPort *,bool> > temp(_backLinks);//edRemoveLink called after causes invalidation of set iterator.
  for(auto iter : temp)
    {
      set<OutPort *> trueBackOutputs;
      iter.first->getAllRepresented(trueBackOutputs);
      for( auto iter2 : trueBackOutputs )
        _node->getRootNode()->edRemoveLink(iter2,this);
    }
  _backLinks.clear();
  modified();
}

//! Returns \b physical backlinks \b NOT user backlinks.
std::set<OutPort *> InPort::edSetOutPort() const
{
  std::set<OutPort *> ret;
  for( auto iter : _backLinks )
    ret.insert(iter.first);
  return ret;
}

bool InPort::canSafelySqueezeMemory() const
{
  if(!isBackLinked())
    return false;
  for(auto bl : _backLinks)
    {
      if(!bl.second)
        return false;
    }
  return true;
}

/*!
 * \param [in] isLoopProof - Tells if the data coming from \a fromPort will be send again in case of \a this is initialized. This value is
 *                           important if \a this is an InPort of a Node contained directly or not inside a Loop, ForEachLoop, OptimizerLoop.
 *                           In this case, to optimize memory consumption (see squeezeMemory method), we need to know if data coming from \a fromPort
 *                           will be generated again in case.
 *                           If true (the default) it means that for that link is a link loop proof so no need to take care. If false, the link is not loop proof so
 *                           event in the context of agressive memory management the data can't be safely released.
 */
void InPort::edNotifyReferencedBy(OutPort *fromPort, bool isLoopProof)
{
  auto it(std::find_if(_backLinks.begin(),_backLinks.end(),[fromPort](const std::pair<OutPort *,bool>& p){ return p.first==fromPort; }));
  if(it!=_backLinks.end())
    _backLinks.erase(it);
  _backLinks.insert(std::pair<OutPort *,bool>(fromPort,isLoopProof));
  modified();
}

void InPort::edNotifyDereferencedBy(OutPort *fromPort)
{
  auto it(std::find_if(_backLinks.begin(),_backLinks.end(),[fromPort](const std::pair<OutPort *,bool>& p){ return p.first==fromPort; }));
  if(it!=_backLinks.end())
    _backLinks.erase(it);
  modified();
}

void InPort::getAllRepresentants(std::set<InPort *>& repr) const
{
  repr.insert((InPort *)this);
}
