// Copyright (C) 2006-2024  CEA, EDF
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

#ifndef __DATANODE_HXX__
#define __DATANODE_HXX__

#include "YACSlibEngineExport.hxx"
#include "ElementaryNode.hxx"

#include <string>

namespace YACS
{
  namespace ENGINE
  {
/*! \brief Class for data parameters specification
 *
 * \ingroup Nodes
 *
 */
    class YACSLIBENGINE_EXPORT DataNode : public ElementaryNode 
    {
    protected:
      DataNode(const DataNode& other, ComposedNode *father)
        :ElementaryNode(other,father){ }
      DataNode(const std::string& name):ElementaryNode(name) { }
    public:
      virtual void setData(InputPort* port ,const std::string& data);
      virtual void setData(OutputPort* port ,const std::string& data);
      virtual void setRef(const std::string& ref);
      virtual std::string getRef();
      virtual ~DataNode();
      virtual std::string typeName() {return "YACS__ENGINE__DataNode";}
    protected:
      std::string _ref;
    };
  }
}

#endif
