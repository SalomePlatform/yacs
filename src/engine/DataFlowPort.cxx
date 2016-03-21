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

#include "DataFlowPort.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char DataFlowPort::NAME[]="DataFlowPort";

DataFlowPort::DataFlowPort(const DataFlowPort& other, Node *newHelder):DataPort(other,newHelder),Port(other,newHelder)
{
}

DataFlowPort::DataFlowPort(const std::string& name, Node *node, TypeCode* type):DataPort(name,node,type),Port(node)
{
}

DataFlowPort::~DataFlowPort()
{
}

string DataFlowPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

//! Gives a string representation of the data, for user interfaces.
/*! Implemented in derived classes, depending on runtime, to provide
 *  the data representation for user interfaces. 
 *  Typical use, a Python string representation that can be used in
 *  edition or restitution.
 */
std::string DataFlowPort::valToStr()
{
  return "no display conversion available";
}

//! Allows to set data from a string representation used in user interface.
/*! Implemented in derived classes, depending on runtime, to set the data
 *  from its user interface representation. 
 *  Typical use, a Python string representation that can be used in
 *  edition or restitution.
 */
void DataFlowPort::valFromStr(std::string valstr)
{
}
