// Copyright (C) 2006-2014  CEA/DEN, EDF R&D
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

#include "RuntimeSALOME.hxx"
#include "Exception.hxx"

#include <sstream>

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

std::string SalomeContainerTools::getHostName() const
{
  return std::string(_params.resource_params.hostname);
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
