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

#include "RuntimeForEngineTest.hxx"
#include <iostream>
#include <sstream>

using namespace YACS::ENGINE;
using namespace std;

TestElemNode::TestElemNode(const TestElemNode& other, ComposedNode *father):ElementaryNode(other,father)
{
}

Node *TestElemNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new TestElemNode(*this,father);
}

TestElemInputPort::TestElemInputPort(const std::string& name, Node *node, TypeCode* type):InputPort(name,node,type),DataPort(name,node,type),Port(node)
{
}

TestElemInputPort::TestElemInputPort(const TestElemInputPort& other, Node *newHelder):InputPort(other,newHelder),DataPort(other,newHelder),Port(other,newHelder)
{
}

void TestElemInputPort::put(const void *data) throw(ConversionException)
{
  cerr << _name << endl;
  stringstream msg;
  msg << "Not implemented (" << __FILE__ << ":" << __LINE__ << ")";
  throw Exception(msg.str());
}

InputPort *TestElemInputPort::clone(Node *newHelder) const
{
  return new TestElemInputPort(*this,newHelder);
}

void *TestElemInputPort::get() const throw(YACS::Exception)
{
  stringstream msg;
  msg << "Not implemented (" << __FILE__ << ":" << __LINE__ << ")";
  throw Exception(msg.str());
}

void TestElemInputPort::exRestoreInit()
{
  if(!_initValue)
    return;
  if(_value)
    _value->decrRef();
  _value=_initValue;
  _value->incrRef();
}

void TestElemInputPort::exSaveInit()
{
  if(_initValue)
    _initValue->decrRef();
  _initValue=_value;
  _initValue->incrRef();
}

TestElemOutputPort::TestElemOutputPort(const std::string& name, Node *node, TypeCode* type):OutputPort(name,node,type),DataPort(name,node,type),Port(node)
{
}

TestElemOutputPort::TestElemOutputPort(const TestElemOutputPort& other, Node *newHelder):OutputPort(other,newHelder),DataPort(other,newHelder),Port(other,newHelder)
{
}

OutputPort *TestElemOutputPort::clone(Node *newHelder) const
{
  return new TestElemOutputPort(*this,newHelder);
}

void TestElemOutputPort::put(const void *data) throw(ConversionException)
{
  cerr << _name << endl;
  stringstream msg;
  msg << "Not implemented (" << __FILE__ << ":" << __LINE__ << ")";
  throw Exception(msg.str());
}

void RuntimeForEngineTest::setRuntime()
{
  if (! Runtime::_singleton)
    Runtime::_singleton = new RuntimeForEngineTest;
}

ElementaryNode* RuntimeForEngineTest::createNode(const string& implementation, const string& name) throw(YACS::Exception)
{
  return new TestElemNode(name);
}

InputPort* RuntimeForEngineTest::createInputPort(const string& name, const string& impl, Node * node, TypeCode * type)
{
  return new TestElemInputPort(name, node, type);
}

OutputPort* RuntimeForEngineTest::createOutputPort(const string& name, const string& impl, Node * node, TypeCode * type)
{
  return new TestElemOutputPort(name, node, type);
}

InputPort* RuntimeForEngineTest::adapt(InputPort* source, const string& impl,TypeCode * type,bool init) throw (ConversionException)
{
  return new ProxyPort(source);
}

InputPort* RuntimeForEngineTest::adapt(InPropertyPort* source, const std::string& impl, TypeCode * type, bool init) throw (ConversionException)
{
  return adapt((InputPort *)source,impl,type,init);
}
