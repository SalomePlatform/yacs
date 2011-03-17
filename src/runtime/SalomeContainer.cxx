//  Copyright (C) 2006-2010  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//#define REFCNT
//
#ifdef REFCNT
#define private public
#define protected public
#include <omniORB4/CORBA.h>
#include <omniORB4/internal/typecode.h>
#endif

#include "RuntimeSALOME.hxx"
#include "SalomeContainer.hxx"
#include "SalomeComponent.hxx"
#include "Proc.hxx"

#include "SALOME_NamingService.hxx"
#include "SALOME_LifeCycleCORBA.hxx"
#include "SALOME_ContainerManager.hxx"
#include "Basics_Utils.hxx"
#include "OpUtil.hxx"

#include <sstream>
#include <iostream>

#ifdef WNT
#include <process.h>
#define getpid _getpid
#endif

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

SalomeContainer::SalomeContainer():_trueCont(Engines::Container::_nil()),_type("mono"),_shutdownLevel(999)
{
  /* Init ContainerParameters */
  _params.container_name = "";
  _params.mode= "start";
  _params.workingdir= "";
  _params.nb_proc = 0;
  _params.isMPI = false;
  _params.parallelLib = "";

  _params.resource_params.name = "";
  _params.resource_params.hostname = "";
  _params.resource_params.OS = "";
  _params.resource_params.nb_proc = 0;
  _params.resource_params.mem_mb = 0;
  _params.resource_params.cpu_clock = 0;
  _params.resource_params.nb_node = 0;
  _params.resource_params.nb_proc_per_node = 0;
  _params.resource_params.policy = "";
  // By default, componentList and resList length is 0
}

SalomeContainer::SalomeContainer(const SalomeContainer& other):Container(other),_trueCont(Engines::Container::_nil()),_type(other._type),
                                                                                _shutdownLevel(other._shutdownLevel)
{
  _params.container_name = CORBA::string_dup(other._params.container_name);
  _params.mode= CORBA::string_dup(other._params.mode);
  _params.workingdir= CORBA::string_dup(other._params.workingdir);
  _params.nb_proc = other._params.nb_proc;
  _params.isMPI = other._params.isMPI;
  _params.parallelLib = CORBA::string_dup(other._params.parallelLib);

  _params.resource_params.name = CORBA::string_dup(other._params.resource_params.name);
  _params.resource_params.hostname = CORBA::string_dup(other._params.resource_params.hostname);
  _params.resource_params.OS = CORBA::string_dup(other._params.resource_params.OS);
  _params.resource_params.nb_proc = other._params.resource_params.nb_proc;
  _params.resource_params.mem_mb = other._params.resource_params.mem_mb;
  _params.resource_params.cpu_clock = other._params.resource_params.cpu_clock;
  _params.resource_params.nb_node = other._params.resource_params.nb_node;
  _params.resource_params.nb_proc_per_node = other._params.resource_params.nb_proc_per_node;
  _params.resource_params.policy = CORBA::string_dup(other._params.resource_params.policy);

  _params.resource_params.componentList.length(other._params.resource_params.componentList.length());
  for(CORBA::ULong i=0; i < other._params.resource_params.componentList.length(); i++)
  {
    _params.resource_params.componentList[i]=CORBA::string_dup(other._params.resource_params.componentList[i]);
  }

  _params.resource_params.resList.length(other._params.resource_params.resList.length());
  for(CORBA::ULong i=0; i < other._params.resource_params.resList.length(); i++)
  {
    _params.resource_params.resList[i]=CORBA::string_dup(other._params.resource_params.resList[i]);
  }
}

SalomeContainer::~SalomeContainer()
{
}

void SalomeContainer::lock()
{
  _mutex.lock();
}

void SalomeContainer::unLock()
{
  _mutex.unlock();
}

Container *SalomeContainer::clone() const
{
  if(_isAttachedOnCloning)
    {
      incrRef();
      return (Container*) (this);
    }
  else
    return new SalomeContainer(*this);
}

void SalomeContainer::checkCapabilityToDealWith(const ComponentInstance *inst) const throw(YACS::Exception)
{
  if(inst->getKind()!=SalomeComponent::KIND)
    throw Exception("SalomeContainer::checkCapabilityToDealWith : SalomeContainer is not able to deal with this type of ComponentInstance.");
}

void SalomeContainer::setProperty(const std::string& name, const std::string& value)
{
  DEBTRACE("SalomeContainer::setProperty : " << name << " ; " << value);
   
  // Container Part
  if (name == "container_name")
    _params.container_name = CORBA::string_dup(value.c_str());
  else if (name == "type")
  {
    if (value == "mono")
      _params.mode = "start";
    else if (value == "multi")
      _params.mode = "getorstart";
    else 
      throw Exception("SalomeContainer::setProperty : type value is not correct (mono or multi): " + value);
    _type=value;
  }
  else if (name == "workingdir")
    _params.workingdir = CORBA::string_dup(value.c_str());
  else if (name == "nb_parallel_procs")
  {
    std::istringstream iss(value);
    if (!(iss >> _params.nb_proc))
      throw Exception("salomecontainer::setproperty : params.nb_proc value not correct : " + value);
  }
  else if (name == "isMPI")
  {
    if (value == "true")
      _params.isMPI = true;
    else if (value == "false")
      _params.isMPI = false;
    else 
      throw Exception("SalomeContainer::setProperty : params.isMPI value not correct : " + value);
  }
  else if (name == "parallelLib")
    _params.parallelLib = CORBA::string_dup(value.c_str());

  // Resource part
  else if (name == "name")
    _params.resource_params.name = CORBA::string_dup(value.c_str());
  else if (name == "hostname")
    _params.resource_params.hostname = CORBA::string_dup(value.c_str());
  else if (name == "OS")
    _params.resource_params.OS = CORBA::string_dup(value.c_str());
  else if (name == "nb_resource_procs")
  {
    std::istringstream iss(value);
    if (!(iss >> _params.resource_params.nb_proc))
      throw Exception("salomecontainer::setproperty : params.resource_params.nb_proc value not correct : " + value);
  }
  else if (name == "mem_mb")
  {
    std::istringstream iss(value);
    if (!(iss >> _params.resource_params.mem_mb))
      throw Exception("salomecontainer::setproperty : params.resource_params.mem_mb value not correct : " + value);
  }
  else if (name == "cpu_clock")
  {
    std::istringstream iss(value);
    if (!(iss >> _params.resource_params.cpu_clock))
      throw Exception("salomecontainer::setproperty : params.resource_params.cpu_clock value not correct : " + value);
  }
  else if (name == "nb_node")
  {
    std::istringstream iss(value);
    if (!(iss >> _params.resource_params.nb_node))
      throw Exception("salomecontainer::setproperty : params.nb_node value not correct : " + value);
  }
  else if (name == "nb_proc_per_node")
  {
    std::istringstream iss(value);
    if (!(iss >> _params.resource_params.nb_proc_per_node))
      throw Exception("salomecontainer::setproperty : params.nb_proc_per_node value not correct : " + value);
  }
  else if (name == "policy")
    _params.resource_params.policy = CORBA::string_dup(value.c_str());
  else if (name == "component_list")
  {
    std::string clean_value(value);

    // Step 1: remove blanks
    while(clean_value.find(" ") != std::string::npos)
      clean_value = clean_value.erase(clean_value.find(" "), 1);

    // Step 2: get values
    while(!clean_value.empty())
    {
      std::string result("");
      std::string::size_type loc = clean_value.find(",", 0);
      if (loc != std::string::npos)
      {
        result = clean_value.substr(0, loc);
        clean_value = clean_value.erase(0, loc+1);
      }
      else
      {
        result = clean_value;
        clean_value.erase();
      }
      if (result != "," && result != "")
      {
        addToComponentList(result);
      }
    }

  }
  else if (name == "resource_list")
  {
    std::string clean_value(value);

    // Step 1: remove blanks
    while(clean_value.find(" ") != std::string::npos)
      clean_value = clean_value.erase(clean_value.find(" "), 1);

    // Step 2: get values
    while(!clean_value.empty())
    {
      std::string result("");
      std::string::size_type loc = clean_value.find(",", 0);
      if (loc != std::string::npos)
      {
        result = clean_value.substr(0, loc);
        clean_value = clean_value.erase(0, loc+1);
      }
      else
      {
        result = clean_value;
        clean_value.erase();
      }
      if (result != "," && result != "")
      {
        addToResourceList(result);
      }
    }

  }
  // End
  Container::setProperty(name, value);
}

void SalomeContainer::addComponentName(std::string name)
{
  _componentNames.push_back(name);
}

//! Load a component instance in this container
/*!
 * \param inst the component instance to load
 */
CORBA::Object_ptr SalomeContainer::loadComponent(ComponentInstance *inst)
{
  DEBTRACE("SalomeContainer::loadComponent ");
  lock();//To be sure
  if(!isAlreadyStarted(inst))
    {
      try
        {
          start(inst);
        }
      catch(Exception& e)
        {
          unLock();
          throw e;
        }
    }
  unLock();
  lock();//To be sure
  CORBA::Object_ptr objComponent=CORBA::Object::_nil();
  std::string compoName=inst->getCompoName();
  const char* componentName=compoName.c_str();
  Engines::Container_var container;
  if(_type=="multi")
    container=_trueContainers[inst];
  else
    container=_trueCont;

  char* reason;

  bool isLoadable = container->load_component_Library(componentName, reason);
  if (isLoadable)
    {
      CORBA::string_free(reason);
      int studyid=1;
      Proc* p=getProc();
      if(p)
        {
          std::string value=p->getProperty("DefaultStudyID");
          if(!value.empty())
            studyid= atoi(value.c_str());
        }
      // prepare component instance properties
      Engines::FieldsDict_var env = new Engines::FieldsDict;
      std::map<std::string, std::string> properties = inst->getProperties();
      if(p)
        {
          std::map<std::string,std::string> procMap=p->getProperties();
          properties.insert(procMap.begin(),procMap.end());
        }

      std::map<std::string, std::string>::const_iterator itm;
      env->length(properties.size());
      int item=0;
      for(itm = properties.begin(); itm != properties.end(); ++itm, item++)
        {
          DEBTRACE("envname="<<itm->first<<" envvalue="<< itm->second);
          env[item].key= CORBA::string_dup(itm->first.c_str());
          env[item].value <<= itm->second.c_str();
        }

      objComponent=container->create_component_instance_env(componentName, studyid, env, reason);
    }

  if(CORBA::is_nil(objComponent))
    {
      unLock();
      std::string text="Error while trying to create a new component: component '"+ compoName;
      text=text+"' is not installed or it's a wrong name";
      text += '\n';
      text += reason;
      CORBA::string_free(reason);
      throw Exception(text);
    }
  unLock();
  return objComponent;
}

//! Get the container placement id for a component instance
/*!
 * \param inst the component instance
 * \return the placement id
 */
std::string SalomeContainer::getPlacementId(const ComponentInstance *inst) const
{

  if(isAlreadyStarted(inst))
    {
      Engines::Container_var container=_trueCont;
      if(_type=="multi")
        {
          std::map<const ComponentInstance *, Engines::Container_var>::const_iterator found = _trueContainers.find(inst);
          container=found->second;
        }
      const char *what="/";
      CORBA::String_var corbaStr=container->name();
      string ret(corbaStr);

      //Salome FOREVER ...
      std::string::size_type i=ret.find_first_of(what,0);
      i=ret.find_first_of(what, i==std::string::npos ? i:i+1);
      if(i!=std::string::npos)
        return ret.substr(i+1);
      return ret;
    }
  else
    return "Not placed yet !!!";
}

//! Check if the component instance container is already started
/*!
 * \param inst the component instance
 * \return true, if the container is already started, else false
 */
bool SalomeContainer::isAlreadyStarted(const ComponentInstance *inst) const
{
  if(_type=="mono")
    {
      if(CORBA::is_nil(_trueCont))
        return false;
      else
        return true;
    }
  else
    {
      if(_trueContainers.count(inst)==0)
        return false;
      else
        return true;
    }
}

Engines::Container_ptr SalomeContainer::getContainerPtr(const ComponentInstance *inst) const
{
  if(_type=="mono")
    {
      if(CORBA::is_nil(_trueCont))
        return Engines::Container::_nil();
      else
        return Engines::Container::_duplicate(_trueCont);
    }
  else
    {
      if(_trueContainers.count(inst)==0)
        return Engines::Container::_nil();
      else
        {
          std::map<const ComponentInstance *,Engines::Container_var>::const_iterator iter=_trueContainers.find(inst);
          return Engines::Container::_duplicate(iter->second);
        }
    }
}

//! Start a salome container (true salome container not yacs one) with given ContainerParameters (_params)
/*!
 * \param inst the component instance
 */
void SalomeContainer::start(const ComponentInstance *inst) throw(YACS::Exception)
{
  CORBA::ORB_ptr orb=getSALOMERuntime()->getOrb();
  SALOME_NamingService ns;
  try
    {
      ns.init_orb(orb);
    }
  catch(SALOME_Exception& e)
    {
      throw Exception("SalomeContainer::start : Unable to contact the SALOME Naming Service");
    }
  CORBA::Object_var obj=ns.Resolve(SALOME_ContainerManager::_ContainerManagerNameInNS);
  Engines::ContainerManager_var contManager=Engines::ContainerManager::_narrow(obj);

  std::string str(_params.container_name);
  DEBTRACE("SalomeContainer::start " << str <<";"<<_params.resource_params.hostname <<";"<<_type);

  // Finalize parameters with components found in the container
  std::vector<std::string>::iterator iter;
  for(CORBA::ULong i=0; i < _componentNames.size();i++)
    addToComponentList(_componentNames[i]);
  Engines::ContainerParameters myparams = _params;

  bool namedContainer=false;
  if(str != "")
    namedContainer=true;

  //If a container_name is given try to find an already existing container in naming service
  //If not found start a new container with the given parameters
  if (_type=="mono" && str != "")
    {
      myparams.mode="getorstart";
    }

  if (str == "")
  {
    //give a almost unique name to the container : Pid_Name_Addr
    std::ostringstream stream;
    stream << getpid();
    stream << "_";
    stream << _name;
    stream << "_";
    stream << (void *)(this);
    DEBTRACE("container_name="<<stream.str());
    myparams.container_name=CORBA::string_dup(stream.str().c_str());
    _shutdownLevel=1;
  }

  _trueCont=Engines::Container::_nil();
  if(namedContainer && _shutdownLevel==999)
    {
      //Make this only the first time start is called (_shutdownLevel==999)
      //If the container is named, first try to get an existing container
      //If there is an existing container use it and set the shutdown level to 3
      //If there is no existing container, try to launch a new one and set the shutdown level to 2
      myparams.mode="get";
      try
        { 
          _trueCont=contManager->GiveContainer(myparams);
        }
      catch( const SALOME::SALOME_Exception& ex )
        {
          std::string msg="SalomeContainer::start : no existing container : ";
          msg += '\n';
          msg += ex.details.text.in();
          DEBTRACE( msg );
        }
      catch(...)
        {
        }

      if(!CORBA::is_nil(_trueCont))
        {
          _shutdownLevel=3;
          DEBTRACE( "container found: " << str << " " << _shutdownLevel );
        }
      else
        {
          _shutdownLevel=2;
          myparams.mode="start";
          DEBTRACE( "container not found: " << str << " " << _shutdownLevel);
        }
    }

  if(CORBA::is_nil(_trueCont))
    try
      { 
        // --- GiveContainer is used in batch mode to retreive launched containers,
        //     and is equivalent to StartContainer when not in batch.
        _trueCont=contManager->GiveContainer(myparams);
      }
    catch( const SALOME::SALOME_Exception& ex )
      {
        std::string msg="SalomeContainer::start : Unable to launch container in Salome : ";
        msg += '\n';
        msg += ex.details.text.in();
        throw Exception(msg);
      }
    catch(CORBA::COMM_FAILURE&)
      {
        throw Exception("SalomeContainer::start : Unable to launch container in Salome : CORBA Comm failure detected");
      }
    catch(CORBA::Exception&)
      {
        throw Exception("SalomeContainer::start : Unable to launch container in Salome : Unexpected CORBA failure detected");
      }

  if(CORBA::is_nil(_trueCont))
    throw Exception("SalomeContainer::start : Unable to launch container in Salome. Check your CatalogResources.xml file");

  _trueContainers[inst]=_trueCont;

  CORBA::String_var containerName=_trueCont->name();
  CORBA::String_var hostName=_trueCont->getHostName();
  std::cerr << "SalomeContainer launched : " << containerName << " " << hostName << " " << _trueCont->getPID() << std::endl;

#ifdef REFCNT
    DEBTRACE(_trueCont->_PR_getobj()->pd_refCount );
    std::map<const ComponentInstance *, Engines::Container_var >::const_iterator it;
    for(it = _trueContainers.begin(); it != _trueContainers.end(); ++it)
      {
        DEBTRACE(it->second->_PR_getobj()->pd_refCount );
      }
#endif
}

void SalomeContainer::shutdown(int level)
{
  DEBTRACE("SalomeContainer::shutdown: " << _name << "," << level << "," << _shutdownLevel);
  if(level < _shutdownLevel)
    return;

  _shutdownLevel=999;
  //shutdown the SALOME containers
  if(_type=="multi")
    {
      std::map<const ComponentInstance *, Engines::Container_var >::const_iterator it;
      for(it = _trueContainers.begin(); it != _trueContainers.end(); ++it)
        {
          try
            {
              DEBTRACE("shutdown SALOME container: " );
              CORBA::String_var containerName=it->second->name();
              DEBTRACE(containerName);
              it->second->Shutdown();
              std::cerr << "shutdown SALOME container: " << containerName << std::endl;
            }
          catch(CORBA::Exception&)
            {
              DEBTRACE("Unexpected CORBA failure detected." );
            }
          catch(...)
            {
              DEBTRACE("Unknown exception ignored." );
            }
        }
      _trueContainers.clear();
    }
  else
    {
      try
        {
          DEBTRACE("shutdown SALOME container: " );
          CORBA::String_var containerName=_trueCont->name();
          DEBTRACE(containerName);
          _trueCont->Shutdown();
          std::cerr << "shutdown SALOME container: " << containerName << std::endl;
        }
      catch(...)
        {
          DEBTRACE("Unknown exception ignored." );
        }
      _trueCont=Engines::Container::_nil();
    }
}

void
SalomeContainer::addToComponentList(const std::string & name)
{
  // Search if name is already in the list
  for (CORBA::ULong i = 0; i < _params.resource_params.componentList.length(); i++)
  {
    std::string component_name = _params.resource_params.componentList[i].in();
    if (component_name == name)
      return;
  }

  // Add name to list
  CORBA::ULong lgth = _params.resource_params.componentList.length();
  _params.resource_params.componentList.length(lgth + 1);
  _params.resource_params.componentList[lgth] = CORBA::string_dup(name.c_str());
}

void
SalomeContainer::addToResourceList(const std::string & name)
{
  // Search if name is already in the list
  for (CORBA::ULong i = 0; i < _params.resource_params.resList.length(); i++)
  {
    std::string component_name = _params.resource_params.resList[i].in();
    if (component_name == name)
      return;
  }

  // Add name to list
  CORBA::ULong lgth = _params.resource_params.resList.length();
  _params.resource_params.resList.length(lgth + 1);
  _params.resource_params.resList[lgth] = CORBA::string_dup(name.c_str());
}

std::map<std::string,std::string> SalomeContainer::getResourceProperties(const std::string& name)
{
  std::map<std::string,std::string> properties;

  YACS::ENGINE::RuntimeSALOME* runTime = YACS::ENGINE::getSALOMERuntime();
  CORBA::ORB_ptr orb = runTime->getOrb();
  if (!orb) return properties;
  SALOME_NamingService namingService(orb);
  SALOME_LifeCycleCORBA lcc(&namingService);
  CORBA::Object_var obj = namingService.Resolve(SALOME_ResourcesManager::_ResourcesManagerNameInNS);
  if (CORBA::is_nil(obj)) return properties;
  Engines::ResourcesManager_var resManager = Engines::ResourcesManager::_narrow(obj);
  if (CORBA::is_nil(resManager)) return properties;

  std::ostringstream value;
  Engines::ResourceDefinition_var resource_definition = resManager->GetResourceDefinition(name.c_str());
  properties["hostname"]=resource_definition->hostname.in();
  properties["OS"]=resource_definition->OS.in();
  value.str(""); value << resource_definition->mem_mb;
  properties["mem_mb"]=value.str();
  value.str(""); value << resource_definition->cpu_clock;
  properties["cpu_clock"]=value.str();
  value.str(""); value << resource_definition->nb_node;
  properties["nb_node"]=value.str();
  value.str(""); value << resource_definition->nb_proc_per_node;
  properties["nb_proc_per_node"]=value.str();
  /*
  properties["component_list"]="";
  for(CORBA::ULong i=0; i < resource_definition->componentList.length(); i++)
    {
      if(i > 0)
        properties["component_list"]=properties["component_list"]+",";
      properties["component_list"]=properties["component_list"]+resource_definition->componentList[i].in();
    }
    */

  return properties;
}
