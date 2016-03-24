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

#include "SalomeContainerTools.hxx"
#include "SALOME_LifeCycleCORBA.hxx"
#include "SALOME_NamingService.hxx"
#include "SALOME_ResourcesManager.hxx"
#include "SALOME_ContainerManager.hxx"
#include "Container.hxx"
#include "AutoLocker.hxx"

#include "YacsTrace.hxx"
#include "Proc.hxx"
#include "ServiceNode.hxx"
#include "ComponentInstance.hxx"
#include "SalomeContainerHelper.hxx"
#include "RuntimeSALOME.hxx"
#include "Exception.hxx"

#include <sstream>

#ifdef WIN32
#include <process.h>
#define getpid _getpid
#endif

using namespace YACS::ENGINE;

SalomeContainerTools::SalomeContainerTools()
{
  /* Init ContainerParameters */
  SALOME_LifeCycleCORBA::preSet(_params);
}

SalomeContainerTools::SalomeContainerTools(const SalomeContainerTools& other):_params(other._params),_propertyMap(other._propertyMap)
{
}

void SalomeContainerTools::clearProperties()
{
  _propertyMap.clear();
  _params=Engines::ContainerParameters();
}

std::string SalomeContainerTools::getProperty(const std::string& name) const
{
  std::map<std::string,std::string>::const_iterator it(_propertyMap.find(name));
  if(it!=_propertyMap.end())
    return (*it).second;
  else
    return std::string();
}

void SalomeContainerTools::setProperty(const std::string& name, const std::string& value)
{
  //DEBTRACE("SalomeContainer::setProperty : " << name << " ; " << value);
  // Container Part
  if (name == "container_name")
    _params.container_name = CORBA::string_dup(value.c_str());
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
  _propertyMap[name]=value;
}

void SalomeContainerTools::addToComponentList(const std::string& name)
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

void SalomeContainerTools::addToResourceList(const std::string& name)
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

std::string SalomeContainerTools::getContainerName() const
{
  return std::string(_params.container_name);
}

void SalomeContainerTools::setContainerName(const std::string& name)
{
  SetContainerNameOf(_params,name);
}

std::string SalomeContainerTools::getNotNullContainerName(const Container *contPtr, const Task *askingNode, bool& isEmpty) const
{
  isEmpty=true;
  std::string name(_params.container_name);
  if(!name.empty())
    {
      isEmpty=false;
      return name;
    }
  else
    {
      //give a almost unique name to the container : Pid_Name_Addr
      std::ostringstream stream;
      stream << getpid();
      stream << "_";
      stream << contPtr->getName();
      stream << "_";
      stream << contPtr->getDiscreminantStrOfThis(askingNode);
      return stream.str();
    }
}

std::string SalomeContainerTools::getHostName() const
{
  return std::string(_params.resource_params.hostname);
}

void SalomeContainerTools::SetContainerNameOf(Engines::ContainerParameters& params, const std::string& name)
{
  params.container_name=CORBA::string_dup(name.c_str());
}

std::map<std::string,std::string> SalomeContainerTools::getResourceProperties(const std::string& name) const
{
  std::map<std::string,std::string> properties;

  YACS::ENGINE::RuntimeSALOME* runTime = YACS::ENGINE::getSALOMERuntime();
  CORBA::ORB_ptr orb = runTime->getOrb();
  if (!orb) return properties;
  SALOME_NamingService namingService(orb);
  SALOME_LifeCycleCORBA lcc(&namingService);
  CORBA::Object_var obj = namingService.Resolve(SALOME_ResourcesManager::_ResourcesManagerNameInNS);
  if (CORBA::is_nil(obj))
    return properties;
  Engines::ResourcesManager_var resManager = Engines::ResourcesManager::_narrow(obj);
  if (CORBA::is_nil(resManager))
    return properties;

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

/*!
 * \param [in] compoNames
 * \param [in,out] shutdownLevel
 */
void SalomeContainerTools::Start(const std::vector<std::string>& compoNames, SalomeContainerHelper *schelp, SalomeContainerTools& sct, int& shutdownLevel, const Container *cont, const Task *askingNode)
{
  CORBA::ORB_ptr orb(getSALOMERuntime()->getOrb());
  SALOME_NamingService ns;
  try
  {
      ns.init_orb(orb);
  }
  catch(SALOME_Exception& e)
  {
      throw Exception("SalomeContainer::start : Unable to contact the SALOME Naming Service");
  }
  CORBA::Object_var obj(ns.Resolve(SALOME_ContainerManager::_ContainerManagerNameInNS));
  Engines::ContainerManager_var contManager(Engines::ContainerManager::_narrow(obj));

  bool isEmptyName;
  std::string str(sct.getNotNullContainerName(cont,askingNode,isEmptyName));
  DEBTRACE("SalomeContainer::start " << str <<";"<< _sct.getHostName() <<";"<<_type);

  // Finalize parameters with components found in the container

  for(std::vector<std::string>::const_iterator iter=compoNames.begin();iter!=compoNames.end();iter++)
    sct.addToComponentList(*iter);

  Engines::ContainerParameters myparams(sct.getParameters());
  {
    std::string dftLauchMode(schelp->getDftLaunchMode());
    myparams.mode=CORBA::string_dup(dftLauchMode.c_str());
  }

  //If a container_name is given try to find an already existing container in naming service
  //If not found start a new container with the given parameters
  if (dynamic_cast<SalomeContainerMonoHelper *>(schelp) && !isEmptyName)
    {
      myparams.mode=CORBA::string_dup("getorstart");
    }

  if (isEmptyName)
    {
      shutdownLevel=1;
    }
  //sct.setContainerName(str);
  SetContainerNameOf(myparams,str);
  Engines::Container_var trueCont(Engines::Container::_nil());
  if(!isEmptyName && shutdownLevel==999)
    {
      //Make this only the first time start is called (_shutdownLevel==999)
      //If the container is named, first try to get an existing container
      //If there is an existing container use it and set the shutdown level to 3
      //If there is no existing container, try to launch a new one and set the shutdown level to 2
      myparams.mode="get";
      try
      {
          trueCont=contManager->GiveContainer(myparams);
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

      if(!CORBA::is_nil(trueCont))
        {
          shutdownLevel=3;
          DEBTRACE( "container found: " << str << " " << _shutdownLevel );
        }
      else
        {
          shutdownLevel=2;
          myparams.mode="start";
          DEBTRACE( "container not found: " << str << " " << _shutdownLevel);
        }
    }

  if(CORBA::is_nil(trueCont))
    try
  {
        // --- GiveContainer is used in batch mode to retreive launched containers,
        //     and is equivalent to StartContainer when not in batch.
        trueCont=contManager->GiveContainer(myparams);
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

  if(CORBA::is_nil(trueCont))
    throw Exception("SalomeContainer::start : Unable to launch container in Salome. Check your CatalogResources.xml file");

  schelp->setContainer(askingNode,trueCont);

  CORBA::String_var containerName(trueCont->name()),hostName(trueCont->getHostName());
  std::cerr << "SalomeContainer launched : " << containerName << " " << hostName << " " << trueCont->getPID() << std::endl;
}

CORBA::Object_ptr SalomeContainerTools::LoadComponent(SalomeContainerHelper *launchModeType, Container *cont, Task *askingNode)
{
  DEBTRACE("SalomeContainer::loadComponent ");
  const ComponentInstance *inst(askingNode?askingNode->getComponent():0);
  {
    YACS::BASES::AutoLocker<Container> alck(cont);//To be sure
    if(!cont->isAlreadyStarted(askingNode))
      cont->start(askingNode);
  }
  if(!inst)
    throw Exception("SalomeContainerTools::LoadComponent : no instance of component in the task requesting for a load of its component !");
  CORBA::Object_ptr objComponent=CORBA::Object::_nil();
  {
    YACS::BASES::AutoLocker<Container> alck(cont);//To be sure
    std::string compoName(inst->getCompoName());
    Engines::Container_var container(launchModeType->getContainer(askingNode));

    char *reason;
    bool isLoadable(container->load_component_Library(compoName.c_str(), reason));
    if(isLoadable)
      objComponent=CreateComponentInstance(cont,container,inst);
  }
  return objComponent;
}

CORBA::Object_ptr SalomeContainerTools::CreateComponentInstance(Container *cont, Engines::Container_ptr contPtr, const ComponentInstance *inst)
{
  if(!inst)
    throw Exception("SalomeContainerTools::CreateComponentInstance : no instance of component in the task requesting for a load of its component !");
  char *reason(0);
  std::string compoName(inst->getCompoName());
  CORBA::Object_ptr objComponent=CORBA::Object::_nil();
  int studyid(1);
  Proc* p(cont->getProc());
  if(p)
    {
      std::string value(p->getProperty("DefaultStudyID"));
      if(!value.empty())
        studyid= atoi(value.c_str());
    }
  // prepare component instance properties
  Engines::FieldsDict_var env(new Engines::FieldsDict);
  std::map<std::string, std::string> properties(inst->getProperties());
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

  objComponent=contPtr->create_component_instance_env(compoName.c_str(), studyid, env, reason);
  if(CORBA::is_nil(objComponent))
    {
      std::string text="Error while trying to create a new component: component '"+ compoName;
      text=text+"' is not installed or it's a wrong name";
      text += '\n';
      text += reason;
      CORBA::string_free(reason);
      throw Exception(text);
    }
  return objComponent;
}

std::string SalomeContainerTools::GetPlacementId(const SalomeContainerHelper *launchModeType, const Container *cont, const Task *askingNode)
{
  if(cont->isAlreadyStarted(askingNode))
    {
      Engines::Container_var container(launchModeType->getContainer(askingNode));
      const char *what="/";
      CORBA::String_var corbaStr(container->name());
      std::string ret(corbaStr);

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

std::string SalomeContainerTools::GetFullPlacementId(const SalomeContainerHelper *launchModeType, const Container *cont, const Task *askingNode)
{
  if(cont->isAlreadyStarted(askingNode))
    {
      Engines::Container_var container(launchModeType->getContainer(askingNode));
      try
      {
          CORBA::String_var corbaStr(container->name());
          std::string ret(corbaStr);
          return ret;
      }
      catch(...)
      {
          return "Unknown_placement";
      }
    }
  else
    return "Not_placed_yet";
}
