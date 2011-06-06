// Copyright (C) 2006-2011  CEA/DEN, EDF R&D
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

#ifndef __SALOMECONTAINER_HXX__
#define __SALOMECONTAINER_HXX__

#include "YACSRuntimeSALOMEExport.hxx"
#include "Container.hxx"
#include "Mutex.hxx"
#include <string>
#include <vector>
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Component)
#include CORBA_CLIENT_HEADER(SALOME_ContainerManager)

namespace YACS
{
  namespace ENGINE
  {
    class SalomeComponent;

    class YACSRUNTIMESALOME_EXPORT SalomeContainer : public Container
    {
      friend class SalomeComponent;
    public:
      SalomeContainer();
      SalomeContainer(const SalomeContainer& other);
      //! For thread safety for concurrent load operation on same Container.
      void lock();
      //! For thread safety for concurrent load operation on same Container.
      void unLock();
      bool isAlreadyStarted(const ComponentInstance *inst) const;
      Engines::Container_ptr getContainerPtr(const ComponentInstance *inst) const;
      void start(const ComponentInstance *inst) throw (Exception);
      Container *clone() const;
      std::string getPlacementId(const ComponentInstance *inst) const;
      void checkCapabilityToDealWith(const ComponentInstance *inst) const throw (Exception);
      virtual void setProperty(const std::string& name, const std::string& value);
      virtual void addComponentName(std::string name);
      virtual CORBA::Object_ptr loadComponent(ComponentInstance *inst);
      virtual void shutdown(int level);
      // Helper methods
      void addToComponentList(const std::string & name);
      void addToResourceList(const std::string & name);
      virtual std::map<std::string,std::string> getResourceProperties(const std::string& name);
    protected:
#ifndef SWIG
      virtual ~SalomeContainer();
#endif
    protected:
      //! thread safety in Salome ???
      YACS::BASES::Mutex _mutex;
      Engines::Container_var _trueCont;
      std::vector<std::string> _componentNames;
      std::map<const ComponentInstance *,Engines::Container_var> _trueContainers;
      std::string _type;
      int _shutdownLevel;
    public:
      Engines::ContainerParameters _params;
    };
  }
}

#endif
