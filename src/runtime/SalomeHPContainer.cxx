// Copyright (C) 2006-2023  CEA, EDF
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

#include <algorithm>
#include <utility>

using namespace YACS::ENGINE;

const char SalomeHPContainer::KIND[]="HPSalome";

SalomeHPContainer::SalomeHPContainer():_shutdownLevel(999)
{
}

SalomeHPContainer::SalomeHPContainer(const SalomeHPContainer& other):_componentNames(other._componentNames),_shutdownLevel(999),_sct(other._sct),_initScript(other._initScript)
{
}

void SalomeHPContainer::assignPG(const PlayGround *pg)
{
  HomogeneousPoolContainer::assignPG(pg);
  int nbOfWorkers(getPG()->getNumberOfWorkers(this->getNumberOfCoresPerWorker()));
  this->setSizeOfPool(nbOfWorkers);
}

void SalomeHPContainer::setSizeOfPool(int sz)
{
  _launchModeType.resize(sz);
}

int SalomeHPContainer::getSizeOfPool() const
{
  return _launchModeType.size();
}

std::size_t SalomeHPContainer::getNumberOfFreePlace() const
{
  return getPG()->getNumberOfFreePlace(this->getNumberOfCoresPerWorker());
}

class PairVecIterator : public std::iterator<
                        std::input_iterator_tag,     // iterator_category
                        std::pair<const Task *,std::size_t>,                    // value_type
                        long,                        // difference_type
                        const std::pair<const Task *,std::size_t>*,             // pointer
                        std::pair<const Task *,std::size_t> > // reference
{
  const std::vector< const Task *> *_v0;
  const std::vector<std::size_t> *_v1;
  std::size_t _num;
public:
  explicit PairVecIterator(const std::vector< const Task * > *v0, const std::vector<std::size_t> *v1, const std::size_t num) : _v0(v0),_v1(v1),_num(num) { }
  PairVecIterator& operator++() { _num++; return *this; }
  bool operator==(PairVecIterator other) const { return _num == other._num; }
  bool operator!=(PairVecIterator other) const { return !(*this == other); }
  reference operator*() const { return std::pair<const Task *,std::size_t>((*_v0)[_num],(*_v1)[_num]); }
};

void SalomeHPContainer::allocateFor(const std::vector<const Task *>& nodes)
{
  std::vector<std::size_t> workerIdsAllocated(getPG()->allocateFor(nodes.size(),this->getNumberOfCoresPerWorker()));
  std::vector<std::pair<const Task *,std::size_t>> nodesAndIds(PairVecIterator(&nodes,&workerIdsAllocated,0),PairVecIterator(&nodes,&workerIdsAllocated,nodes.size()));
  _launchModeType.allocateForCrude(nodesAndIds);
}

void SalomeHPContainer::release(const Task *node)
{
  std::size_t workerId(_launchModeType.release(node));
  getPG()->release(workerId,this->getNumberOfCoresPerWorker());
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

bool SalomeHPContainer::isAlreadyStarted(const Task *askingNode) const
{
  const SalomeContainerMonoHelper *helper(_launchModeType.getHelperOfTaskThreadSafe(this,askingNode));
  return helper->isAlreadyStarted(askingNode);
}

void SalomeHPContainer::start(const Task *askingNode)
{
  SalomeContainerMonoHelper *helper(_launchModeType.getHelperOfTaskThreadSafe(this,askingNode));
  SalomeContainerToolsDecorator sctDeco(&_sct,this->getPG(),&_launchModeType,askingNode,this->getNumberOfCoresPerWorker());
  SalomeContainerTools::Start(_componentNames,helper,_sct,_shutdownLevel,this,askingNode);
}

void SalomeHPContainer::shutdown(int level)
{
  if(level < _shutdownLevel)
      return;
  _shutdownLevel=999;
  for(std::size_t i=0;i<_launchModeType.size();i++)
    {
      SalomeContainerMonoHelper *helper(_launchModeType.at(i));
      helper->shutdown();
    }
}

std::string SalomeHPContainer::getPlacementId(const Task *askingNode) const
{
  const SalomeContainerMonoHelper *helper(nullptr);
  {
    std::lock_guard<std::mutex> lg(getPG()->getLocker());
    helper=_launchModeType.getHelperOfTask(askingNode);
  }
  return SalomeContainerTools::GetPlacementId(helper,this,askingNode);
}

std::string SalomeHPContainer::getFullPlacementId(const Task *askingNode) const
{
  const SalomeContainerMonoHelper *helper(nullptr);
  {
    std::lock_guard<std::mutex> lg(getPG()->getLocker());
    helper=_launchModeType.getHelperOfTask(askingNode);
  }
  return SalomeContainerTools::GetFullPlacementId(helper,this,askingNode);
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
    _sct.setProperty(name,value);
}

std::string SalomeHPContainer::getProperty(const std::string& name) const
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
    return _sct.getProperty(name);
}

void SalomeHPContainer::clearProperties()
{
  _initScript.clear();
  _sct.clearProperties();
}

void SalomeHPContainer::addComponentName(const std::string& name)
{
  _componentNames.push_back(name);
}

std::map<std::string,std::string> SalomeHPContainer::getProperties() const
{
  std::map<std::string,std::string> ret(_sct.getProperties());
  std::ostringstream oss; oss << getSizeOfPool();
  ret[SIZE_OF_POOL_KEY]=oss.str();
  if(!_initScript.empty())
    ret[INITIALIZE_SCRIPT_KEY]=_initScript;
  return ret;
}

std::map<std::string,std::string> SalomeHPContainer::getResourceProperties(const std::string& name) const
{
  return _sct.getResourceProperties(name);
}

void SalomeHPContainer::checkCapabilityToDealWith(const ComponentInstance *inst) const
{
  if(inst->getKind()!=SalomeHPComponent::KIND)
    throw Exception("SalomeHPContainer::checkCapabilityToDealWith : SalomeContainer is not able to deal with this type of ComponentInstance.");
}

std::vector<std::string> SalomeHPContainer::getKernelContainerNames() const
{
  return _launchModeType.getKernelContainerNames(this);
}

int SalomeHPContainer::getNumberOfCoresPerWorker() const
{
  return _sct.getNumberOfCoresPerWorker();
}
