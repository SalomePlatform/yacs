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

#ifndef __SERVERNODE_HXX__
#define __SERVERNODE_HXX__

#include "YACSlibEngineExport.hxx"
#include "InlineNode.hxx"

#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Container;
    class Visitor;

    class YACSLIBENGINE_EXPORT ServerNode : public InlineFuncNode
    {
    public:
      ServerNode(const std::string& name);
      ServerNode(const ServerNode& other, ComposedNode *father);
      void load();
      void accept(Visitor *visitor);
      virtual ServerNode *createNode(const std::string& name) const = 0;
      bool isDeployable() const;
      virtual std::string getEffectiveKindOfServer() const = 0;
      virtual ~ServerNode();
      virtual std::string typeName() { return "YACS__ENGINE__ServerNode"; }
    };
  }
}

#endif
