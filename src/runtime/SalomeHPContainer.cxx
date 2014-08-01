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

#include "SalomeHPContainer.hxx"
#include "SalomeComponent.hxx"

#include <algorithm>

using namespace YACS::ENGINE;

const char SalomeHPContainer::KIND[]="HPSalome";

SalomeHPContainer::SalomeHPContainer():_shutdownLevel(999)
{
}

SalomeHPContainer::SalomeHPContainer(const SalomeHPContainer& other):_componentNames(other._componentNames),_shutdownLevel(999),_sct(other._sct)
{
}

void SalomeHPContainer::setSizeOfPool(int sz)
{
  _launchModeType.resize(sz);
}

std::size_t SalomeHPContainer::getNumberOfFreePlace() const
{
  return _launchModeType.getNumberOfFreePlace();
}

void SalomeHPContainer::allocateFor(const std::vector<const Task *>& nodes)
{
  _launchModeType.allocateFor(nodes);
}

void SalomeHPContainer::release(Task *node)
{
  _launchModeType.release(node);
}

SalomeHPContainer::~SalomeHPContainer()
{
}

void SalomeHPContainer::lock()
{
  _mutex.lock();
}

void SalomeHPContainer::unLock()
{
  _mutex.unlock();
}

bool SalomeHPContainer::isAlreadyStarted(const Task *askingNode) const
{
  const SalomeContainerMonoHelper& helper(_launchModeType.getHelperOfTask(askingNode));
  return helper.isAlreadyStarted(askingNode);
}

void SalomeHPContainer::start(const Task *askingNode) throw(Exception)
{
  SalomeContainerMonoHelper& helper(_launchModeType.getHelperOfTask(askingNode));
  SalomeContainerTools::Start(_componentNames,&helper,_sct,_shutdownLevel,this,askingNode);
}

void SalomeHPContainer::shutdown(int level)
{
  if(level < _shutdownLevel)
      return;
  _shutdownLevel=999;
  for(std::size_t i=0;_launchModeType.size();i++)
    {
      SalomeContainerMonoHelper& helper(_launchModeType.at(i));
      helper.shutdown();
    }
}

std::string SalomeHPContainer::getPlacementId(const Task *askingNode) const
{
  const SalomeContainerMonoHelper& helper(_launchModeType.getHelperOfTask(askingNode));
  return SalomeContainerTools::GetPlacementId(&helper,this,askingNode);
}

std::string SalomeHPContainer::getFullPlacementId(const Task *askingNode) const
{
  const SalomeContainerMonoHelper& helper(_launchModeType.getHelperOfTask(askingNode));
  return SalomeContainerTools::GetFullPlacementId(&helper,this,askingNode);
}

/*!
 * It is not a bug here ! clone for homogeneous container is not supposed to be copied !
 */
Container *SalomeHPContainer::clone() const
{
  incrRef();
  return const_cast<SalomeHPContainer*>(this);
}

Container *SalomeHPContainer::cloneAlways() const
{
  return new SalomeHPContainer(*this);
}

void SalomeHPContainer::setProperty(const std::string& name,const std::string& value)
{
  _sct.setProperty(name,value);
}

std::string SalomeHPContainer::getProperty(const std::string& name) const
{
  return _sct.getProperty(name);
}

void SalomeHPContainer::clearProperties()
{
  _sct.clearProperties();
}

std::map<std::string,std::string> SalomeHPContainer::getProperties() const
{
  return _sct.getProperties();
}

std::map<std::string,std::string> SalomeHPContainer::getResourceProperties(const std::string& name) const
{
  return _sct.getResourceProperties(name);
}

void SalomeHPContainer::checkCapabilityToDealWith(const ComponentInstance *inst) const throw(YACS::Exception)
{
  if(inst->getKind()!=SalomeComponent::KIND)
    throw Exception("SalomeHPContainer::checkCapabilityToDealWith : SalomeContainer is not able to deal with this type of ComponentInstance.");
}
