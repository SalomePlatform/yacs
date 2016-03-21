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

#ifndef __CONTAINER_HXX__
#define __CONTAINER_HXX__

#include "YACSlibEngineExport.hxx"
#include "Exception.hxx"
#include "RefCounter.hxx"

#include <list>
#include <map>

namespace YACS
{
  namespace ENGINE
  {
    class ComponentInstance;
    class Proc;
    class Task;
    /*!
     * This is an abstract class, that represents an abstract process in which ComponentInstances can be launched and run.
     * An instance of this will be mapped to one and only one physical container (except in the foreach context)
     */
    class YACSLIBENGINE_EXPORT Container : public RefCounter
    {
    protected:
      Container();
#ifndef SWIG
      virtual ~Container();
#endif
    public:
      virtual std::string getKind() const = 0;
      //Execution only methods
      virtual std::string getDiscreminantStrOfThis(const Task *askingNode) const;
      virtual bool isAlreadyStarted(const Task *askingNode) const = 0;
      virtual void start(const Task *askingNode) throw(Exception) = 0;
      virtual std::string getPlacementId(const Task *askingNode) const = 0;
      virtual std::string getFullPlacementId(const Task *askingNode) const = 0;
      //Edition only methods
      virtual void setAttachOnCloningStatus(bool val) const;
      virtual void attachOnCloning() const;
      virtual void dettachOnCloning() const;
      virtual bool isAttachedOnCloning() const;
      virtual void lock() = 0;
      virtual void unLock() = 0;
      //! \b WARNING ! clone behaviour \b MUST be in coherence with what is returned by isAttachedOnCloning() method
      virtual Container *clone() const = 0;
      virtual Container *cloneAlways() const = 0;
      virtual bool isSupportingRTODefNbOfComp() const;
      virtual void checkCapabilityToDealWith(const ComponentInstance *inst) const throw(Exception) = 0;
      virtual void setProperty(const std::string& name,const std::string& value) = 0;
      virtual std::string getProperty(const std::string& name) const = 0;
      virtual void clearProperties() = 0;
      virtual void addComponentName(const std::string& name) = 0;
      virtual std::map<std::string,std::string> getProperties() const = 0;
      virtual std::map<std::string,std::string> getResourceProperties(const std::string& name) const = 0;
      virtual void setProperties(const std::map<std::string,std::string>& properties);
      std::string getName() const { return _name; }
      //! \b WARNING ! name is used in edition to identify different containers, it is not the runtime name of the container
      void setName(std::string name) { _name = name; }
      void setProc(Proc* proc) { _proc = proc; }
      Proc* getProc() { return _proc; }
      virtual void shutdown(int level) = 0;
      static const char KIND_ENTRY[];
      static const char AOC_ENTRY[];
    protected:
      std::string _name;
      mutable bool _isAttachedOnCloning;
      Proc* _proc;
    };
  }
}

#endif
