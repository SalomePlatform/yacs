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

#include "ServerNode.hxx"
#include "Container.hxx"
#include "Visitor.hxx"

using namespace YACS::ENGINE;

ServerNode::ServerNode(const std::string& name):InlineFuncNode(name)
{
}

ServerNode::ServerNode(const ServerNode& other, ComposedNode *father):InlineFuncNode(other,father)
{
}

void ServerNode::load()
{
  if(_container)
    {
      if(!_container->isAlreadyStarted(this))
        {
          try
            {
              _container->start(this);
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

//! By definition of ServerNode class.
bool ServerNode::isDeployable() const
{
  return true;
}

ServerNode::~ServerNode()
{
}

