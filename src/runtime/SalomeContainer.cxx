//#define REFCNT
#ifdef REFCNT
#define private public
#define protected public
#include <omniORB4/CORBA.h>
#include <omniORB4/internal/typecode.h>
#endif

#include "RuntimeSALOME.hxx"
#include "SalomeContainer.hxx"
#include "SalomeComponent.hxx"

#include "SALOME_NamingService.hxx"
#include "SALOME_LifeCycleCORBA.hxx"
#include "SALOME_ContainerManager.hxx"
#include "Basics_Utils.hxx"

#include <sstream>
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

SalomeContainer::SalomeContainer():_trueCont(Engines::Container::_nil())
{
  /* Init MachinesParameters */
  _params.container_name = "";
  _params.hostname = "";
  _params.OS = "";
  _params.mem_mb = 0;
  _params.cpu_clock = 0;
  _params.nb_proc_per_node = 0;
  _params.nb_node = 0;
  _params.isMPI = false;
  _params.parallelLib = "";
  _params.nb_component_nodes = 0;
}

SalomeContainer::SalomeContainer(const SalomeContainer& other):Container(other),_trueCont(Engines::Container::_nil())
{
  _params.container_name = CORBA::string_dup(other._params.container_name);
  _params.hostname = CORBA::string_dup(other._params.hostname);
  _params.OS = CORBA::string_dup(other._params.OS);
  _params.mem_mb = other._params.mem_mb;
  _params.cpu_clock = other._params.cpu_clock;
  _params.nb_proc_per_node = other._params.nb_proc_per_node;
  _params.nb_node = other._params.nb_node;
  _params.isMPI = other._params.isMPI;
  _params.parallelLib = CORBA::string_dup(other._params.parallelLib);
  _params.nb_component_nodes = other._params.nb_component_nodes;
  _params.workingdir= CORBA::string_dup(other._params.workingdir);
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

bool SalomeContainer::isAlreadyStarted() const
{
  if(CORBA::is_nil(_trueCont))
    return false;
  else
    return true;
}

void SalomeContainer::start() throw (Exception)
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
  DEBTRACE("SalomeContainer::start " << str);
  //If a container_name is given try to find an already existing container in naming service
  //If not found start a new container with the given parameters
  if (str != "")
    {
      std::string machine(_params.hostname);
      if(machine == "" || machine == "localhost")
        machine=Kernel_Utils::GetHostname();
      std::string ContainerNameInNS=ns.BuildContainerNameForNS(_params,machine.c_str());
      obj=ns.Resolve(ContainerNameInNS.c_str());
      if(!CORBA::is_nil(obj))
        {
          std::cerr << "Container already exists: " << ContainerNameInNS << std::endl;
          _trueCont=Engines::Container::_narrow(obj);
          return;
        }
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
      _params.container_name=CORBA::string_dup(stream.str().c_str());
    }
  Engines::CompoList compolist;
  compolist.length(_componentNames.size());
  std::vector<std::string>::iterator iter;
  for(CORBA::ULong i=0; i < _componentNames.size();i++)
    {
      compolist[i]=CORBA::string_dup(_componentNames[i].c_str());
    }

  try
    { 
      // --- GiveContainer is used in batch mode to retreive launched containers,
      //     and is equivalent to StartContainer when not in batch.
      std::string policy=getProperty("policy");
      if(policy=="best")
        _trueCont=contManager->GiveContainer(_params,Engines::P_BEST,compolist);
      else if(policy=="first")
        _trueCont=contManager->GiveContainer(_params,Engines::P_FIRST,compolist);
      else
        _trueCont=contManager->GiveContainer(_params,Engines::P_CYCL,compolist);
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

  CORBA::String_var containerName=_trueCont->name();
  CORBA::String_var hostName=_trueCont->getHostName();
  std::cerr << "SalomeContainer launched : " << containerName << " " << hostName << " " << _trueCont->getPID() << std::endl;

#ifdef REFCNT
    DEBTRACE(_trueCont->_PR_getobj()->pd_refCount );
#endif
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

std::string SalomeContainer::getPlacementId() const
{
  if(isAlreadyStarted())
    {
      const char *what="/";
      char *corbaStr=_trueCont->name();
      string ret(corbaStr);
      CORBA::string_free(corbaStr);
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

void SalomeContainer::checkCapabilityToDealWith(const ComponentInstance *inst) const throw (Exception)
{
  if(inst->getKind()!=SalomeComponent::KIND)
    throw Exception("SalomeContainer::checkCapabilityToDealWith : SalomeContainer is not able to deal with this type of ComponentInstance.");
}

void SalomeContainer::setProperty(const std::string& name, const std::string& value)
{
  DEBTRACE("SalomeContainer::setProperty : " << name << " ; " << value);
  
  if (name == "container_name")
    _params.container_name = CORBA::string_dup(value.c_str());
  else if (name == "hostname")
    _params.hostname = CORBA::string_dup(value.c_str());
  else if (name == "OS")
    _params.OS = CORBA::string_dup(value.c_str());
  else if (name == "parallelLib")
    _params.parallelLib = CORBA::string_dup(value.c_str());
  else if (name == "workingdir")
    _params.workingdir = CORBA::string_dup(value.c_str());
  else if (name == "isMPI")
    {
      if (value == "true")
        _params.isMPI = true;
      else if (value == "false")
        _params.isMPI = false;
      else 
        throw Exception("SalomeContainer::SetProperty : params.isMPI value not correct : " + value);
    }
  else if (name == "mem_mb")
    {
      std::istringstream iss(value);
      if (!(iss >> _params.mem_mb))
        throw Exception("salomecontainer::setproperty : params.mem_mb value not correct : " + value);
    }
  else if (name == "cpu_clock")
    {
      std::istringstream iss(value);
      if (!(iss >> _params.cpu_clock))
        throw Exception("salomecontainer::setproperty : params.cpu_clock value not correct : " + value);
    }
  else if (name == "nb_proc_per_node")
    {
      std::istringstream iss(value);
      if (!(iss >> _params.nb_proc_per_node))
        throw Exception("salomecontainer::setproperty : params.nb_proc_per_node value not correct : " + value);
    }
  else if (name == "nb_node")
    {
      std::istringstream iss(value);
      if (!(iss >> _params.nb_node))
        throw Exception("salomecontainer::setproperty : params.nb_node value not correct : " + value);
    }
  else if (name == "nb_component_nodes")
    {
      std::istringstream iss(value);
      if (!(iss >> _params.nb_component_nodes))
        throw Exception("salomecontainer::setproperty : params.nb_component_nodes value not correct : " + value);
    }
  Container::setProperty(name, value);
}

bool SalomeContainer::isAPaCOContainer() const
{
  bool result = false;
  string parallelLib(_params.parallelLib);
  if (parallelLib != "")
    result = true;
  return result;
}

void SalomeContainer::addComponentName(std::string name)
{
  _componentNames.push_back(name);
}
