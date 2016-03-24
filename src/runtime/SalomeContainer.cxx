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
#include "ServiceNode.hxx"
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

const char SalomeContainer::KIND[]="Salome";

const char SalomeContainer::TYPE_PROPERTY_STR[]="type";

SalomeContainer::SalomeContainer():_launchModeType(new SalomeContainerMonoHelper),_shutdownLevel(999)
{
}

SalomeContainer::SalomeContainer(const SalomeContainer& other)
: Container(other),_componentNames(other._componentNames),
  _launchModeType(other._launchModeType->deepCpyOnlyStaticInfo()),
  _shutdownLevel(other._shutdownLevel),
  _sct(other._sct)
{
}

SalomeContainer::SalomeContainer(const Container& other, const SalomeContainerTools& sct, const SalomeContainerHelper *lmt,
                                 const std::vector<std::string>& componentNames, int shutdownLev):Container(other),_componentNames(componentNames),
                                     _launchModeType(const_cast<SalomeContainerHelper *>(lmt)),_shutdownLevel(shutdownLev),_sct(sct)
{
  if(lmt)
    lmt->incrRef();
}

SalomeContainer::~SalomeContainer()
{
  _launchModeType->decrRef();
}

void SalomeContainer::lock()
{
  _mutex.lock();
}

void SalomeContainer::unLock()
{
  _mutex.unLock();
}

std::string SalomeContainer::getKind() const
{
  return KIND;
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
  if (name == AOC_ENTRY)
    {
      std::istringstream iss(value);
      int val;
      iss >> val;
      setAttachOnCloningStatus((bool)val);
    }
  else if (name == TYPE_PROPERTY_STR)
    {
      if (value == SalomeContainerMonoHelper::TYPE_NAME)
        {
          _launchModeType->decrRef();
          _launchModeType=new SalomeContainerMonoHelper;
        }
      else if (value == SalomeContainerMultiHelper::TYPE_NAME)
        {
          _launchModeType->decrRef();
          _launchModeType=new SalomeContainerMultiHelper;
        }
      else
        throw Exception("SalomeContainer::setProperty : type value is not correct (mono or multi): " + value);
    }
  _sct.setProperty(name,value);
}

std::string SalomeContainer::getProperty(const std::string& name) const
{
  if (name == TYPE_PROPERTY_STR)
    return _launchModeType->getType();
  if (name==AOC_ENTRY)
    {
      int reti(_isAttachedOnCloning);
      std::ostringstream oss; oss << reti;
      return oss.str();
    }
  return _sct.getProperty(name);
}

void SalomeContainer::clearProperties()
{
  _sct.clearProperties();
}

void SalomeContainer::addComponentName(const std::string& name)
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
CORBA::Object_ptr SalomeContainer::loadComponent(Task *askingNode)
{
  return SalomeContainerTools::LoadComponent(_launchModeType,this,askingNode);
}

//! Get the container placement id for a component instance
/*!
 * \param inst the component instance
 * \return the placement id
 */
std::string SalomeContainer::getPlacementId(const Task *askingNode) const
{
  return SalomeContainerTools::GetPlacementId(_launchModeType,this,askingNode);
}

//! Get the container full path for a component instance
/*!
 * \param inst the component instance
 * \return the full placement id
 */
std::string SalomeContainer::getFullPlacementId(const Task *askingNode) const
{
  return SalomeContainerTools::GetFullPlacementId(_launchModeType,this,askingNode);
}

//! Check if the component instance container is already started
/*!
 * \param inst the component instance
 * \return true, if the container is already started, else false
 */
bool SalomeContainer::isAlreadyStarted(const Task *askingNode) const
{
  return _launchModeType->isAlreadyStarted(askingNode);
}

Engines::Container_ptr SalomeContainer::getContainerPtr(const Task *askingNode) const
{
  return Engines::Container::_duplicate(_launchModeType->getContainer(askingNode));
}

//! Start a salome container (true salome container not yacs one) with given ContainerParameters (_params)
/*!
 * \param inst the component instance
 */
void SalomeContainer::start(const Task *askingNode) throw(YACS::Exception)
{
  SalomeContainerTools::Start(_componentNames,_launchModeType,_sct,_shutdownLevel,this,askingNode);
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
