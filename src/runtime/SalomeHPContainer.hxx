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

#ifndef __SALOMEHPCONTAINER_HXX__
#define __SALOMEHPCONTAINER_HXX__

#include "YACSRuntimeSALOMEExport.hxx"
#include "HomogeneousPoolContainer.hxx"
#include "SalomeContainerHelper.hxx"
#include "SalomeContainerTools.hxx"
#include "SalomeHPContainerTools.hxx"
#include "PlayGround.hxx"

#include "Mutex.hxx"
#include "AutoRefCnt.hxx"

#include <string>
#include <vector>
#include <map>
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Component)
#include CORBA_CLIENT_HEADER(SALOME_ContainerManager)

namespace YACS
{
  namespace ENGINE
  {
    class Task;
    class SalomeComponent;
    class SalomeHPContainer;
    
    class YACSRUNTIMESALOME_EXPORT SalomeHPContainerBase : public HomogeneousPoolContainer
    {
    protected:
      SalomeHPContainerBase(SalomeHPContainerVectOfHelper *resShared);
      SalomeHPContainerBase(SalomeHPContainerVectOfHelper *resShared, bool isRefEaten);
      SalomeHPContainerBase(const SalomeHPContainerBase& other);
      void startInternal(const Task *askingNode, SalomeContainerToolsBase& sct, const std::vector<std::string>& compoNames);
      SalomeHPContainer *getTheBoss();
      const SalomeHPContainer *getTheBoss() const;
    public:
      void release(const Task *node);
      //
      bool isAlreadyStarted(const Task *askingNode) const;
      void shutdown(int level);
      //! For thread safety for concurrent load operation on same Container.
      void lock();
      //! For thread safety for concurrent load operation on same Container.
      void unLock();
      void setSizeOfPool(int sz);
      int getSizeOfPool() const;
      void setProperty(const std::string& name,const std::string& value);
      std::string getProperty(const std::string& name) const;
      std::map<std::string,std::string> getProperties() const;
      void clearProperties();
      std::string getPlacementId(const Task *askingNode) const;
      std::string getFullPlacementId(const Task *askingNode) const;
      std::map<std::string,std::string> getResourceProperties(const std::string& name) const;
      void addComponentName(const std::string& name);
      void checkCapabilityToDealWith(const ComponentInstance *inst) const throw(YACS::Exception);
      Engines::Container_var getContainerPtr(const Task *askingNode) const;
      std::vector<std::string> getKernelContainerNames() const;
    public:
      int getShutdownLev() const { return _shutdownLevel; }
      YACS::BASES::AutoRefCnt<HomogeneousPoolContainer> decorate(YACS::BASES::AutoConstRefCnt<PartDefinition> pd);
#ifndef SWIG
      const SalomeHPContainerVectOfHelper *getDirectAccessToVecOfCont() const { return _launchModeType; }
#endif
    protected:
      int _shutdownLevel;
      YACS::BASES::AutoRefCnt<SalomeHPContainerVectOfHelper> _launchModeType;
      std::string _initScript;
    };
    
    class YACSRUNTIMESALOME_EXPORT SalomeHPContainer : public SalomeHPContainerBase
    {
    public:
      SalomeHPContainer();
      SalomeHPContainer(const SalomeHPContainer& other);
      //HP specific part
      std::size_t getNumberOfFreePlace() const;
      void allocateFor(const std::vector<const Task *>& nodes);
      //
      std::string getKind() const;
      std::string getDiscreminantStrOfThis(const Task *askingNode) const;
      void start(const Task *askingNode) throw(Exception);
      Container *clone() const;
      Container *cloneAlways() const;
      int getNumberOfCoresPerWorker() const;
      //! do nothing. because no special actions to do. Only for decorators
      void prepareMaskForExecution() const { }
      //
#ifndef SWIG
      void forYourTestsOnly(ForTestOmlyHPContCls *data) const;
      std::map<std::string,std::string> getResourceProperties(const std::string& name) const;
      void addComponentNameSpe(const std::string& name);
      void checkCapabilityToDealWithSpe(const ComponentInstance *inst) const throw(YACS::Exception);
      std::size_t locateTask(const Task *askingNode) const { return _launchModeType->locateTask(askingNode); }
      const SalomeContainerTools &getContainerInfo() const { return _sct; }
      SalomeContainerTools &getContainerInfo() { return _sct; }
      std::vector<std::string> getComponentNames() const { return _componentNames; }
      SalomeContainerMonoHelper *getHelperOfTask(const Task *node) { return _launchModeType->getHelperOfTask(node); }
      const SalomeContainerMonoHelper *getHelperOfTask(const Task *node) const { return _launchModeType->getHelperOfTask(node); }
      //
      HomogeneousPoolContainer *getDirectFather() { return NULL; }
      const HomogeneousPoolContainer *getDirectFather() const { return NULL; }
#endif
    public:
      static const char KIND[];
    protected:
#ifndef SWIG
      ~SalomeHPContainer();
#endif
    protected:
      SalomeContainerTools _sct;
      std::vector<std::string> _componentNames;
    };

#ifndef SWIG
    class YACSRUNTIMESALOME_EXPORT SalomeHPContainerShared : public SalomeHPContainerBase
    {
    public:
      SalomeHPContainerShared(YACS::BASES::AutoConstRefCnt<PartDefinition> pd, SalomeHPContainerVectOfHelper *resShared, SalomeHPContainerBase *directFather);
      HomogeneousPoolContainer *getDirectFather() { return _directFather; }
      const HomogeneousPoolContainer *getDirectFather() const { return _directFather; }
      int getNumberOfCoresPerWorker() const { return _directFather->getNumberOfCoresPerWorker(); }
      std::string getProperty(const std::string& name) const { return _directFather->getProperty(name); }
      void prepareMaskForExecution() const;
      void forYourTestsOnly(ForTestOmlyHPContCls *data) const;
      std::string getName() const;
      std::string getDiscreminantStrOfThis(const Task *askingNode) const;
    public:
      std::string getKind() const;
      void start(const Task *askingNode) throw(Exception);
      Container *clone() const;
      Container *cloneAlways() const;
      //
      void allocateFor(const std::vector<const Task *>& nodes);
      std::size_t getNumberOfFreePlace() const;
    private:
      YACS::BASES::AutoRefCnt<SalomeHPContainerBase> _directFather;
      YACS::BASES::AutoConstRefCnt<PartDefinition> _pd;
      //! ids in _launchModeType covered by _pd.
      mutable std::vector<std::size_t> _idsOfKernelContainers;
    };
#endif
  }
}

#endif
