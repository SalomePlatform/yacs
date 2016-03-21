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

#include <iostream>
#include <sstream>
#ifdef WIN32
#include <windows.h>
#define dlopen LoadLibrary
#define dlclose FreeLibrary
#define dlsym GetProcAddress
#else
#include <dlfcn.h>
#endif

#include "CppContainer.hxx"
#include "CppComponent.hxx"
#include "Exception.hxx"
#include <algorithm>
#include <iostream>


using namespace YACS::ENGINE;

char CppContainer::KIND[]="Cpp";

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

//=============================================================================
/*! 
 *  Local C++ container class constructor
 */
//=============================================================================

CppContainer::CppContainer() : _trueCont(0L)
{
    DEBTRACE("CppContainer::CppContainer()");
}


//=============================================================================
/*! 
 *  Local C++ container class destructor
 */
//=============================================================================

CppContainer::~CppContainer()
{
    DEBTRACE("CppContainer::~CppContainer()");
}

void CppContainer::lock()
{
  _mutex.lock();
}

void CppContainer::unLock()
{
  _mutex.unLock();
}

std::string CppContainer::getKind() const
{
  return KIND;
}

bool CppContainer::isAlreadyStarted(const Task *askingNode) const
{
  return NULL != _trueCont;
}

void CppContainer::start(const Task *askingNode) throw (YACS::Exception)
{
  _trueCont = LocalContainer::get();
}

void CppContainer::shutdown(int level)
{

}

Container *CppContainer::clone() const
{
  if(_isAttachedOnCloning)
    {
      incrRef();
      return (Container*) (this);
    }
  else
    return new CppContainer(*this);
}

Container *CppContainer::cloneAlways() const
{
  return new CppContainer(*this);
}

bool CppContainer::loadComponentLibrary(const std::string & componentName) throw (YACS::Exception)
    {
  if (_trueCont)
    {
      LocalLibrary L = _trueCont->loadComponentLibrary(componentName);
      return L.good();
    }
  else
    {
      std::string mesg = "CppContainer not started";
      throw YACS::Exception(mesg);
    }
  return false;
    }

CppComponent * CppContainer::createComponentInstance(const std::string & componentName, int /* studyID */)
{
  DEBTRACE("CppContainer::createComponentInstance");
  if (_trueCont)
    return _trueCont->createComponentInstance(componentName.c_str());
  else
    {
      std::string mesg = "CppContainer not started";
      throw YACS::Exception(mesg);
    }
}

void CppContainer::createInternalInstance(const std::string & name, void *&obj, 
                                          RunFunction &r, TerminateFunction &t)
{
  DEBTRACE("CppContainer::createInternalInstance");
  if (_trueCont)
    _trueCont->createInternalInstance(name.c_str(), obj, r, t);
  else
    {
      std::string mesg = "CppContainer not started";
      throw YACS::Exception(mesg);
    }
}

void CppContainer::unregisterComponentInstance(CppComponent *compo)
{
  if (_trueCont)
    _trueCont->unregisterComponentInstance(compo);
}


std::string CppContainer::getPlacementId(const Task *askingNode) const
{
  return "/";
}

std::string CppContainer::getFullPlacementId(const Task *askingNode) const
{
  return "/";
}

void CppContainer::checkCapabilityToDealWith(const ComponentInstance *inst) const throw(YACS::Exception)
{
  if(inst->getKind()!=CppComponent::KIND)
    throw Exception("CppContainer::checkCapabilityToDealWith : CppContainer is not able to deal with this type of ComponentInstance.");
}

std::map<std::string, LocalLibrary> LocalContainer::_library_map; // libraries, loaded
std::multimap<std::string, CppComponent *> LocalContainer::_instance_map;

LocalContainer * LocalContainer::_singleton = NULL;

LocalContainer::LocalContainer()
{
}

LocalContainer::~LocalContainer()
{
    destroy();
}

LocalContainer * LocalContainer::get()
{
    if (NULL == _singleton) 
    {
        _singleton = new LocalContainer;
    }
    return _singleton;
}

void LocalContainer::destroy()
{
  if (NULL == _singleton)
    return;

  // destroy all component instances
  _instance_mapMutex.lock(); // lock
  std::multimap<std::string, CppComponent *>::iterator iI, iJ;
  for (iI=_instance_map.begin(); iI != _instance_map.end(); iI = iJ)
    {
      iJ = iI++;
      iI->second->setContainer(NULL);
      delete iI->second;
    }
  _instance_map.clear();
  _instance_mapMutex.unLock(); // unlock

  // unload all dynamic libraries
  _library_mapMutex.lock();
  std::map<std::string, LocalLibrary>::iterator iL;
  for (iL=_library_map.begin(); iL != _library_map.end(); iL++)
    dlclose(iL->second.handle);
  _library_map.clear();
  _library_mapMutex.unLock();

  delete _singleton;
  _singleton = NULL;
}


//=============================================================================
//! Find or create a new C++ component instance
/*! 
 *  Create a new component class (C++ implementation)
 *  \param name : component name
 *
 *  Try to return a handle to an new instance of a C++ component
 *  If the associated library is not loaded, try to load it
 *  
 * \return a handle to the component instance or throw an exception
 *  if it's not possible
 */
//=============================================================================
CppComponent * LocalContainer::createComponentInstance(const char * name)
{
  void *o;
  RunFunction r;
  TerminateFunction t;
  
  createInternalInstance(name, o, r, t);
  
  CppComponent * C;
  C = new CppComponent(o, r, t, name);
  _instance_mapMutex.lock(); // lock to be alone 
  _instance_map.insert(std::pair<std::string, CppComponent *>(name, C));
  _instance_mapMutex.unLock(); // unlock
  return C;
}

void LocalContainer::createInternalInstance(const char *name, void *&obj, 
                                            RunFunction &r, TerminateFunction &t)
{
  LocalLibrary L;

  std::map<std::string, LocalLibrary>::iterator foundL = _library_map.find(name);
  if (foundL != _library_map.end())
    L = foundL->second;
  else
    L = loadComponentLibrary(name, NULL, false);
  
  r = L.runHandle;
  InitFunction i = L.initHandle;
  t = L.terminateHandle;
  
  PingFunction p = L.pingHandle;
  if (p) p();
  
  obj = i();
  
}

void LocalContainer::unregisterComponentInstance(CppComponent * C)
{
  _instance_mapMutex.lock(); // lock to be alone
  _instance_map.erase(C->getCompoName());
  _instance_mapMutex.unLock(); // unlock
}

inline void toupper (std::string & s)
{
  transform (s.begin (), s.end (), s.begin (), (int(*)(int)) toupper);
}

LocalLibrary  LocalContainer::loadComponentLibrary(const std::string & aCompName, const char * prefix, bool forcedLoad)
{

  // if forcedLoad is true, unload library if it exists
  // if forcedLoad is false, return the existing library or load it
  
  if (forcedLoad)
      unLoadComponentLibrary(aCompName);
  else 
    {
      std::map<std::string, LocalLibrary >::iterator itLib 
           = _library_map.find(aCompName);
      if (itLib !=  _library_map.end()) return itLib->second;
    }
    
  // --- try dlopen C++ component

  std::string sprefix;
  if (prefix)
    sprefix = prefix;
  else 
    {
      std::string s = aCompName + "_ROOT_DIR";
      toupper(s);
      const char * t = getenv(s.c_str());
      sprefix="";
      if (t) 
        {
          sprefix = t;
          sprefix += "/lib/salome";
        }
    }
  
#ifndef WIN32
  std::string impl_name = std::string ("lib") + aCompName + std::string("Local.so");
  if(sprefix != "")
    impl_name = sprefix + std::string("/") + impl_name;
#else
  std::string impl_name = aCompName + std::string("Local.dll");
  impl_name = sprefix + std::string("\\") + impl_name;
#endif
  DEBTRACE("impl_name = " << impl_name);
    
#if defined( WIN32 )
  HMODULE handle;
  handle = dlopen( impl_name.c_str() ) ;
#else
  void* handle;
  handle = dlopen( impl_name.c_str() , RTLD_LAZY ) ;
#endif

  const char * sError;
#if defined( WIN32 )
  sError = "Not available here !";
#endif
  
#if defined( WIN32 )
  if (!handle) 
#else
  sError = dlerror();
  if ((sError = dlerror()) || !handle) 
#endif
    {
      std::stringstream msg;
      msg << "Can't load shared library : " << impl_name
          << " (dlopen error : " << sError << ") at "
          << __FILE__ << ":" << __LINE__;
      throw YACS::Exception(msg.str());
    }
  
  void *ihandle, *rhandle, *phandle = NULL, *thandle = NULL;
      
  ihandle = dlsym(handle, "__init");
#if defined( WIN32 )
  if (!ihandle)
#else
  if (sError = dlerror()) 
#endif
    {
      dlclose(handle);
      std::stringstream msg;
      msg << "Library " << impl_name
          << " doesn't contains initialization function (" << sError << ") at "
          << __FILE__ << ":" << __LINE__;
      throw YACS::Exception(msg.str());
    }
  
  rhandle = dlsym(handle, "__run");
#if defined( WIN32 )
  if (!rhandle)
#else
  if (sError = dlerror()) 
#endif
    {
      dlclose(handle);
      std::stringstream msg;
      msg << "Library " << impl_name
          << " doesn't contains main switch function (" << sError << ") at "
          << __FILE__ << ":" << __LINE__;
      throw YACS::Exception(msg.str());
    }
      
  thandle = dlsym(handle, "__terminate");
#if defined( WIN32 )
  if (!thandle)
#else
  if (sError = dlerror()) 
#endif
    {
      dlclose(handle);
      std::stringstream msg;
      msg << "Library " << impl_name
          << " doesn't contains terminate function (" << sError << ") at "
          << __FILE__ << ":" << __LINE__;
      throw YACS::Exception(msg.str());
    }
  phandle = dlsym(handle, "__ping");
      
  _library_map[aCompName] = LocalLibrary(handle, (InitFunction) ihandle, 
                                                 (RunFunction)  rhandle, 
                                                 (PingFunction) phandle,
                                                 (TerminateFunction) thandle);
  return _library_map[aCompName];
}

void LocalContainer::unLoadComponentLibrary(const std::string & aCompName)
{
  std::map<std::string, LocalLibrary >::iterator itLib 
           = _library_map.find(aCompName);
  
  if (itLib ==  _library_map.end()) return;
  
  dlclose(itLib->second.handle);
  _library_map.erase(itLib);

}
