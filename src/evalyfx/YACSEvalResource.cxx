// Copyright (C) 2012-2016  CEA/DEN, EDF R&D
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
// Author : Anthony Geay (EDF R&D)

#include "YACSEvalResource.hxx"

#include "DeploymentTree.hxx"
#include "Container.hxx"
#include "Exception.hxx"

#include "SalomeContainer.hxx"
#include "SalomeHPContainer.hxx"
#include "ComponentInstance.hxx"

#include "ResourcesManager.hxx"

#include <set>
#include <queue>
#include <sstream>
#include <iterator>
#include <algorithm>

const char YACSEvalVirtualYACSContainer::CONTAINER_NAME_KEY[]="container_name";

const char YACSEvalVirtualYACSContainer::CPU_CLOCK_KEY[]="cpu_clock";

const char YACSEvalVirtualYACSContainer::HOSTNAME_KEY[]="hostname";

const char YACSEvalVirtualYACSContainer::MEM_KEY[]="mem_mb";

const char YACSEvalVirtualYACSContainer::NB_NODE_KEY[]="nb_node";

const char YACSEvalVirtualYACSContainer::NB_PROC_PER_NODE_KEY[]="nb_proc_per_node";

const char YACSEvalVirtualYACSContainer::NB_RESOURCE_PROCS_KEY[]="nb_resource_procs";

const char YACSEvalVirtualYACSContainer::POLICY_KEY[]="policy";

const char YACSEvalVirtualYACSContainer::OS_KEY[]="OS";

/*!
 * This class is here only to avoid "friend class YACSEvalListOfResources".
 */
class YACSEvalResourceEff : public YACSEvalResource
{
public:
  YACSEvalResourceEff(YACSEvalListOfResources *gf, const std::vector< YACS::ENGINE::Container * >& conts);
};

YACSEvalResourceEff::YACSEvalResourceEff(YACSEvalListOfResources *gf, const std::vector< YACS::ENGINE::Container * >& conts):YACSEvalResource(gf,conts)
{
}

void YACSEvalNonConstLocker::checkNonLocked() const
{
  if(_isLocked)
    throw YACS::Exception("YACSEvalNonConstLocker::checkNonLocked : this is locked and trying to invoke non-const method !");
}

void YACSEvalNonConstLocker::checkLocked() const
{
  if(!_isLocked)
    throw YACS::Exception("YACSEvalNonConstLocker::checkLocked : this is NOT locked whereas it should be !");
}

YACSEvalVirtualYACSContainer::YACSEvalVirtualYACSContainer():_gf(0),_cont(0)
{
}

void YACSEvalVirtualYACSContainer::set(YACSEvalResource *gf, YACS::ENGINE::Container *cont)
{
  checkNonLocked();
  _gf=gf;
  if(_cont==cont)
    return ;
  if(_cont)
    _cont->decrRef();
  _cont=cont;
  if(_cont)
    _cont->incrRef();
  _propertyMap=listOfPropertiesInYACSContainer();
}

YACSEvalVirtualYACSContainer::YACSEvalVirtualYACSContainer(YACSEvalResource *gf, YACS::ENGINE::Container *cont):_gf(gf),_cont(cont)
{
  if(_cont)
    _cont->incrRef();
  _propertyMap=listOfPropertiesInYACSContainer();
}

YACSEvalVirtualYACSContainer::~YACSEvalVirtualYACSContainer()
{
  if(_cont)
    _cont->decrRef();
}

/*!
 * \param [in,out] entry - Resource type instance to be updated with \a this.
 */
void YACSEvalVirtualYACSContainer::aggregate(ParserResourcesType& entry) const
{
  if(entry.HostName.empty())
    entry.HostName=getHostName();
  entry.DataForSort._memInMB=std::max(entry.DataForSort._memInMB,getMem());
  entry.DataForSort._nbOfProcPerNode=std::max(entry.DataForSort._nbOfProcPerNode,getNbProcPerNode());
  entry.DataForSort._nbOfNodes=std::max(entry.DataForSort._nbOfNodes,getNbNodes());
  entry.DataForSort._CPUFreqMHz=std::max(entry.DataForSort._CPUFreqMHz,getCPUFreq());
  std::vector<YACS::ENGINE::ComponentInstance *> comps(_gf->getGodFather()->getDeploymentTree()->getComponentsLinkedToContainer(_cont));
  std::set<std::string> compNames;
  for(std::vector<YACS::ENGINE::ComponentInstance *>::const_iterator it=comps.begin();it!=comps.end();it++)
    {
      YACS::ENGINE::ComponentInstance *elt(*it);
      if(elt)
        compNames.insert(elt->getCompoName());
    }
  compNames.insert(entry.ComponentsList.begin(),entry.ComponentsList.end());
  compNames.erase(std::string());
  std::vector<std::string> compNames2(compNames.begin(),compNames.end());
  entry.ComponentsList=compNames2;
}

std::string YACSEvalVirtualYACSContainer::findDefault(bool isInteractive) const
{
  std::vector<std::string> possibleHosts(_gf->getAllFittingMachines());
  for(std::vector<std::string>::const_iterator it=possibleHosts.begin();it!=possibleHosts.end();it++)
    {
      if(_gf->getGodFather()->hasRightInteractiveStatus(*it,isInteractive))
        return *it;
    }
  throw YACS::Exception("YACSEvalVirtualYACSContainer::findDefault : impossible to find a right machine with requested interactive status !");
}

void YACSEvalVirtualYACSContainer::setWantedMachine(const std::string& machine)
{
  checkNonLocked();
  std::vector<std::string> possibleHosts(_gf->getAllFittingMachines());
  if(std::find(possibleHosts.begin(),possibleHosts.end(),machine)==possibleHosts.end())
    throw YACS::Exception("YACSEvalVirtualYACSContainer::setWantedMachine : the specified machine is not in the list of available !");
  std::string oldMachine(getChosenMachine());
  setMachineNoCheck(machine);
  try
  {
      _gf->notifyWantedMachine(this,oldMachine,machine);
  }
  catch(YACS::Exception& e)
  {
      setMachineNoCheck(oldMachine);
      throw e;
  }
}

std::vector<std::string> YACSEvalVirtualYACSContainer::listOfPropertyKeys() const
{
  std::set<std::string> s;
  std::map<std::string,std::string>::const_iterator it;
  for(it=_overloadedPropertyMap.begin();it!=_overloadedPropertyMap.end();it++)
    s.insert((*it).first);
  for(it=_propertyMap.begin();it!=_propertyMap.end();it++)
    s.insert((*it).first);
  std::vector<std::string> ret(s.begin(),s.end());
  return ret;
}

std::string YACSEvalVirtualYACSContainer::getValueOfKey(const char *key) const
{
  std::string skey(key);
  if(skey==HOSTNAME_KEY)
    return _chosenHost;
  std::map<std::string,std::string>::const_iterator it;
  it=_overloadedPropertyMap.find(skey);
  if(it!=_overloadedPropertyMap.end())
    return (*it).second;
  it=_propertyMap.find(skey);
  if(it!=_propertyMap.end())
    return (*it).second;
  return std::string();
}

void YACSEvalVirtualYACSContainer::setProperty(const std::string& key, const std::string &value)
{
  checkNonLocked();
  if(key==HOSTNAME_KEY)
    setWantedMachine(value);
  else
    _overloadedPropertyMap[key]=value;
}

void YACSEvalVirtualYACSContainer::apply(bool interactiveStatus)
{
  YACS::ENGINE::SalomeContainer *cont0(dynamic_cast<YACS::ENGINE::SalomeContainer *>(_cont));
  YACS::ENGINE::SalomeHPContainer *cont1(dynamic_cast<YACS::ENGINE::SalomeHPContainer *>(_cont));
  if(!cont0 && !cont1)
    throw YACS::Exception("YACSEvalVirtualYACSContainer::apply : unrecognized container !");
  if(interactiveStatus)
    _cont->setProperty(HOSTNAME_KEY,getValueOfKey(HOSTNAME_KEY));
  else
    {// in cluster mode no hostname and policy set to cycl by default
      _cont->setProperty(HOSTNAME_KEY,std::string());
      _cont->setProperty("policy","cycl");
    }
}

std::string YACSEvalVirtualYACSContainer::getName() const
{
  checkNotNullYACSContainer();
  return _cont->getName();
}

unsigned int YACSEvalVirtualYACSContainer::getValueOfKeyUInt(const char *key) const
{
  std::string v(getValueOfKey(key));
  unsigned int ret(0);
  if(v.empty())
    return ret;
  std::istringstream iss(v);
  iss >> ret;
  return ret;
}

std::map<std::string,std::string> YACSEvalVirtualYACSContainer::listOfPropertiesInYACSContainer() const
{
  YACS::ENGINE::SalomeContainer *cont0(dynamic_cast<YACS::ENGINE::SalomeContainer *>(_cont));
  YACS::ENGINE::SalomeHPContainer *cont1(dynamic_cast<YACS::ENGINE::SalomeHPContainer *>(_cont));
  std::map<std::string,std::string> props;
  if(cont0)
    props=cont0->getProperties();
  else if(cont1)
    props=cont1->getProperties();
  return props;
}

void YACSEvalVirtualYACSContainer::checkNotNullYACSContainer() const
{
  if(!_cont)
    throw YACS::Exception("YACSEvalVirtualYACSContainer::checkNotNullYACSContainer : internal YACS container is NULL !");
}

YACSEvalResource::~YACSEvalResource()
{
}

std::vector<std::string> YACSEvalResource::getAllChosenMachines() const
{
  std::set<std::string> s;
  for(std::vector< YACSEvalVirtualYACSContainer >::const_iterator it=_containers.begin();it!=_containers.end();it++)
    s.insert((*it).getChosenMachine());
  std::vector<std::string> ret(s.begin(),s.end());
  return ret;
}

std::vector<std::string> YACSEvalResource::getAllFittingMachines() const
{
  ParserResourcesType ref;
  aggregate(ref);
  return _gf->getFittingResources(&ref);
}

void YACSEvalResource::setWantedMachine(const std::string& machine)
{
  checkNonLocked();
  std::vector<std::string> possibleHosts(getAllFittingMachines());
  if(std::find(possibleHosts.begin(),possibleHosts.end(),machine)==possibleHosts.end())
    throw YACS::Exception("YACSEvalResource::setWantedMachine : the specified machine is not in the list of available !");
  for(std::vector< YACSEvalVirtualYACSContainer >::iterator it=_containers.begin();it!=_containers.end();it++)
    (*it).setMachineNoCheck(machine);
}

YACSEvalVirtualYACSContainer *YACSEvalResource::at(std::size_t i) const
{
  if(i>=_containers.size())
    throw YACS::Exception("YACSEvalResource::at : invalid input ! must be < size !");
  return const_cast<YACSEvalVirtualYACSContainer *>(&_containers[i]);
}

void YACSEvalResource::apply(bool interactiveStatus)
{
  for(std::vector< YACSEvalVirtualYACSContainer >::iterator it=_containers.begin();it!=_containers.end();it++)
    (*it).apply(interactiveStatus);
}

void YACSEvalResource::fitWithCurrentCatalogAbs()
{
  std::vector<std::string> ress(getAllFittingMachines());
  if(ress.empty())
    throw YACS::Exception("YACSEvalResource::fitWithCurrentCatalogAbs : no suitable host in your catalog of resource !");
  setMachineNoCheck(ress.front());
}

void YACSEvalResource::aggregate(ParserResourcesType& entry) const
{
  for(std::vector< YACSEvalVirtualYACSContainer >::const_iterator it=_containers.begin();it!=_containers.end();it++)
    (*it).aggregate(entry);
}

void YACSEvalResource::notifyWantedMachine(YACSEvalVirtualYACSContainer *sender, const std::string& oldMachine, const std::string& newMachine)
{
  _gf->notifyWantedMachine(sender,oldMachine,newMachine);
}

void YACSEvalResource::setMachineNoCheck(const std::string& machine)
{
  for(std::vector< YACSEvalVirtualYACSContainer >::iterator it=_containers.begin();it!=_containers.end();it++)
    (*it).setMachineNoCheck(machine);
}

YACSEvalResource::YACSEvalResource(YACSEvalListOfResources *gf, const std::vector< YACS::ENGINE::Container * >& conts):_gf(gf)
{
  std::size_t sz(conts.size());
  _containers.resize(sz);
  for(std::size_t i=0;i<sz;i++)
    _containers[i].set(this,conts[i]);
}

void YACSEvalParamsForCluster::setExclusiveness(bool newStatus)
{
  if(newStatus)
    throw YACS::Exception("YACSEvalParamsForCluster::setExclusiveness : exclusive mode set to true is not implemented yet !");
}

void YACSEvalParamsForCluster::checkConsistency() const
{
  if(_remoteWorkingDir.empty())
    throw YACS::Exception("YACSEvalParamsForCluster::checkConsistency : remote work dir is not set !");
  if(_localWorkingDir.empty())
    throw YACS::Exception("YACSEvalParamsForCluster::checkConsistency : local work dir is not set !");
  if(_wcKey.empty())
    throw YACS::Exception("YACSEvalParamsForCluster::checkConsistency : WC key is not set !");
  if(_nbOfProcs==0)
    throw YACS::Exception("YACSEvalParamsForCluster::checkConsistency : nb procs must be != 0 !");
}

YACSEvalListOfResources::YACSEvalListOfResources(int maxLevOfPara, ResourcesManager_cpp *rm, const YACS::ENGINE::DeploymentTree& dt):_maxLevOfPara(maxLevOfPara),_rm(rm),_dt(new YACS::ENGINE::DeploymentTree(dt))
{
  std::vector<YACS::ENGINE::Container *> conts(_dt->getAllContainers());
  std::map<std::string, std::vector<YACS::ENGINE::Container *> > listOfHosts;
  for(std::vector<YACS::ENGINE::Container *>::const_iterator it=conts.begin();it!=conts.end();it++)
    {
      std::vector<YACS::ENGINE::ComponentInstance *> cis(dt.getComponentsLinkedToContainer(*it));
      YACS::ENGINE::SalomeContainer *c1(dynamic_cast<YACS::ENGINE::SalomeContainer *>(*it));
      YACS::ENGINE::SalomeHPContainer *c2(dynamic_cast<YACS::ENGINE::SalomeHPContainer *>(*it));
      std::string zeHost;
      if(c1)
        zeHost=c1->getProperty(YACSEvalVirtualYACSContainer::HOSTNAME_KEY);
      if(c2)
        zeHost=c2->getProperty(YACSEvalVirtualYACSContainer::HOSTNAME_KEY);
      listOfHosts[zeHost].push_back(*it);
    }
  for(std::map<std::string, std::vector<YACS::ENGINE::Container *> >::const_iterator it=listOfHosts.begin();it!=listOfHosts.end();it++)
    _resources.push_back(new YACSEvalResourceEff(this,(*it).second));
  fitWithCurrentCatalog();
}

std::vector<std::string> YACSEvalListOfResources::getAllChosenMachines() const
{
  std::set<std::string> s;
  for(std::vector<YACSEvalResource *>::const_iterator it=_resources.begin();it!=_resources.end();it++)
    {
      std::vector<std::string> tmp((*it)->getAllChosenMachines());
      s.insert(tmp.begin(),tmp.end());
    }
  std::vector<std::string> ret(s.begin(),s.end());
  return ret;
}

std::vector<std::string> YACSEvalListOfResources::getAllFittingMachines() const
{
  ParserResourcesType ref;
  for(std::vector<YACSEvalResource *>::const_iterator it=_resources.begin();it!=_resources.end();it++)
    (*it)->aggregate(ref);
  return getFittingResources(&ref);
}

void YACSEvalListOfResources::setWantedMachine(const std::string& machine)
{
  checkNonLocked();
  std::vector<std::string> possibleHosts(getAllFittingMachines());
  if(std::find(possibleHosts.begin(),possibleHosts.end(),machine)==possibleHosts.end())
    throw YACS::Exception("YACSEvalResource::setWantedMachine : the specified machine is not in the list of available !");
  for(std::vector<YACSEvalResource *>::iterator it=_resources.begin();it!=_resources.end();it++)
    (*it)->setMachineNoCheck(machine);
}

YACSEvalResource *YACSEvalListOfResources::at(std::size_t i) const
{
  if(i>=size())
    throw YACS::Exception("YACSEvalListOfResources::at : invalid id !");
  return _resources[i];
}

bool YACSEvalListOfResources::isInteractive() const
{
  std::vector<std::string> allMachines(getAllChosenMachines());
  if(allMachines.empty())
    return true;
  std::size_t ii(0),sz(allMachines.size());
  std::vector<bool> status(sz);
  for(std::vector<std::string>::const_iterator it=allMachines.begin();it!=allMachines.end();it++,ii++)
    {
      status[ii]=isMachineInteractive(*it);
    }
  std::size_t trueRet(std::count(status.begin(),status.end(),true)),falseRet(std::count(status.begin(),status.end(),false));
  if(trueRet==sz && falseRet==0)
    return true;
  else if(trueRet==0 && falseRet==sz)
    return false;
  throw YACS::Exception("YACSEvalListOfResources::isInteractive : mix of interactive and non interactive ! internal error !");
}

unsigned int YACSEvalListOfResources::getNumberOfProcsDeclared() const
{
  if(isInteractive())
    {
      std::vector<std::string> chosen(getAllChosenMachines());
      unsigned int ret(0);
      for(std::vector<std::string>::const_iterator it=chosen.begin();it!=chosen.end();it++)
        ret+=getNumberOfProcOfResource(*it);
      return ret;
    }
  else
    return _paramsInCaseOfCluster.getNbProcs();
}

void YACSEvalListOfResources::checkOKForRun() const
{
  if(!isInteractive())
    _paramsInCaseOfCluster.checkConsistency();
}

void YACSEvalListOfResources::apply()
{
  bool interactiveSt(isInteractive());
  for(std::vector<YACSEvalResource *>::iterator it=_resources.begin();it!=_resources.end();it++)
    (*it)->apply(interactiveSt);
}

YACSEvalListOfResources::~YACSEvalListOfResources()
{
  delete _dt;
  for(std::vector<YACSEvalResource *>::iterator it=_resources.begin();it!=_resources.end();it++)
    delete *it;
}

bool YACSEvalListOfResources::isMachineInteractive(const std::string& machine) const
{
  const MapOfParserResourcesType& zeList(_rm->GetList());
  std::map<std::string, ParserResourcesType>::const_iterator it2(zeList.find(machine));
  if(it2==zeList.end())
    {
      std::ostringstream oss; oss << "YACSEvalListOfResources::isMachineInteractive : no such machine with name \"" << machine << "\" !";
      throw YACS::Exception(oss.str());
    }
  const ParserResourcesType& elt((*it2).second);
  return (elt.Batch==none);
}

class EffectiveComparator
{
public:
  virtual ~EffectiveComparator() { }
  virtual EffectiveComparator *copy() const = 0;
  virtual bool compare(const ResourceDataToSort& elt1, const ResourceDataToSort& elt2) const = 0;
};

// comparator for a null request in scheme
class BasicComparator : public EffectiveComparator
{
public:
  EffectiveComparator *copy() const { return new BasicComparator; }
  bool compare(const ResourceDataToSort& elt1, const ResourceDataToSort& elt2) const;
};

class MemAwareComparator : public EffectiveComparator
{
public:
  EffectiveComparator *copy() const { return new MemAwareComparator; }
  bool compare(const ResourceDataToSort& elt1, const ResourceDataToSort& elt2) const;
};

class CompromiseComparator : public EffectiveComparator
{
public:
  CompromiseComparator(int memMB, int nbProcs):_memMB(memMB),_nbProcs(nbProcs) { }
  EffectiveComparator *copy() const { return new CompromiseComparator(_memMB,_nbProcs); }
  bool compare(const ResourceDataToSort& elt1, const ResourceDataToSort& elt2) const;
private:
  int _memMB;
  int _nbProcs;
};

class CompareRes
{
public:
  CompareRes(const ParserResourcesType *request, const MapOfParserResourcesType& zeCatalog);
  CompareRes(const CompareRes& other);
  bool operator()(const std::string& res1, const std::string& res2);
  ~CompareRes() { delete _comp; }
private:
  EffectiveComparator *_comp;
  const ParserResourcesType *_request;
  const MapOfParserResourcesType& _zeCatalog;
};

bool BasicComparator::compare(const ResourceDataToSort& elt1, const ResourceDataToSort& elt2) const
{
  int nbOfProcs1(elt1._nbOfNodes*elt1._nbOfProcPerNode),nbOfProcs2(elt2._nbOfNodes*elt2._nbOfProcPerNode);
  return nbOfProcs1<nbOfProcs2;
}

bool MemAwareComparator::compare(const ResourceDataToSort& elt1, const ResourceDataToSort& elt2) const
{
  return elt1._memInMB<elt2._memInMB;
}

bool CompromiseComparator::compare(const ResourceDataToSort& elt1, const ResourceDataToSort& elt2) const
{
  double v1a(elt1._memInMB/_memMB),v1b(elt1._nbOfNodes*elt1._nbOfProcPerNode/_nbProcs);
  double v2a(elt2._memInMB/_memMB),v2b(elt2._nbOfNodes*elt2._nbOfProcPerNode/_nbProcs);
  return std::min(v1a,v1b)<std::min(v2a,v2b);
}

CompareRes::CompareRes(const ParserResourcesType *request, const MapOfParserResourcesType& zeCatalog):_comp(0),_request(request),_zeCatalog(zeCatalog)
{
  const ResourceDataToSort& tmp(_request->DataForSort);
  // all request items are set to default -> Basic comparison on procs
  if(request->DataForSort._nbOfNodes==1 && request->DataForSort._nbOfProcPerNode==1 && request->DataForSort._memInMB==0)
    _comp=new BasicComparator;
  // all items are set to default except memory -> Memory aware only
  else if(request->DataForSort._memInMB!=0 && request->DataForSort._nbOfNodes==1 && request->DataForSort._nbOfProcPerNode==1)
    _comp=new MemAwareComparator;
  // memory and procs are set by the user -> do the best possible.
  else if(request->DataForSort._memInMB!=0 && ( request->DataForSort._nbOfNodes!=1 || request->DataForSort._nbOfProcPerNode!=1 ) )
    _comp=new CompromiseComparator(request->DataForSort._memInMB,request->DataForSort._nbOfNodes*request->DataForSort._nbOfProcPerNode);
  else
    _comp=new BasicComparator;
}

CompareRes::CompareRes(const CompareRes& other):_comp(other._comp->copy()),_request(other._request),_zeCatalog(other._zeCatalog)
{
}

bool CompareRes::operator()(const std::string& res1, const std::string& res2)
{
  std::map<std::string, ParserResourcesType>::const_iterator it1(_zeCatalog.find(res1)),it2(_zeCatalog.find(res2));
  if(it1==_zeCatalog.end() || it2==_zeCatalog.end())
    throw YACS::Exception("Internal error during comparison !");
  const ParserResourcesType& elt1((*it1).second);
  const ParserResourcesType& elt2((*it2).second);
  return _comp->compare(elt1.DataForSort,elt2.DataForSort);
}

/*!
 * Implements an another alg than those proposed by default in ResourcesManager_cpp in "ResourcesManager.hxx".
 */
std::vector<std::string> YACSEvalListOfResources::getFittingResources(ParserResourcesType *request) const
{
  int mlop(getMaxLevelOfParallelism());
  request->DataForSort._memInMB*=mlop;
  request->DataForSort._nbOfNodes*=mlop;
  //
  const MapOfParserResourcesType& zeListInCatalog(_rm->GetList());
  std::vector<std::string> list0;
  // first keep those having relevant components
  const std::vector<std::string>& listExpected(request->ComponentsList);
  std::set<std::string> listExpected2(listExpected.begin(),listExpected.end());
  for(std::map<std::string, ParserResourcesType>::const_iterator it=zeListInCatalog.begin();it!=zeListInCatalog.end();it++)
    {
      const std::vector<std::string>& compoList((*it).second.ComponentsList);
      std::set<std::string> s1(compoList.begin(),compoList.end());
      std::vector<std::string> diff;
      std::set_difference(listExpected2.begin(),listExpected2.end(),s1.begin(),s1.end(),std::inserter(diff,diff.begin()));
      if(diff.empty())
        list0.push_back((*it).first);
    }
  // sort list0 regarding request
  std::sort(list0.begin(),list0.end(),CompareRes(request,zeListInCatalog));
  // if HostName in request is defined and in list0 put it first.
  std::string tmp(request->HostName);
  if(!tmp.empty())
    {
      std::vector<std::string>::iterator it1(std::find(list0.begin(),list0.end(),tmp));
      if(it1!=list0.end())
        {// HostName in list0 so put it in first place.
          list0.erase(it1);
          list0.insert(list0.begin(),tmp);
        }
    }
  return list0;
}

void YACSEvalListOfResources::notifyWantedMachine(YACSEvalVirtualYACSContainer *sender, const std::string& oldMachine, const std::string& newMachine)
{
  const MapOfParserResourcesType& zeList(_rm->GetList());
  std::map<std::string, ParserResourcesType>::const_iterator itOld(zeList.find(oldMachine)),itNew(zeList.find(newMachine));
  if(itOld==zeList.end() || itNew==zeList.end())
    throw YACS::Exception("YACSEvalListOfResources::notifyWantedMachine : internal error !");
  const ParserResourcesType& oldPRT((*itOld).second);
  const ParserResourcesType& newPRT((*itNew).second);
  bool oldISt(oldPRT.Batch==none),newISt(newPRT.Batch==none);//interactive status
  if(oldISt==newISt)
    return ;
  // the batch/interactive mode has changed -> try to change for all.
  std::queue<std::string> sts;
  try
  {
    if(newISt)
      {// switching from interactive to batch -> In batch every YACSEvalVirtualYACSContainer instances in this must lie on newMachine.
        for(std::vector<YACSEvalResource *>::const_iterator it=_resources.begin();it!=_resources.end();it++)
          {
            std::vector<std::string> fms((*it)->getAllFittingMachines());
            std::vector<std::string>::iterator it0(std::find(fms.begin(),fms.end(),newMachine));
            if(it0==fms.end())
              {
                std::ostringstream oss; oss << "In the context of switch to batch the requested cluster machine \"" << newMachine << "\" is not compatible for following list of containers : " << std::endl;
                std::size_t sz((*it)->size());
                for(std::size_t i=0;i<sz;i++)
                  {
                    YACSEvalVirtualYACSContainer *cont((*it)->at(i));
                    if(cont)
                      oss << " \"" << cont->getName() << "\", ";
                  }
                throw YACS::Exception(oss.str());
              }
            std::size_t sz((*it)->size());
            for(std::size_t i=0;i<sz;i++)
              {
                std::size_t sz((*it)->size());
                for(std::size_t i=0;i<sz;i++)
                  {
                    YACSEvalVirtualYACSContainer *cont((*it)->at(i));
                    if(cont==sender)
                      continue;
                    sts.push(newMachine);
                  }
              }
          }
      }
    else
      {
        for(std::vector<YACSEvalResource *>::const_iterator it=_resources.begin();it!=_resources.end();it++)
          {
            std::size_t sz((*it)->size());
            for(std::size_t i=0;i<sz;i++)
              {
                YACSEvalVirtualYACSContainer *cont((*it)->at(i));
                if(cont==sender)
                  continue;
                sts.push(cont->findDefault(false));
              }
          }
      }
  }
  catch(YACS::Exception& e)
  {
      std::ostringstream oss; oss << "YACSEvalListOfResources::notifyWantedMachine : switching from interactive/batch must be global ! " << e.what();
      throw YACS::Exception(oss.str());
  }
  for(std::vector<YACSEvalResource *>::const_iterator it=_resources.begin();it!=_resources.end();it++)
    {
      std::size_t sz((*it)->size());
      for(std::size_t i=0;i<sz;i++)
        {
          YACSEvalVirtualYACSContainer *cont((*it)->at(i));
          if(cont==sender)
            continue;
          cont->setMachineNoCheck(sts.front());
          sts.pop();
        }
    }
}

bool YACSEvalListOfResources::hasRightInteractiveStatus(const std::string& machineToTest, bool isInteractive) const
{
  const MapOfParserResourcesType& zeList(_rm->GetList());
  std::map<std::string, ParserResourcesType>::const_iterator it(zeList.find(machineToTest));
  if(it==zeList.end())
    throw YACS::Exception("YACSEvalListOfResources::hasRightInteractiveStatus : internal error !");
  const ParserResourcesType& elt((*it).second);
  bool myStatus(elt.Batch==none);
  return myStatus==isInteractive;
}

void YACSEvalListOfResources::fitWithCurrentCatalog()
{
  std::vector<std::string> ress(getAllFittingMachines());
  if(ress.empty())
    throw YACS::Exception("YACSEvalListOfResources::fitWithCurrentCatalog : no available resource in your catalog !");
  for(std::vector<YACSEvalResource *>::iterator it=_resources.begin();it!=_resources.end();it++)
    (*it)->setMachineNoCheck(ress.front());
}

unsigned int YACSEvalListOfResources::getNumberOfProcOfResource(const std::string& machine) const
{
  const MapOfParserResourcesType& zeList(_rm->GetList());
  std::map<std::string, ParserResourcesType>::const_iterator it(zeList.find(machine));
  if(it==zeList.end())
    throw YACS::Exception("YACSEvalListOfResources::getNumberOfProcOfResource : internal error !");
  const ParserResourcesType& PRT((*it).second);
  const ResourceDataToSort& RDT(PRT.DataForSort);
  unsigned int ret(RDT._nbOfNodes*RDT._nbOfProcPerNode);
  return ret;
}
