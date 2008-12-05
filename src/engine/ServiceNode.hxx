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
#ifndef __SERVICENODE_HXX__
#define __SERVICENODE_HXX__

#include "ElementaryNode.hxx"
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class ComponentInstance;

/*! \brief Class for calculation node associated with a component service
 *
 * \ingroup Nodes
 *
 * \see InlineNode
 * \see ElementaryNode
 */
    class ServiceNode: public ElementaryNode 
    {
    protected:
      ServiceNode(const std::string& name);
      ServiceNode(const ServiceNode& other, ComposedNode *father);
      void performDuplicationOfPlacement(const Node& other);
    public:
      virtual void load();
      virtual bool isDeployable() const;
      virtual void setComponent(ComponentInstance* compo) throw(Exception);
      virtual ComponentInstance *getComponent();
      virtual void setRef(const std::string& ref);
      virtual std::string getRef();
      virtual void setMethod(const std::string& method){ _method=method; }
      virtual std::string getMethod(){return _method;}
      virtual ServiceNode *createNode(const std::string& name) = 0;
      virtual ~ServiceNode();
      virtual void accept(Visitor *visitor);

//! Return the service node kind
/*!
 * A runtime can provide several implementations of a service node.
 * Each implementation has a different kind. A ComponentInstance can be
 * associated to a ServiceNode with the same kind.
 */
      virtual std::string getKind() const;
      static const char KIND[];
      virtual std::string typeName() {return "YACS__ENGINE__ServiceNode";}
    protected:
      ComponentInstance* _component;
      std::string _method;
      std::string _ref;
    };
  }
}

#endif
