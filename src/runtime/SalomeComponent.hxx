// Copyright (C) 2006-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#ifndef _SALOMECOMPONENT_HXX_
#define _SALOMECOMPONENT_HXX_

#include "YACSRuntimeSALOMEExport.hxx"
#include "ComponentInstance.hxx"
#include <omniORB4/CORBA.h>

namespace YACS
{
  namespace ENGINE
  {
    class ServiceNode;

/*! \brief Class for Salome component instance
 *
 *
 */
    class YACSRUNTIMESALOME_EXPORT SalomeComponent : public ComponentInstance
    {
    public:
      SalomeComponent(const std::string& name);
      SalomeComponent(const SalomeComponent& other);
      virtual ~SalomeComponent();
      virtual void load();
      virtual void unload();
      virtual bool isLoaded();
      virtual void setContainer(Container *cont);
      virtual ServiceNode* createNode(const std::string& name);
      virtual ComponentInstance* clone() const;
      virtual std::string getFileRepr() const;
      virtual CORBA::Object_ptr getCompoPtr(){return CORBA::Object::_duplicate(_objComponent);}
      virtual void shutdown(int level);
    public:
      static const char KIND[];
      virtual std::string getKind() const;
    protected:
      CORBA::Object_var _objComponent;
    };
  }
}

#endif
