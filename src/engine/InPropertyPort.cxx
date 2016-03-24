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

#include "InPropertyPort.hxx"
#include "Node.hxx"
#include "TypeCode.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char InPropertyPort::NAME[]="InPropertyPort";

InPropertyPort::InPropertyPort(const InPropertyPort& other, Node *newHelder)
  : InputPort(other, newHelder),
    DataPort(other, newHelder),
    Port(other, newHelder),
    _property_data(NULL), _init_property_data(NULL)
{}

InPropertyPort::InPropertyPort(const std::string& name, Node *node, TypeCode* type, bool canBeNull)
  : InputPort(name, node, type, canBeNull),
    DataPort(name, node, type),
    Port(node),
    _property_data(NULL), _init_property_data(NULL)
{
}

InPropertyPort::~InPropertyPort() {}

string InPropertyPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

void
InPropertyPort::exNewPropertyValue(const std::string & name, const std::string & value)
{
  DEBTRACE("Adding new Property to the node " << name << " " << value);
  _node->setProperty(name, value);
}

void
InPropertyPort::exSaveInit()
{
  _init_property_data = _property_data;
}

void
InPropertyPort::exRestoreInit()
{
  if(!_init_property_data)return;
  _property_data = _init_property_data;
}

InPropertyPort *
InPropertyPort::clone(Node *newHelder) const
{
  return new InPropertyPort(*this,newHelder);
}

void *
InPropertyPort::get() const
{
  return (void*) _property_data;
}

void
InPropertyPort::put(const void *data) throw(ConversionException)
{
  put((YACS::ENGINE::Any *)data);
}

void
InPropertyPort::put(YACS::ENGINE::Any *data)
{
  // Add new properties to the node
  YACS::ENGINE::SequenceAny * seq_data = static_cast<YACS::ENGINE::SequenceAny*>(data);
  for (int i = 0; i < seq_data->size(); i++)
  {
    std::string key = ((*seq_data)[i]["name"])->getStringValue();
    std::string value = ((*seq_data)[i]["value"])->getStringValue();
    exNewPropertyValue(key, value);
  }

  if(_property_data)
    _property_data->decrRef();
  _property_data = data;
  _property_data->incrRef();
  DEBTRACE("value ref count: " << _property_data->getRefCnt());
}
