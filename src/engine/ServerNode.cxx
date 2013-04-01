// Copyright (C) 2006-2013  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include "ServerNode.hxx"
#include "Container.hxx"
#include "Visitor.hxx"

using namespace YACS::ENGINE;

ServerNode::ServerNode(const std::string& name):InlineFuncNode(name),_container(0)
{
}

ServerNode::ServerNode(const ServerNode& other, ComposedNode *father):InlineFuncNode(other,father),_container(0)
{
}

void ServerNode::performDuplicationOfPlacement(const Node& other)
{
  const ServerNode &otherC=*(dynamic_cast<const ServerNode *>(&other));
  //if other has no container don't clone: this will not have one
  if(otherC._container)
    _container=otherC._container->clone();
}

void ServerNode::load()
{
  if(_container)
    {
      if(!_container->isAlreadyStarted(0))
        {
          try
            {
              _container->start(0);
            }
          catch(Exception& e)
            {
              _errorDetails=e.what();
              throw e;
            }
        }
    }
  else
    {
      std::string what("ServerNode::load : a load operation requested on ServerNode called \"");
      what+=_name; what+="\" with no container specified.";
      _errorDetails=what;
      throw Exception(what);
    }
}

void ServerNode::accept(Visitor *visitor)
{
  visitor->visitServerNode(this);
}

void ServerNode::setContainer(Container *container)
{
  if(_container==container)
    return ;
  if(_container)
    _container->decrRef();
  _container=container;
  if(_container)
    _container->incrRef();
  modified();
}

//! By definition of ServerNode class.
bool ServerNode::isDeployable() const
{
  return true;
}

ServerNode::~ServerNode()
{
  if(_container)
    _container->decrRef();
}
