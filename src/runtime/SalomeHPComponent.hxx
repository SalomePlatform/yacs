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

#ifndef _SALOMEHPCOMPONENT_HXX_
#define _SALOMEHPCOMPONENT_HXX_

#include "YACSRuntimeSALOMEExport.hxx"
#include "ComponentInstance.hxx"
#include <omniORB4/CORBA.h>

namespace YACS
{
  namespace ENGINE
  {
    class YACSRUNTIMESALOME_EXPORT SalomeHPComponent : public ComponentInstance
    {
    public:
      SalomeHPComponent(const std::string& name);
      SalomeHPComponent(const SalomeHPComponent& other);
      virtual ~SalomeHPComponent();
      virtual void load(Task *askingNode);
      virtual void unload(Task *askingNode);
      virtual bool isLoaded(Task *askingNode) const;
      virtual bool setContainer(Container *cont);
      virtual ServiceNode* createNode(const std::string& name);
      virtual ComponentInstance* clone() const;
      virtual ComponentInstance* cloneAlways() const;
      virtual std::string getFileRepr() const;
      virtual CORBA::Object_ptr getCompoPtr(){return CORBA::Object::_duplicate(_objComponent);}
      virtual void shutdown(int level);
    public:
      static const char KIND[];
      virtual std::string getKind() const;
      virtual std::string getKindForNode() const;
    protected:
      CORBA::Object_var _objComponent;
    };
  }
}

#endif
