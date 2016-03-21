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

#ifndef _XMLNODE_HXX_
#define _XMLNODE_HXX_

#include "ServiceNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class XmlNode:public ServiceNode
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      XmlNode(const XmlNode& other, ComposedNode *father);
      XmlNode(const std::string& name);
      virtual void execute();
      //! \b DISTRIBUTION \b NOT \b YET implemented for XMLNode.
      virtual void load() { }
      virtual void setRef(const std::string& ref);
      virtual void setScript(const std::string& script);
      virtual std::string getScript() const;
      virtual ServiceNode* createNode(const std::string& name) 
        { throw Exception("not implemented"); }
      virtual std::string getKind() const;
    public:
      static const char IMPL_NAME[];
      static const char KIND[];
    protected:
      std::string _script;
    };
  }
}

#endif
