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

#ifdef WIN32
#include <process.h>
#define getpid _getpid
#endif

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

SalomeContainer::SalomeContainer():_launchMode("start"),_launchModeType(new SalomeContainerMonoHelper),_shutdownLevel(999)
{
}

SalomeContainer::SalomeContainer(const SalomeContainer& other)
: Container(other),
  _launchMode(other._launchMode),
  _launchModeType(other._launchModeType->deepCpyOnlyStaticInfo()),
  _shutdownLevel(other._shutdownLevel),
  _sct(other._sct)
{
}

SalomeContainer::~SalomeContainer()
{
  delete _launchModeType;
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

Container *SalomeContainer::cloneAlways() const
{
  return new SalomeContainer(*this);
}

void SalomeContainer::checkCapabilityToDealWith(const ComponentInstance *inst) const throw(YACS::Exception)
{
  if(inst->getKind()!=SalomeComponent::KIND)
    throw Exception("SalomeContainer::checkCapabilityToDealWith : SalomeContainer is not able to deal with this type of ComponentInstance.");
}

void SalomeContainer::setProperty(const std::string& name, const std::string& value)
{
  if (name == "type")
    {
      if (value == SalomeContainerMonoHelper::TYPE_NAME)
        {
          delete _launchModeType;
          _launchModeType=new SalomeContainerMonoHelper;
        }
      else if (value == SalomeContainerMultiHelper::TYPE_NAME)
        {
          delete _launchModeType;
          _launchModeType=new SalomeContainerMultiHelper;
        }
      else
        throw Exception("SalomeContainer::setProperty : type value is not correct (mono or multi): " + value);
      return ;
    }
  _sct.setProperty(name,value);
}

std::string SalomeContainer::getProperty(const std::string& name) const
{
  return _sct.getProperty(name);
}

void SalomeContainer::clearProperties()
{
  _sct.clearProperties();
}

void SalomeContainer::addComponentName(std::string name)
{
  _componentNames.push_back(name);
}

void SalomeContainer::addToResourceList(const std::string& name)
{
  _sct.addToResourceList(name);
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
  Engines::Container_var container(_launchModeType->getContainer(inst));

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
      Engines::Container_var container(_launchModeType->getContainer(inst));
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

//! Get the container full path for a component instance
/*!
 * \param inst the component instance
 * \return the full placement id
 */
std::string SalomeContainer::getFullPlacementId(const ComponentInstance *inst) const
{

  if(isAlreadyStarted(inst))
    {
      Engines::Container_var container(_launchModeType->getContainer(inst));
      try
        {
          CORBA::String_var corbaStr=container->name();
          string ret(corbaStr);
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

//! Check if the component instance container is already started
/*!
 * \param inst the component instance
 * \return true, if the container is already started, else false
 */
bool SalomeContainer::isAlreadyStarted(const ComponentInstance *inst) const
{
  return _launchModeType->isAlreadyStarted(inst);
}

Engines::Container_ptr SalomeContainer::getContainerPtr(const ComponentInstance *inst) const
{
  return Engines::Container::_duplicate(_launchModeType->getContainer(inst));
}

//! Start a salome container (true salome container not yacs one) with given ContainerParameters (_params)
/*!
 * \param inst the component instance
 */
void SalomeContainer::start(const ComponentInstance *inst) throw(YACS::Exception)
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

  std::string str(_sct.getContainerName());
  DEBTRACE("SalomeContainer::start " << str <<";"<< _sct.getHostName() <<";"<<_type);

  // Finalize parameters with components found in the container
  std::vector<std::string>::iterator iter;
  for(CORBA::ULong i=0; i < _componentNames.size();i++)
    _sct.addToComponentList(_componentNames[i]);
  Engines::ContainerParameters myparams(_sct.getParameters());

  bool namedContainer=false;
  if(str != "")
    namedContainer=true;

  //If a container_name is given try to find an already existing container in naming service
  //If not found start a new container with the given parameters
  if (dynamic_cast<SalomeContainerMonoHelper *>(_launchModeType) && str != "")
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

  Engines::Container_var trueCont(Engines::Container::_nil());
  if(namedContainer && _shutdownLevel==999)
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

  _launchModeType->setContainer(inst,trueCont);

  CORBA::String_var containerName(trueCont->name()),hostName(trueCont->getHostName());
  std::cerr << "SalomeContainer launched : " << containerName << " " << hostName << " " << trueCont->getPID() << std::endl;
}

void SalomeContainer::shutdown(int level)
{
  DEBTRACE("SalomeContainer::shutdown: " << _name << "," << level << "," << _shutdownLevel);
  if(level < _shutdownLevel)
    return;

  _shutdownLevel=999;
  //shutdown the SALOME containers
  _launchModeType->shutdown();
}

std::map<std::string,std::string> SalomeContainer::getResourceProperties(const std::string& name) const
{
  return _sct.getResourceProperties(name);
}

std::map<std::string,std::string> SalomeContainer::getProperties() const
{
  return _sct.getProperties();
}
