//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "DataStreamPort.hxx"
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

const char DataStreamPort::NAME[]="DataStreamPort";

DataStreamPort::DataStreamPort(const DataStreamPort& other, Node *newHelder):DataPort(other,newHelder),Port(other,newHelder),_propertyMap(other._propertyMap)
{
}

DataStreamPort::DataStreamPort(const std::string& name, Node *node, TypeCode* type):DataPort(name,node,type),Port(node)
{
}

DataStreamPort::~DataStreamPort()
{
}

string DataStreamPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

void DataStreamPort::setProperty(const std::string& name, const std::string& value)
{
  _propertyMap[name]=value;
}

std::string DataStreamPort::getProperty(const std::string& name)
{
  return _propertyMap[name];
}
void DataStreamPort::initPortProperties()
{
}

void DataStreamPort::setProperties(std::map<std::string,std::string> properties)
{
  _propertyMap.clear();
  std::map<std::string,std::string>::iterator it;
  for (it = properties.begin(); it != properties.end(); ++it)
    {
      setProperty((*it).first, (*it).second); // setProperty virtual and derived
    }
}
