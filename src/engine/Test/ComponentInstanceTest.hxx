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

#ifndef __COMPONENTINTANCETEST_HXX__
#define __COMPONENTINTANCETEST_HXX__

#include "ComponentInstance.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class ComponentInstanceTest1 : public ComponentInstance
    {
    public:
      ComponentInstanceTest1(const ComponentInstanceTest1& other);
      ComponentInstanceTest1(const std::string& name);
      void load(Task *askingNode);
      void unload(Task *askingNode);
      bool isLoaded(Task *askingNode) const;
      std::string getKind() const;
      std::string getKindForNode() const;
      ServiceNode* createNode(const std::string& name);
      ComponentInstance *clone() const;
      ComponentInstance *cloneAlways() const;
    protected:
      bool _loaded;
    };

    class ComponentInstanceTest2 : public ComponentInstance
    {
    public:
      ComponentInstanceTest2(const ComponentInstanceTest2& other);
      ComponentInstanceTest2(const std::string& name);
      void load(Task *askingNode);
      void unload(Task *askingNode);
      bool isLoaded(Task *askingNode) const;
      std::string getKind() const;
      std::string getKindForNode() const;
      ServiceNode* createNode(const std::string& name);
      ComponentInstance *clone() const;
      ComponentInstance *cloneAlways() const;
    protected:
      bool _loaded;
    };
  }
}

#endif
