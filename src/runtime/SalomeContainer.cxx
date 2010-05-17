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

SalomeContainer::SalomeContainer():_trueCont(Engines::Container::_nil()),_type("mono")
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
}

SalomeContainer::SalomeContainer(const SalomeContainer& other):Container(other),_trueCont(Engines::Container::_nil()),_type(other._type)
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
  else if (name == "nb_component_nodes") // TODO - TO Change.... in two nb_proc
  {
    std::istringstream iss(value);
    if (!(iss >> _params.nb_proc))
      throw Exception("salomecontainer::setproperty : params.nb_component_nodes value not correct : " + value);
    _params.resource_params.nb_proc=_params.nb_proc;
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
  else if (name == "mem_mb")
  {
    std::istringstream iss(value);
    if (!(iss >> _params.resource_params.mem_mb))
      throw Exception("salomecontainer::setproperty : params.mem_mb value not correct : " + value);
  }
  else if (name == "cpu_clock")
  {
    std::istringstream iss(value);
    if (!(iss >> _params.resource_params.cpu_clock))
      throw Exception("salomecontainer::setproperty : params.cpu_clock value not correct : " + value);
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
  //If a container_name is given try to find an already existing container in naming service
  //If not found start a new container with the given parameters
  if (_type=="mono" && str != "")
    {
      std::string machine(_params.resource_params.hostname);
      if(machine == "" || machine == "localhost")
        machine=Kernel_Utils::GetHostname();
      std::string ContainerNameInNS=ns.BuildContainerNameForNS(_params,machine.c_str());
      obj=ns.Resolve(ContainerNameInNS.c_str());
      if(!CORBA::is_nil(obj))
        {
          std::cerr << "Container already exists: " << ContainerNameInNS << std::endl;
          _trueCont=Engines::Container::_narrow(obj);
          _trueContainers[inst]=_trueCont;
          return;
        }
    }

  Engines::ContainerParameters myparams=_params;

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
  }
  myparams.resource_params.componentList.length(_componentNames.size());
  std::vector<std::string>::iterator iter;
  for(CORBA::ULong i=0; i < _componentNames.size();i++)
  {
    myparams.resource_params.componentList[i]=CORBA::string_dup(_componentNames[i].c_str());
  }
  myparams.resource_params.policy=CORBA::string_dup(getProperty("policy").c_str());

  try
    { 
      // --- GiveContainer is used in batch mode to retreive launched containers,
      //     and is equivalent to StartContainer when not in batch.
      _trueCont=contManager->GiveContainer(myparams);
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

