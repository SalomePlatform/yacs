// Copyright (C) 2006-2019  CEA/DEN, EDF R&D
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
#include "SalomeHPComponent.hxx"
#include "SalomeContainerTmpForHP.hxx"
#include "AutoLocker.hxx"
#include "AutoRefCnt.hxx"

#include <algorithm>

using namespace YACS::ENGINE;

const char SalomeHPContainer::KIND[]="HPSalome";

SalomeHPContainerBase::SalomeHPContainerBase(SalomeHPContainerVectOfHelper *resShared):_launchModeType(resShared),_shutdownLevel(999)
{
}

SalomeHPContainerBase::SalomeHPContainerBase(const SalomeHPContainerBase& other):_shutdownLevel(999),_launchModeType(new SalomeHPContainerVectOfHelper),_initScript(other._initScript)
{
}

SalomeHPContainer *SalomeHPContainerBase::getTheBoss()
{
  HomogeneousPoolContainer *ret(this);
  while(ret->getDirectFather())
    ret=ret->getDirectFather();
  SalomeHPContainer *retC(dynamic_cast<SalomeHPContainer *>(ret));
  if(!retC)
    throw Exception("SalomeHPContainerBase::getTheBoss : unexpected type of object !");
  return retC;
}

const SalomeHPContainer *SalomeHPContainerBase::getTheBoss() const
{
  const HomogeneousPoolContainer *ret(this);
  while(ret->getDirectFather())
    ret=ret->getDirectFather();
  const SalomeHPContainer *retC(dynamic_cast<const SalomeHPContainer *>(ret));
  if(!retC)
    throw Exception("SalomeHPContainerBase::getTheBoss : unexpected type of object !");
  return retC;
}

void SalomeHPContainerBase::startInternal(const Task *askingNode, SalomeContainerToolsBase& sct, const std::vector<std::string>& compoNames)
{
  SalomeContainerMonoHelper *helper(_launchModeType->getHelperOfTaskThreadSafe(askingNode));
  SalomeContainerTools::Start(compoNames,helper,sct,_shutdownLevel,this,askingNode);
}

void SalomeHPContainerBase::shutdown(int level)
{
  if(level < _shutdownLevel)
      return;
  _shutdownLevel=999;
  _launchModeType->shutdown();
}

SalomeHPContainerBase::SalomeHPContainerBase(SalomeHPContainerVectOfHelper *resShared, bool isRefEaten):_launchModeType(resShared)
{
  if(!isRefEaten)
    if(_launchModeType.isNotNull())
      _launchModeType->incrRef();
}

bool SalomeHPContainerBase::isAlreadyStarted(const Task *askingNode) const
{
  const SalomeContainerMonoHelper *helper(_launchModeType->getHelperOfTaskThreadSafe(askingNode));
  return helper->isAlreadyStarted(askingNode);
}

void SalomeHPContainerBase::release(const Task *node)
{
  _launchModeType->release(node);
}

void SalomeHPContainerBase::lock()
{
  _launchModeType->lock();
}

void SalomeHPContainerBase::unLock()
{
  _launchModeType->unLock();
}

void SalomeHPContainerBase::setSizeOfPool(int sz)
{
  _launchModeType->resize(sz);
}

int SalomeHPContainerBase::getSizeOfPool() const
{
  return _launchModeType->size();
}

void SalomeHPContainerBase::setProperty(const std::string& name,const std::string& value)
{
  if(name==AOC_ENTRY)//no sense to set it ! It is always true ! ignore it !
    return ;
  else if(name==SIZE_OF_POOL_KEY)
    {
      std::istringstream iss(value);
      int val(0);
      iss >> val;
      setSizeOfPool(val);
    }
  else if(name==INITIALIZE_SCRIPT_KEY)
    {
      _initScript=value;
    }
  else
    getTheBoss()->getContainerInfo().setProperty(name,value);
}

std::string SalomeHPContainerBase::getProperty(const std::string& name) const
{
  if(name==AOC_ENTRY)
    {
      return std::string("1");
    }
  else if(name==SIZE_OF_POOL_KEY)
    {
      std::ostringstream oss; oss << getSizeOfPool();
      return oss.str();
    }
  else if(name==INITIALIZE_SCRIPT_KEY)
    {
      return _initScript;
    }
  else
    return getTheBoss()->getContainerInfo().getProperty(name);
}

std::map<std::string,std::string> SalomeHPContainerBase::getProperties() const
{
  std::map<std::string,std::string> ret(getTheBoss()->getContainerInfo().getProperties());
  std::ostringstream oss; oss << getSizeOfPool();
  ret[SIZE_OF_POOL_KEY]=oss.str();
  if(!_initScript.empty())
    ret[INITIALIZE_SCRIPT_KEY]=_initScript;
  return ret;
}

void SalomeHPContainerBase::clearProperties()
{
  _initScript.clear();
  getTheBoss()->getContainerInfo().clearProperties();
}

std::string SalomeHPContainerBase::getPlacementId(const Task *askingNode) const
{
  const SalomeContainerMonoHelper *helper(0);
  {
    YACS::BASES::AutoLocker<Container> alckCont(const_cast<SalomeHPContainerBase *>(this));
    helper=_launchModeType->getHelperOfTask(askingNode);
  }
  return SalomeContainerTools::GetPlacementId(helper,this,askingNode);
}

std::string SalomeHPContainerBase::getFullPlacementId(const Task *askingNode) const
{
  const SalomeContainerMonoHelper *helper(0);
  {
    YACS::BASES::AutoLocker<Container> alckCont(const_cast<SalomeHPContainerBase *>(this));
    helper=_launchModeType->getHelperOfTask(askingNode);
  }
  return SalomeContainerTools::GetFullPlacementId(helper,this,askingNode);
}

std::map<std::string,std::string> SalomeHPContainerBase::getResourceProperties(const std::string& name) const
{
  return getTheBoss()->getResourceProperties(name);
}

void SalomeHPContainerBase::addComponentName(const std::string& name)
{
  getTheBoss()->addComponentNameSpe(name);
}

void SalomeHPContainerBase::checkCapabilityToDealWith(const ComponentInstance *inst) const throw(YACS::Exception)
{
  getTheBoss()->checkCapabilityToDealWith(inst);
}

YACS::BASES::AutoRefCnt<HomogeneousPoolContainer> SalomeHPContainerBase::decorate(YACS::BASES::AutoConstRefCnt<PartDefinition> pd)
{
  YACS::BASES::AutoRefCnt<HomogeneousPoolContainer> ret(new SalomeHPContainerShared(pd,_launchModeType,this));
  return ret;
}

Engines::Container_var SalomeHPContainerBase::getContainerPtr(const Task *askingNode) const
{
  const SalomeContainerMonoHelper *helper(0);
  {
    YACS::BASES::AutoLocker<SalomeHPContainerBase> alck(const_cast<SalomeHPContainerBase *>(this));
    helper=_launchModeType->getHelperOfTask(askingNode);
  }
  return helper->getContainer(NULL);
}

std::vector<std::string> SalomeHPContainerBase::getKernelContainerNames() const
{
  return _launchModeType->getKernelContainerNames();
}

////////////////

SalomeHPContainer::SalomeHPContainer():SalomeHPContainerBase(new SalomeHPContainerVectOfHelper)
{
}

SalomeHPContainer::SalomeHPContainer(const SalomeHPContainer& other):SalomeHPContainerBase(other),_sct(other._sct),_componentNames(other._componentNames)
{
}

std::size_t SalomeHPContainer::getNumberOfFreePlace() const
{
  return _launchModeType->getNumberOfFreePlace();
}

void SalomeHPContainer::allocateFor(const std::vector<const Task *>& nodes)
{
  _launchModeType->allocateFor(nodes);
}

SalomeHPContainer::~SalomeHPContainer()
{
}

std::string SalomeHPContainer::getKind() const
{
  return KIND;
}

std::string SalomeHPContainer::getDiscreminantStrOfThis(const Task *askingNode) const
{
  YACS::BASES::AutoCppPtr<SalomeContainerTmpForHP> tmpCont(SalomeContainerTmpForHP::BuildFrom(this,askingNode));
  return tmpCont->getDiscreminantStrOfThis(askingNode);
}

void SalomeHPContainer::start(const Task *askingNode) throw(YACS::Exception)
{
  startInternal(askingNode,_sct,_componentNames);
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

int SalomeHPContainer::getNumberOfCoresPerWorker() const
{
  return _sct.getNumberOfCoresPerWorker();
}

std::map<std::string,std::string> SalomeHPContainer::getResourceProperties(const std::string& name) const
{
  return _sct.getResourceProperties(name);
}

void SalomeHPContainer::addComponentNameSpe(const std::string& name)
{
  _componentNames.push_back(name);
}

void SalomeHPContainer::checkCapabilityToDealWithSpe(const ComponentInstance *inst) const throw(YACS::Exception)
{
  if(inst->getKind()!=SalomeHPComponent::KIND)
    throw Exception("SalomeHPContainer::checkCapabilityToDealWithSpe : SalomeContainer is not able to deal with this type of ComponentInstance.");
}

void SalomeHPContainer::forYourTestsOnly(ForTestOmlyHPContCls *data) const
{
  data->setContainerType("HPContainer");
}

//////////////////////////////////

SalomeHPContainerShared::SalomeHPContainerShared(YACS::BASES::AutoConstRefCnt<PartDefinition> pd, SalomeHPContainerVectOfHelper *resShared, SalomeHPContainerBase *directFather):SalomeHPContainerBase(resShared,false),_pd(pd)
{
  if(!directFather)
    throw Exception("SalomeHPContainerShared : NULL pointer not allowed !");
  _directFather.takeRef(directFather);
}

std::string SalomeHPContainerShared::getKind() const
{
  return SalomeHPContainer::KIND;
}

void SalomeHPContainerShared::prepareMaskForExecution() const
{
  _idsOfKernelContainers=_pd->computeWorkerIdsCovered(getNumberOfCoresPerWorker());
}

/*!
 * It is not a bug here ! clone for homogeneous container is not supposed to be copied !
 */
Container *SalomeHPContainerShared::clone() const
{
  incrRef();
  return const_cast<SalomeHPContainerShared*>(this);
}

Container *SalomeHPContainerShared::cloneAlways() const
{
  throw Exception("SalomeHPContainerShared::cloneAlways : you are not supposed to be in this situation ! This type of container has only existence during execution !");
}

std::string SalomeHPContainerShared::getName() const
{
  return getTheBoss()->getName();
}

std::string SalomeHPContainerShared::getDiscreminantStrOfThis(const Task *askingNode) const
{
  return getTheBoss()->getDiscreminantStrOfThis(askingNode);
}

void SalomeHPContainerShared::start(const Task *askingNode) throw(YACS::Exception)
{
  SalomeContainerToolsSpreadOverTheResDecorator sct(&getTheBoss()->getContainerInfo(),_pd->getPlayGround(),_launchModeType,askingNode);
  startInternal(askingNode,sct,getTheBoss()->getComponentNames());
}

void SalomeHPContainerShared::allocateFor(const std::vector<const Task *>& nodes)
{
  _launchModeType->allocateForAmong(_idsOfKernelContainers,nodes);
}

std::size_t SalomeHPContainerShared::getNumberOfFreePlace() const
{
  return _launchModeType->getNumberOfFreePlaceAmong(_idsOfKernelContainers);
}

void SalomeHPContainerShared::forYourTestsOnly(ForTestOmlyHPContCls *data) const
{
  data->setContainerType("HPContainerShared");
  data->setPD(_pd);
  data->setIDS(_idsOfKernelContainers);
}

/*
 * SalomeHPContainerVectOfHelper is an holder of vector of SalomeContainerMonoHelper (holding itself a Kernel Container)
 * SalomeContainerTools is a Engines::ContainerParameters holder. It is the data keeper for GiveContainer invokation.
 * 
 */
