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

#include "Port.hxx"
#include "Node.hxx"
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

const char Port::NAME[]="Port";

int Port::_total = 0;

Port::Port(Node *node):_node(node)
{
  _id = _total++;
}

Port::Port(const Port& other, Node *newHelder):_node(newHelder)
{
  _id = _total++;
}

Port::~Port()
{
}

string Port::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

void Port::modified()
{
  DEBTRACE("Port::modified()");
  if(_node)
    _node->modified();
}
