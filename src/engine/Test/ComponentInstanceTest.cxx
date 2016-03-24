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

#include "ComponentInstanceTest.hxx"
#include "ToyNode.hxx"

using namespace std;
using namespace YACS::ENGINE;

ComponentInstanceTest1::ComponentInstanceTest1(const ComponentInstanceTest1& other):ComponentInstance(other),_loaded(false)
{
}

ComponentInstanceTest1::ComponentInstanceTest1(const std::string& name):ComponentInstance(name),_loaded(false)
{
}

void ComponentInstanceTest1::load(Task *askingNode)
{
  _loaded=true;
}

void ComponentInstanceTest1::unload(Task *askingNode)
{
  _loaded=false;
}

bool ComponentInstanceTest1::isLoaded(Task *askingNode) const
{
  return _loaded;
}

std::string ComponentInstanceTest1::getKind() const
{
  return ToyNode1S::KIND;
}

std::string ComponentInstanceTest1::getKindForNode() const
{
  return ToyNode1S::KIND;
}

ServiceNode* ComponentInstanceTest1::createNode(const std::string& name)
{
  ToyNode1S* node=new ToyNode1S(name);
  node->setComponent(this);
  return node;
}

ComponentInstance *ComponentInstanceTest1::clone() const
{
  if(isAttachedOnCloning())
    {
      incrRef();
      return (ComponentInstance *) this;
    }
  else
    return new ComponentInstanceTest1(*this);
}

ComponentInstance *ComponentInstanceTest1::cloneAlways() const
{
  return new ComponentInstanceTest1(*this);
}

ComponentInstanceTest2::ComponentInstanceTest2(const ComponentInstanceTest2& other):ComponentInstance(other),_loaded(false)
{
}

ComponentInstanceTest2::ComponentInstanceTest2(const std::string& name):ComponentInstance(name),_loaded(false)
{
}

void ComponentInstanceTest2::load(Task *askingNode)
{
  _loaded=true;
}

void ComponentInstanceTest2::unload(Task *askingNode)
{
  _loaded=false;
}

bool ComponentInstanceTest2::isLoaded(Task *askingNode) const
{
  return _loaded;
}

std::string ComponentInstanceTest2::getKind() const
{
  return ToyNode2S::KIND;
}

std::string ComponentInstanceTest2::getKindForNode() const
{
  return ToyNode2S::KIND;
}

ServiceNode* ComponentInstanceTest2::createNode(const std::string& name)
{
  ToyNode2S* node=new ToyNode2S(name);
  node->setComponent(this);
  return node;
}

ComponentInstance *ComponentInstanceTest2::clone() const
{
  if(isAttachedOnCloning())
    {
      incrRef();
      return (ComponentInstance *) this;
    }
  else
    return new ComponentInstanceTest2(*this);
}

ComponentInstance *ComponentInstanceTest2::cloneAlways() const
{
  return new ComponentInstanceTest2(*this);
}
