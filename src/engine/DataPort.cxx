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

#include "DataPort.hxx"
#include "TypeCode.hxx"
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

const char DataPort::NAME[]="DataPort";

DataPort::~DataPort()
{
  if(_type)
    _type->decrRef();
}

DataPort::DataPort(const std::string& name, Node *node, TypeCode* type):Port(node),_name(name),_type(type)
{
  if(_type)
	_type->incrRef();
}

DataPort::DataPort(const DataPort& other, Node *newHelder):Port(other,newHelder),_name(other._name),_type(other._type)
{
  if(_type)
	_type->incrRef();
}

void DataPort::edSetType(TypeCode* type)
{
  if(_type)
    _type->decrRef();
  _type=type;
  if(_type)
    _type->incrRef();
}

string DataPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

bool DataPort::isDifferentTypeOf(const DataPort *other) const
{
  return getTypeOfChannel()!=other->getTypeOfChannel();
}

/*!
 * If in historyOfLink different type of Port are detected : The first one (by starting from the end of 'historyOfLink')
 * is returned. Else 0 is returned if they are all of the same type.
 */
DataPort *DataPort::isCrossingType(const std::vector<DataPort *>& historyOfLink)
{
  vector<DataPort *>::const_reverse_iterator iter=historyOfLink.rbegin()+1;
  const DataPort *base=historyOfLink.back();
  for(;iter!=historyOfLink.rend();iter++)
    if(base->isDifferentTypeOf(*iter))
      return *iter;
  return 0;
}

//! returns port value as a string that can be used in a GUI for example
/*!
 * Do nothing here. To subclass
 */
std::string DataPort::getAsString()
{
  return "";
}
