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

#include "Visitor.hxx"
#include "Node.hxx"
#include "Proc.hxx"
#include "TypeCode.hxx"

using namespace YACS::ENGINE;
using namespace std;

Visitor::Visitor(ComposedNode *root): _root(root)
{
}

std::map<std::string, std::string> Visitor::getNodeProperties(Node *node)
{
  return node->_propertyMap;
}

std::map<std::string, TypeCode*> Visitor::getTypeCodeMap(Proc *proc)
{
  return proc->typeMap;
}

std::map<std::string, Container*> Visitor::getContainerMap(Proc *proc)
{
  return proc->containerMap;
}

std::list<TypeCodeObjref *> Visitor::getListOfBases(TypeCodeObjref *objref)
{
  return objref->_listOfBases;
}
