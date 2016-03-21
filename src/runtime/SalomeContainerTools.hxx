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

#ifndef __SALOMECONTAINERTOOLS_HXX__
#define __SALOMECONTAINERTOOLS_HXX__

#include "YACSRuntimeSALOMEExport.hxx"
#include "SALOMEconfig.h"
#include CORBA_CLIENT_HEADER(SALOME_ContainerManager)

#include <string>
#include <vector>
#include <map>

namespace YACS
{
  namespace ENGINE
  {
    class Task;
    class Container;
    class ComponentInstance;
    class SalomeContainerHelper;

    class YACSRUNTIMESALOME_EXPORT SalomeContainerTools
    {
    public:
      SalomeContainerTools();
      SalomeContainerTools(const SalomeContainerTools& other);
      std::string getProperty(const std::string& name) const;
      void setProperty(const std::string& name, const std::string& value);
      const std::map<std::string,std::string>& getProperties() const { return _propertyMap; }
      void clearProperties();
      std::map<std::string,std::string> getResourceProperties(const std::string& name) const;
      void addToComponentList(const std::string& name);
      void addToResourceList(const std::string& name);
    public:
      std::string getContainerName() const;
      void setContainerName(const std::string& name);
      std::string getNotNullContainerName(const Container *contPtr, const Task *askingNode, bool& isEmpty) const;
      std::string getHostName() const;
      Engines::ContainerParameters getParameters() const { return _params; }
      static void SetContainerNameOf(Engines::ContainerParameters& params, const std::string& name);
    public:
      static void Start(const std::vector<std::string>& compoNames, SalomeContainerHelper *schelp, SalomeContainerTools& sct, int& shutdownLevel, const Container *cont, const Task *askingNode);
      static CORBA::Object_ptr LoadComponent(SalomeContainerHelper *launchModeType, Container *cont, Task *askingNode);
      static CORBA::Object_ptr CreateComponentInstance(Container *cont, Engines::Container_ptr contPtr, const ComponentInstance *inst);
      static std::string GetPlacementId(const SalomeContainerHelper *launchModeType, const Container *cont, const Task *askingNode);
      static std::string GetFullPlacementId(const SalomeContainerHelper *launchModeType, const Container *cont, const Task *askingNode);
    protected:
      std::map<std::string,std::string> _propertyMap;
      Engines::ContainerParameters _params;
    };
  }
}

#endif
