// Copyright (C) 2006-2019  CEA/DEN, EDF R&D
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

    class YACSRUNTIMESALOME_EXPORT SalomeContainerToolsBase
    {
    public:
      virtual ~SalomeContainerToolsBase() { }
      virtual std::string getProperty(const std::string& name) const = 0;
      virtual void setProperty(const std::string& name, const std::string& value) = 0;
      virtual const std::map<std::string,std::string>& getProperties() const = 0;
      virtual void clearProperties() = 0;
      virtual std::map<std::string,std::string> getResourceProperties(const std::string& name) const = 0;
      virtual void addToComponentList(const std::string& name) = 0;
      virtual void addToResourceList(const std::string& name) = 0;
      virtual Engines::ContainerParameters getParameters() const = 0;
      virtual std::string getContainerName() const = 0;
      virtual void setContainerName(const std::string& name) = 0;
      virtual std::string getHostName() const = 0;
      virtual std::string getNotNullContainerName(const Container *contPtr, const Task *askingNode, bool& isEmpty) const = 0;
    public:
      static void Start(const std::vector<std::string>& compoNames, SalomeContainerHelper *schelp, SalomeContainerToolsBase& sct, int& shutdownLevel, const Container *cont, const Task *askingNode);
      static CORBA::Object_ptr LoadComponent(SalomeContainerHelper *launchModeType, Container *cont, Task *askingNode);
      static CORBA::Object_ptr CreateComponentInstance(Container *cont, Engines::Container_ptr contPtr, const ComponentInstance *inst);
      static std::string GetPlacementId(const SalomeContainerHelper *launchModeType, const Container *cont, const Task *askingNode);
      static std::string GetFullPlacementId(const SalomeContainerHelper *launchModeType, const Container *cont, const Task *askingNode);
      static void SetContainerNameOf(Engines::ContainerParameters& params, const std::string& name);
    };
    
    class YACSRUNTIMESALOME_EXPORT SalomeContainerToolsInter : public SalomeContainerToolsBase
    {
    public:
      SalomeContainerToolsInter() { }
      SalomeContainerToolsInter(const SalomeContainerToolsInter& other);
      std::string getProperty(const std::string& name) const;
      const std::map<std::string,std::string>& getProperties() const { return _propertyMap; }
      void clearProperties();
      std::map<std::string,std::string> getResourceProperties(const std::string& name) const;
    protected:
      std::map<std::string,std::string> _propertyMap;
    };

    class YACSRUNTIMESALOME_EXPORT SalomeContainerTools : public SalomeContainerToolsInter
    {
    public:
      SalomeContainerTools();
      SalomeContainerTools(const SalomeContainerTools& other);
      int getNumberOfCoresPerWorker() const;
    public:
      Engines::ContainerParameters getParameters() const { return _params; }
      void clearProperties();
      void setProperty(const std::string& name, const std::string& value);
      void addToComponentList(const std::string& name);
      void addToResourceList(const std::string& name);
      std::string getContainerName() const;
      void setContainerName(const std::string& name);
      std::string getHostName() const;
      std::string getNotNullContainerName(const Container *contPtr, const Task *askingNode, bool& isEmpty) const;
    protected:
      Engines::ContainerParameters _params;
    };

    class PlayGround;
    
    class YACSRUNTIMESALOME_EXPORT SalomeContainerToolsDecoratorBase : public SalomeContainerToolsBase
    {
    protected:
      SalomeContainerToolsDecoratorBase(SalomeContainerToolsBase *sct):_sct(sct) { }
      SalomeContainerToolsBase *getWorker() { return _sct; }
      const SalomeContainerToolsBase *getWorker() const { return _sct; }
    public:
      std::string getProperty(const std::string& name) const;
      void setProperty(const std::string& name, const std::string& value);
      const std::map<std::string,std::string>& getProperties() const;
      void clearProperties();
      std::map<std::string,std::string> getResourceProperties(const std::string& name) const;
      void addToComponentList(const std::string& name);
      void addToResourceList(const std::string& name);
      Engines::ContainerParameters getParameters() const;
      std::string getContainerName() const;
      void setContainerName(const std::string& name);
      std::string getHostName() const;
      std::string getNotNullContainerName(const Container *contPtr, const Task *askingNode, bool& isEmpty) const;
    private:
      SalomeContainerToolsBase *_sct;
    };

    class SalomeHPContainerVectOfHelper;
    
    class YACSRUNTIMESALOME_EXPORT SalomeContainerToolsSpreadOverTheResDecorator : public SalomeContainerToolsDecoratorBase
    {
    public:
      SalomeContainerToolsSpreadOverTheResDecorator(SalomeContainerToolsBase *sct, const PlayGround *pg, const SalomeHPContainerVectOfHelper *vh, const Task *node):SalomeContainerToolsDecoratorBase(sct),_pg(pg),_vh(vh),_node(node) { }
      Engines::ContainerParameters getParameters() const;
    private:
      static std::string DeduceMachineFrom(const std::vector< std::pair<std::string,int> >& allResInfo, int iPos, int sz, int nbProcPerNode);
    private:
      const PlayGround *_pg;
      const SalomeHPContainerVectOfHelper *_vh;
      const Task *_node;
    };
  }
}

#endif
