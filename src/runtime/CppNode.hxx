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

#ifndef _CPPNODE_HXX_
#define _CPPNODE_HXX_

#include "ServiceNode.hxx"
#include "YACSRuntimeSALOMEExport.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Any;

    // local C++ implementation - single process

    class CppComponent;

    typedef void (*MYRUN)(int nbin, int nbout, 
                          YACS::ENGINE::Any **in, YACS::ENGINE::Any ** out);
    
/*! \brief Class for C++ Nodes (in process component)
 *
 * local C++ implementation - single process
 *
 * \ingroup Nodes
 *
 */
    class YACSRUNTIMESALOME_EXPORT CppNode : public YACS::ENGINE::ServiceNode 
    {
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
    public:
      CppNode(const CppNode &other, ComposedNode *father);
      CppNode(const std::string &name);
      virtual void load();
      virtual void execute();
      virtual ServiceNode* createNode(const std::string& name);
      CppNode* cloneNode(const std::string& name);

      void setCode(const std::string & componentName, const std::string & service);
      void setFunc(MYRUN fonc);
    
      static const char IMPL_NAME[];
      static const char KIND[];
      std::string getKind() const { return CppNode::KIND; }
    protected:
      std::string _componentName;
      MYRUN _run;

    };
  }
}

#endif
