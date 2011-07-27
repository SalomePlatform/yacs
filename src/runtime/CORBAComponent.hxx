//  Copyright (C) 2006-2011  CEA/DEN, EDF R&D
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

#ifndef _CORBACOMPONENT_HXX_
#define _CORBACOMPONENT_HXX_

#include "ComponentInstance.hxx"
#include <omniORB4/CORBA.h>

namespace YACS
{
  namespace ENGINE
  {
    class ServiceNode;

/*! \brief Class for CORBA component instance
 *
 *
 */
    class CORBAComponent : public ComponentInstance
    {
    public:
      CORBAComponent(const std::string& name);
      CORBAComponent(const CORBAComponent& other);
      virtual ~CORBAComponent();
      virtual void load();
      virtual void unload();
      virtual bool isLoaded();
      virtual ServiceNode* createNode(const std::string& name);
      virtual ComponentInstance* clone() const;
      virtual std::string getFileRepr() const;
      virtual CORBA::Object_ptr getCompoPtr();
    public:
      static const char KIND[];
      virtual std::string getKind() const;
    protected:
      CORBA::Object_var _objComponent;
    };
  }
}

#endif
