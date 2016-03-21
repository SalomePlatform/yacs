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

#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

const char InputDataStreamPort::NAME[]="InputDataStreamPort";

InputDataStreamPort::InputDataStreamPort(const InputDataStreamPort& other, Node *newHelder):
    DataStreamPort(other,newHelder),
    InPort(other,newHelder),
    DataPort(other,newHelder),
    Port(other,newHelder)
{
}

InputDataStreamPort::InputDataStreamPort(const std::string& name, Node *node, TypeCode* type):
    DataStreamPort(name,node,type),
    InPort(name,node,type),
    DataPort(name,node,type),
    Port(node)
{
}

InputDataStreamPort::~InputDataStreamPort()
{
}

string InputDataStreamPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

InputDataStreamPort *InputDataStreamPort::clone(Node *newHelder) const
{
  return new InputDataStreamPort(*this,newHelder);
}

void InputDataStreamPort::edAddOutputDataStreamPort(OutputDataStreamPort *port)
{
  DEBTRACE("InputDataStreamPort::edAddOutputDataStreamPort");
  if(!isAlreadyInSet(port))
    _setOfOutputDataStreamPort.insert(port);
}

bool InputDataStreamPort::isAlreadyInSet(OutputDataStreamPort *outPort) const
{
  return _setOfOutputDataStreamPort.find(outPort)!=_setOfOutputDataStreamPort.end();
}
