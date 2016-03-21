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

#ifndef __SALOMEHPCONTAINER_HXX__
#define __SALOMEHPCONTAINER_HXX__

#include "YACSRuntimeSALOMEExport.hxx"
#include "HomogeneousPoolContainer.hxx"
#include "SalomeContainerHelper.hxx"
#include "SalomeContainerTools.hxx"
#include "SalomeHPContainerTools.hxx"
#include "Mutex.hxx"
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

    class YACSRUNTIMESALOME_EXPORT SalomeHPContainer : public HomogeneousPoolContainer
    {
    public:
      SalomeHPContainer();
      SalomeHPContainer(const SalomeHPContainer& other);
      //HP specific part
      void setSizeOfPool(int sz);
      int getSizeOfPool() const;
      std::size_t getNumberOfFreePlace() const;
      void allocateFor(const std::vector<const Task *>& nodes);
      void release(const Task *node);
      //! For thread safety for concurrent load operation on same Container.
      void lock();
      //! For thread safety for concurrent load operation on same Container.
      void unLock();
      //
      std::string getKind() const;
      std::string getDiscreminantStrOfThis(const Task *askingNode) const;
      bool isAlreadyStarted(const Task *askingNode) const;
      void start(const Task *askingNode) throw(Exception);
      void shutdown(int level);
      std::string getPlacementId(const Task *askingNode) const;
      std::string getFullPlacementId(const Task *askingNode) const;
      Container *clone() const;
      Container *cloneAlways() const;
      void setProperty(const std::string& name,const std::string& value);
      std::string getProperty(const std::string& name) const;
      void clearProperties();
      void addComponentName(const std::string& name);
      std::map<std::string,std::string> getProperties() const;
      std::map<std::string,std::string> getResourceProperties(const std::string& name) const;
      void checkCapabilityToDealWith(const ComponentInstance *inst) const throw(YACS::Exception);
      //
#ifndef SWIG
      std::size_t locateTask(const Task *askingNode) const { return _launchModeType.locateTask(askingNode); }
      const SalomeContainerTools &getContainerInfo() const { return _sct; }
      std::vector<std::string> getComponentNames() const { return _componentNames; }
      int getShutdownLev() const { return _shutdownLevel; }
      SalomeContainerMonoHelper *getHelperOfTask(const Task *node) { return _launchModeType.getHelperOfTask(node); }
      const SalomeContainerMonoHelper *getHelperOfTask(const Task *node) const { return _launchModeType.getHelperOfTask(node); }
      //
      YACS::BASES::Mutex& getLocker() { return _mutex; }
#endif
    public:
      static const char KIND[];
    protected:
#ifndef SWIG
      ~SalomeHPContainer();
#endif
    protected:
      int _shutdownLevel;
      SalomeContainerTools _sct;
      YACS::BASES::Mutex _mutex;
      std::vector<std::string> _componentNames;
      //
      SalomeHPContainerVectOfHelper _launchModeType;
      std::string _initScript;
    };
  }
}

#endif
