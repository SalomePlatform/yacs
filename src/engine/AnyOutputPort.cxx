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

#include "AnyOutputPort.hxx"
#include "AutoLocker.hxx"
#include "Runtime.hxx"
#include "Any.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;

AnyOutputPort::AnyOutputPort(const std::string& name, Node *node, TypeCode *type):OutputPort(name,node,type),
                                                                                  DataPort(name,node,type),Port(node),_data(0)
{
}

AnyOutputPort::AnyOutputPort(const AnyOutputPort& other, Node *newHelder):OutputPort(other,newHelder),
                                                                          DataPort(other,newHelder),
                                                                          Port(other,newHelder),_data(0)
{
}

AnyOutputPort::~AnyOutputPort()
{
  if(_data)
    _data->decrRef();
}

//! store the current dispatched value
void AnyOutputPort::setValue(Any *data) 
{
  YACS::BASES::AutoLocker<YACS::BASES::Mutex> lock(&_mutex);
  if(_data)
    _data->decrRef();
  _data = data; 
  if(_data)
    _data->incrRef();
}

OutputPort *AnyOutputPort::clone(Node *newHelder) const
{
  return new AnyOutputPort(*this,newHelder);
}

void AnyOutputPort::put(const void *data) throw(ConversionException)
{
  put((YACS::ENGINE::Any *)data);
}

void AnyOutputPort::put(YACS::ENGINE::Any *data) throw(ConversionException)
{
  setValue(data);
  OutputPort::put(data);
}

std::string AnyOutputPort::getAsString()
{
  YACS::BASES::AutoLocker<YACS::BASES::Mutex> lock(&_mutex);
  return getRuntime()->convertNeutralAsString(edGetType(),_data);
}
