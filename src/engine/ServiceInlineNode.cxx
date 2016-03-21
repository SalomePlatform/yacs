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

#include "ServiceInlineNode.hxx"
#include "Visitor.hxx"

using namespace YACS::ENGINE;

ServiceInlineNode::ServiceInlineNode(const std::string& name):ServiceNode(name)
{
}

ServiceInlineNode::ServiceInlineNode(const ServiceInlineNode& other, ComposedNode *father):ServiceNode(other,father),_script(other._script)
{
}

void ServiceInlineNode::setScript (const std::string &script)
{
  _script=script;
}

std::string  ServiceInlineNode::getScript() const
{
  return _script;
}

void ServiceInlineNode::accept(Visitor *visitor)
{
  visitor->visitServiceInlineNode(this);
}
