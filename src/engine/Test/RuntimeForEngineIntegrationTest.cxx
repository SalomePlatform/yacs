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

#include "RuntimeForEngineIntegrationTest.hxx"
#include "ComponentInstanceTest.hxx"
#include "Exception.hxx"
#include "ToyNode.hxx"
#include "TypeCode.hxx"
#include <sstream>
#include <string.h>


using namespace YACS::ENGINE;

void RuntimeForEngineIntegrationTest::setRuntime()
{
  if (! Runtime::_singleton)
    Runtime::_singleton = new RuntimeForEngineIntegrationTest;
}

ElementaryNode* RuntimeForEngineIntegrationTest::createNode(const std::string& implementation, const std::string& name) throw (YACS::Exception)
{
  if (implementation == ToyNode::MY_IMPL_NAME)
    return new ToyNode(name);
  else if(implementation == LimitNode::MY_IMPL_NAME)
    return new LimitNode(name);
  std::string what="RuntimeForEngineIntegrationTest does not handle this implementation: " + implementation;
  throw YACS::Exception(what);
}

InputPort* RuntimeForEngineIntegrationTest::createInputPort(const std::string& name, const std::string& impl, Node * node, TypeCode * type)
{
  if(impl == ToyNode::MY_IMPL_NAME)
    {
      if(type->kind()!=Double)
        throw Exception("Invalid type");
      return new InputToyPort(name, node);
    }
  else if(impl == LimitNode::MY_IMPL_NAME)
    throw Exception("InputPort creation not allowed for LimitNode");
  std::ostringstream msg;
  msg << "Cannot create " << impl << " OutputPort" ;
  throw YACS::Exception(msg.str());
}

OutputPort* RuntimeForEngineIntegrationTest::createOutputPort(const std::string& name, const std::string& impl, Node * node, TypeCode * type)
{
  if(impl == ToyNode::MY_IMPL_NAME)
    {
      if(type->kind()!=Double && type->kind()!=Int)
        throw Exception("Invalid type");
      return new OutputToyPort(name, node, type);
    }
  else if(impl == LimitNode::MY_IMPL_NAME)
    throw Exception("OutputPort creation not allowed for LimitNode");
  std::stringstream msg;
  msg << "Cannot create " << impl << " OutputPort" ;
  throw Exception(msg.str());
}

InputPort* RuntimeForEngineIntegrationTest::adapt(InputPort* source, const std::string& impl,TypeCode * type,bool init) throw (ConversionException)
{
  return new ProxyPort(source);
}

InputPort* RuntimeForEngineIntegrationTest::adapt(InPropertyPort* source, const std::string& impl, TypeCode * type, bool init) throw (ConversionException)
{
  return adapt((InputPort *)source, impl, type, init);
}

ComponentInstance* RuntimeForEngineIntegrationTest::createComponentInstance(const std::string& name, const std::string& kind)
{
  if(kind==ToyNode1S::KIND)
    return new ComponentInstanceTest1(name);
  else if(kind==ToyNode2S::KIND)
    return new ComponentInstanceTest2(name);
  else
    {
      std::string msg("RuntimeForEngineIntegrationTest::createComponentInstance : Unable to crate component with kind \"");
      msg+=kind; msg+="\"";
      throw Exception(msg);
    }
}
