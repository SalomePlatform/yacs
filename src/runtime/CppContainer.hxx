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

#ifndef __Cpp_CONTAINER_HXX__
#define __Cpp_CONTAINER_HXX__

#include <map>
#include <string>
#include "Mutex.hxx"
#include "Container.hxx"
#include "CppComponent.hxx"

#ifdef WIN32
#include <windows.h>
#endif

namespace YACS
{
  namespace ENGINE
  {
  
    struct LocalLibrary {
      
#if defined( WIN32 )
      HMODULE handle;
#else
      void * handle;
#endif
      
      InitFunction initHandle;
      RunFunction runHandle;
      PingFunction pingHandle;
      TerminateFunction terminateHandle;
      
#if defined( WIN32 )
      LocalLibrary(HMODULE h, InitFunction i, RunFunction r, 
#else
      LocalLibrary(void *h, InitFunction i, RunFunction r, 
#endif
                            PingFunction p, TerminateFunction t) 
        : handle(h), initHandle(i), runHandle(r), 
                           pingHandle(p), terminateHandle(t) {}
      LocalLibrary() 
        : handle(NULL), initHandle(NULL), runHandle(NULL), 
                              pingHandle(NULL), terminateHandle(NULL) {}
                              
      bool good() {
        return (handle != NULL)    && (initHandle != NULL) && 
               (runHandle != NULL) && (terminateHandle != NULL);
      }
        
    };

    // Local container singleton
    class LocalContainer {

      friend class CppComponent;

    public:

      static YACS::ENGINE::LocalContainer * get();
      void destroy();
      LocalLibrary loadComponentLibrary(const std::string &, const char * prefix = NULL, 
                                        bool forcedLoad = false);
      CppComponent * createComponentInstance(const char * componentName);
      void createInternalInstance(const char * componentName, 
                                  void *& obj, RunFunction &r, TerminateFunction &t);
     void unLoadComponentLibrary(const std::string & aCompName);
      void unregisterComponentInstance(CppComponent * C);
                  
    protected:

      LocalContainer();
      virtual ~LocalContainer();

      YACS::BASES::Mutex _instance_mapMutex, _library_mapMutex;
      static std::map<std::string, LocalLibrary > _library_map; // libraries, loaded
      static std::multimap<std::string, CppComponent *>  _instance_map;
      
      
    private:
      static LocalContainer *_singleton;
      

    };

    class CppContainer : public Container
    {
      friend class CppComponent;
      friend class LocalContainer;
    public:
      CppContainer();
      virtual ~CppContainer();
      std::string getKind() const;
      bool isAlreadyStarted(const Task *askingNode) const;
      void start(const Task *askingNode) throw (YACS::Exception);
      void shutdown(int level);
      std::string getPlacementId(const Task *askingNode) const;
      std::string getFullPlacementId(const Task *askingNode) const;
      YACS::ENGINE::Container *clone() const;
      Container *cloneAlways() const;
      void lock();
      void unLock();

      void checkCapabilityToDealWith(const ComponentInstance *inst) const throw (YACS::Exception);
      bool loadComponentLibrary(const std::string & componentName) throw (YACS::Exception);
      CppComponent * createComponentInstance(const std::string & componentName, int studyID = 0);
      void createInternalInstance(const std::string & componentName, 
                                  void *& obj, RunFunction &r, TerminateFunction &t);
      void unregisterComponentInstance(CppComponent * C);
      //
      void setProperty(const std::string& name,const std::string& value) { }
      std::string getProperty(const std::string& name) const { return std::string(); }
      void clearProperties() { }
      void addComponentName(const std::string& name) { }
      std::map<std::string,std::string> getProperties() const { return std::map<std::string,std::string>(); }
      std::map<std::string,std::string> getResourceProperties(const std::string& name) const { return std::map<std::string,std::string>(); }
      //
    public:
      static char KIND[];
    protected:
      YACS::BASES::Mutex _mutex;
      LocalContainer * _trueCont;

    };
  };
};

#endif

