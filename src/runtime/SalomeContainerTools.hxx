// Copyright (C) 2006-2023  CEA/DEN, EDF R&D
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

#pragma once

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

    class YACSRUNTIMESALOME_EXPORT SalomeContainerToolsBase
    {
    public:
      virtual ~SalomeContainerToolsBase() { }
      virtual std::string getProperty(const std::string& name) const = 0;
      virtual void setProperty(const std::string& name, const std::string& value) = 0;
      virtual const std::map<std::string,std::string>& getProperties() const = 0;
      virtual std::map<std::string,std::string> getResourceProperties(const std::string& name) const = 0;
      virtual void clearProperties() = 0;
      virtual void addToComponentList(const std::string& name) = 0;
      virtual void addToResourceList(const std::string& name) = 0;
      virtual Engines::ContainerParameters getParameters() const = 0;
      virtual void setContainerName(const std::string& name) = 0;
      virtual std::string getHostName() const = 0;
      virtual std::string getNotNullContainerName(const Container *contPtr, const Task *askingNode, bool& isEmpty) const = 0;
      virtual std::string getContainerName() const = 0;
      virtual int getNumberOfCoresPerWorker() const = 0;
    public:
      static void SetContainerNameOf(Engines::ContainerParameters& params, const std::string& name);
    public:
      static void Start(const std::vector<std::string>& compoNames, SalomeContainerHelper *schelp, SalomeContainerToolsBase& sct, int& shutdownLevel, const Container *cont, const Task *askingNode);
      static CORBA::Object_ptr LoadComponent(SalomeContainerHelper *launchModeType, Container *cont, Task *askingNode);
      static CORBA::Object_ptr CreateComponentInstance(Container *cont, Engines::Container_ptr contPtr, const ComponentInstance *inst);
      static std::string GetPlacementId(const SalomeContainerHelper *launchModeType, const Container *cont, const Task *askingNode);
      static std::string GetFullPlacementId(const SalomeContainerHelper *launchModeType, const Container *cont, const Task *askingNode);
    };

    class YACSRUNTIMESALOME_EXPORT SalomeContainerTools : public SalomeContainerToolsBase
    {
    public:
      SalomeContainerTools();
      SalomeContainerTools(const SalomeContainerTools& other);
      ~SalomeContainerTools() { }
      std::string getProperty(const std::string& name) const override;
      void setProperty(const std::string& name, const std::string& value) override;
      const std::map<std::string,std::string>& getProperties() const override { return _propertyMap; }
      std::map<std::string,std::string> getResourceProperties(const std::string& name) const override;
      void clearProperties() override;
      void addToComponentList(const std::string& name) override;
      void addToResourceList(const std::string& name) override;
      Engines::ContainerParameters getParameters() const override { return _params; }
      void setContainerName(const std::string& name) override;
      std::string getHostName() const override;
      std::string getNotNullContainerName(const Container *contPtr, const Task *askingNode, bool& isEmpty) const override;
      std::string getContainerName() const override;
      int getNumberOfCoresPerWorker() const override;
    private:
      std::map<std::string,std::string> _propertyMap;
      Engines::ContainerParameters _params;
    };

    class PlayGround;
    class SalomeHPContainerVectOfHelper;

    class YACSRUNTIMESALOME_EXPORT SalomeContainerToolsDecorator : public SalomeContainerToolsBase
    {
    public:
      SalomeContainerToolsDecorator(SalomeContainerToolsBase *decorated, const PlayGround *pg, SalomeHPContainerVectOfHelper *vh, const Task *node, int nbCoresPerWorker)
      :_decorated(decorated),_pg(pg),_vh(vh),_node(node),_nb_cores_per_worker(nbCoresPerWorker) { }
      std::string getProperty(const std::string& name) const override { return _decorated->getProperty(name); }
      void setProperty(const std::string& name, const std::string& value) override { return _decorated->setProperty(name,value); }
      const std::map<std::string,std::string>& getProperties() const override { return _decorated->getProperties(); }
      std::map<std::string,std::string> getResourceProperties(const std::string& name) const override { return _decorated->getResourceProperties(name); }
      void clearProperties() override { return _decorated->clearProperties(); }
      void addToComponentList(const std::string& name) override { return _decorated->addToComponentList(name); }
      void addToResourceList(const std::string& name) override { return _decorated->addToResourceList(name); }
      // Everything for it
      Engines::ContainerParameters getParameters() const override;
      void setContainerName(const std::string& name) override { return _decorated->setContainerName(name); }
      std::string getHostName() const override { return _decorated->getHostName(); }
      std::string getNotNullContainerName(const Container *contPtr, const Task *askingNode, bool& isEmpty) const override { return _decorated->getNotNullContainerName(contPtr,askingNode,isEmpty); }
      std::string getContainerName() const override { return _decorated->getContainerName(); }
      int getNumberOfCoresPerWorker() const override { return _decorated->getNumberOfCoresPerWorker(); }
    private:
      SalomeContainerToolsBase *_decorated;
      const PlayGround *_pg;
      SalomeHPContainerVectOfHelper *_vh;
      const Task *_node;
      int _nb_cores_per_worker;
    };
  }
}
