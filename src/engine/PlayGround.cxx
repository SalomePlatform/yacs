// Copyright (C) 2006-2017  CEA/DEN, EDF R&D
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

#include "PlayGround.hxx"
#include "Runtime.hxx"

#include <set>
#include <map>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <algorithm>

using namespace YACS::ENGINE;

std::string PlayGround::printSelf() const
{
  std::ostringstream oss;
  std::size_t sz(0);
  for(std::vector< std::pair<std::string,int> >::const_iterator it=_data.begin();it!=_data.end();it++)
    sz=std::max(sz,(*it).first.length());
  for(std::vector< std::pair<std::string,int> >::const_iterator it=_data.begin();it!=_data.end();it++)
    {
      oss << " - " << std::setw(10) << (*it).first << " : " << (*it).second << std::endl;
    }
  return oss.str();
}

void PlayGround::loadFromKernelCatalog()
{
  Runtime *r(getRuntime());
  if(!r)
    throw Exception("PlayGround::loadFromKernelCatalog : no runtime  !");
  std::vector< std::pair<std::string,int> > data(r->getCatalogOfComputeNodes());
  setData(data);
}

void PlayGround::setData(const std::vector< std::pair<std::string,int> >& defOfRes)
{
  _data=defOfRes;
  checkCoherentInfo();
}

int PlayGround::getNumberOfCoresAvailable() const
{
  int ret(0);
  for(std::vector< std::pair<std::string,int> >::const_iterator it=_data.begin();it!=_data.end();it++)
    ret+=(*it).second;
  return ret;
}

int PlayGround::getMaxNumberOfContainersCanBeHostedWithoutOverlap(int nbCoresPerCont) const
{
  if(nbCoresPerCont<1)
    throw Exception("PlayGround::getMaxNumberOfContainersCanBeHostedWithoutOverlap : invalid nbCoresPerCont. Must be >=1 !");
  int ret(0);
  for(std::vector< std::pair<std::string,int> >::const_iterator it=_data.begin();it!=_data.end();it++)
    ret+=(*it).second/nbCoresPerCont;
  return ret;
}

std::vector<int> PlayGround::computeOffsets() const
{
  std::size_t sz(_data.size()),i(0);
  std::vector<int> ret(sz+1); ret[0]=0;
  for(std::vector< std::pair<std::string,int> >::const_iterator it=_data.begin();it!=_data.end();it++,i++)
    ret[i+1]=ret[i]+(*it).second;
  return ret;
}

void PlayGround::checkCoherentInfo() const
{
  std::set<std::string> s;
  for(std::vector< std::pair<std::string,int> >::const_iterator it=_data.begin();it!=_data.end();it++)
    {
      s.insert((*it).first);
      if((*it).second<0)
        throw Exception("Presence of negative int value !");
    }
  if(s.size()!=_data.size())
    throw Exception("host names entries must be different each other !");
}

std::vector<bool> PlayGround::FromUItoVB(unsigned int sz, unsigned int v)
{
  std::vector<bool> ret(sz);
  unsigned int p(1);
  for(std::size_t i=0;i<sz;i++)
    {
      ret[i]=p&v;
      p<<=1;
    }
  return ret;
}

unsigned int PlayGround::FromVBtoUI(const std::vector<bool>& v)
{
  std::size_t sz(v.size());
  unsigned int ret(0);
  for(std::size_t i=0;i<sz;i++)
    {
      if(v[i])
        ret+=1u<<i;
    }
  return ret;
}

std::vector<int> PlayGround::GetIdsMatching(const std::vector<bool>& bigArr, const std::vector<bool>& pat)
{
  std::vector<int> ret;
  std::size_t szp(pat.size());
  std::size_t sz(bigArr.size()/szp);
  for(std::size_t i=0;i<sz;i++)
    {
      std::vector<bool> t(bigArr.begin()+i*szp,bigArr.begin()+(i+1)*szp);
      if(t==pat)
        ret.push_back(i);
    }
  return ret;
}

std::vector<int> PlayGround::BuildVectOfIdsFromVecBool(const std::vector<bool>& v)
{
  std::size_t sz(std::count(v.begin(),v.end(),true)),i(0);
  std::vector<int> ret(sz);
  std::vector<bool>::const_iterator it(v.begin());
  while(i<sz)
    {
      it=std::find(it,v.end(),true);
      ret[i++]=std::distance(v.begin(),it);
      it++;
    }
  return ret;
}

void PlayGround::highlightOnIds(const std::vector<int>& coreIds, std::vector<bool>& v) const
{
  if(v.size()!=getNumberOfCoresAvailable())
    throw Exception("PlayGround::highlightOnIds : oops ! invalid size !");
  for(std::vector<int>::const_iterator it=coreIds.begin();it!=coreIds.end();it++)
    v[*it]=true;
}

/*! 
 * you must garantee coherence between PlayGround::deduceMachineFrom, PlayGround::getNumberOfWorkers, and PartDefinition::computeWorkerIdsCovered
 */
std::vector<bool> PlayGround::getFetchedCores(int nbCoresPerWorker) const
{
  int nbCores(getNumberOfCoresAvailable());
  std::vector<bool> ret(nbCores,false);
  if(nbCoresPerWorker==1)
    std::fill(ret.begin(),ret.end(),true);
  else
    {
      std::size_t posBg(0);
      for(std::vector< std::pair<std::string,int> >::const_iterator it=_data.begin();it!=_data.end();it++)
        {
          int nbElemsToPutOn(((*it).second/nbCoresPerWorker)*nbCoresPerWorker);
          std::fill(ret.begin()+posBg,ret.begin()+posBg+nbElemsToPutOn,true);
          posBg+=(*it).second;
        }
    }
  return ret;
}
/*!
 * follow getMaxNumberOfContainersCanBeHostedWithoutOverlap method
 */
std::vector<std::size_t> PlayGround::getWorkerIdsFullyFetchedBy(int nbCoresPerComp, const std::vector<bool>& coreFlags) const
{
  std::size_t posBg(0),posWorker(0);
  std::vector<std::size_t> ret;
  for(std::vector< std::pair<std::string,int> >::const_iterator it=_data.begin();it!=_data.end();it++)
    {
      int nbWorker((*it).second/nbCoresPerComp);
      for(int j=0;j<nbWorker;j++,posWorker++)
        {
          std::vector<bool>::const_iterator it2(std::find(coreFlags.begin()+posBg+j*nbCoresPerComp,coreFlags.begin()+posBg+(j+1)*nbCoresPerComp,false));
          if(it2==coreFlags.begin()+posBg+(j+1)*nbCoresPerComp)
            ret.push_back(posWorker);
        }
      posBg+=(*it).second;
    }
  return ret;
}

std::vector< YACS::BASES::AutoRefCnt<PartDefinition> > PlayGround::partition(const std::vector< std::pair<const PartDefinition *,double> >& parts) const
{
  std::size_t sz(parts.size()),szs(getNumberOfCoresAvailable());
  if(sz==0)
    return std::vector< YACS::BASES::AutoRefCnt<PartDefinition> >();
  if(sz==1)
    {
      const PartDefinition *pd(parts[0].first);
      if(!pd)
        throw Exception("Presence of null pointer as part def  0 !");
      YACS::BASES::AutoRefCnt<PartDefinition> ret(pd->copy());
      std::vector< YACS::BASES::AutoRefCnt<PartDefinition> > ret2(1,ret);
      return ret2;
    }
  if(sz>31)
    throw Exception("PlayGround::partition : not implemented yet for more than 31 ! You need to pay for it :)");
  std::vector<bool> zeArr(szs*sz,false);
  std::size_t i(0);
  for(std::vector< std::pair<const PartDefinition *,double> >::const_iterator it=parts.begin();it!=parts.end();it++,i++)
    {
      const PartDefinition *pd((*it).first);
      if(!pd)
        throw Exception("Presence of null pointer as part def !");
      if(pd->getPlayGround()!=this)
        throw Exception("Presence of non homogeneous playground !");
      if((*it).second<=0.)
        throw Exception("Invalid weight !");
      std::vector<bool> bs(pd->getCoresOn());
      for(std::size_t j=0;j<szs;j++)
        zeArr[j*sz+i]=bs[j];
    }
  std::set<unsigned int> ss;
  for(std::size_t i=0;i<szs;i++)
    {
      std::vector<bool> vb(zeArr.begin()+i*sz,zeArr.begin()+(i+1)*sz);
      ss.insert(FromVBtoUI(vb));
    }
  std::vector< std::vector<int> > retIds(sz);
  for(std::set<unsigned int>::const_iterator i=ss.begin();i!=ss.end();i++)
    {
      std::vector<bool> code(FromUItoVB(sz,*i));// for this configuration which parts are considered
      std::vector<int> locIds(GetIdsMatching(zeArr,code));
      std::vector<int> partsIds(BuildVectOfIdsFromVecBool(code));
      if(partsIds.empty())
        continue;
      std::vector<double> wg;
      std::vector<int> nbCores2;
      for(std::vector<int>::const_iterator it=partsIds.begin();it!=partsIds.end();it++)
        {
          wg.push_back(parts[*it].second);
          nbCores2.push_back(parts[*it].first->getNbCoresPerCompo());
        }
      std::vector< std::vector<int> > ress(splitIntoParts(locIds,nbCores2,wg));
      std::size_t k(0);
      for(std::vector<int>::const_iterator it=partsIds.begin();it!=partsIds.end();it++,k++)
        {
          retIds[*it].insert(retIds[*it].end(),ress[k].begin(),ress[k].end());
        }
    }
  //
  std::vector< YACS::BASES::AutoRefCnt<PartDefinition> > ret(sz);
  for(std::size_t i=0;i<sz;i++)
    {
      std::set<int> s(retIds[i].begin(),retIds[i].end());
      std::vector<int> v(s.begin(),s.end());
      ret[i]=PartDefinition::BuildFrom(this,(parts[i].first)->getNbCoresPerCompo(),v);
    }
  return ret;
}

std::vector<int> sortArr(const std::vector<int>& v)
{
  std::multimap<int,int> m;
  int i(v.size()-1);
  for(std::vector<int>::const_reverse_iterator it=v.rbegin();it!=v.rend();it++)
    m.insert(std::pair<int,int>(*it,i--));
  std::vector<int> ret(m.size());
  i=0;
  for(std::multimap<int,int>::const_reverse_iterator it=m.rbegin();it!=m.rend();it++)// reverse -> sort from biggest to the finest
    ret[i++]=(*it).second;
  return ret;
}

std::vector< std::vector<int> > PlayGround::splitIntoParts(const std::vector<int>& coreIds, const std::vector<int>& nbCoresConso, const std::vector<double>& weights) const
{
  double wgs(std::accumulate(weights.begin(),weights.end(),0.));
  std::size_t sz(nbCoresConso.size());
  if(sz!=weights.size())
    throw Exception("PlayGround::splitIntoParts : internal error !");
  if(sz==0)
    return std::vector< std::vector<int> >();
  int totalSpace(coreIds.size());
  std::vector< std::vector<int> > ret(sz);
  std::vector<int> fromBigToTiny(sortArr(nbCoresConso));// start to treat the coarse grain to finish with fine grain
  std::vector<bool> zeArr(getNumberOfCoresAvailable(),false);
  highlightOnIds(coreIds,zeArr);
  int nbOfCoresToSplit(coreIds.size());
  std::size_t ii(fromBigToTiny.size());
  for(std::vector<int>::const_iterator it=fromBigToTiny.begin();it!=fromBigToTiny.end();it++,ii--)
    {
      int maxNbOfCores((int)(totalSpace*weights[*it]/wgs));// now try to find in zeArr at most maxNbOfCores cores
      ret[*it]=takePlace(maxNbOfCores,nbCoresConso[*it],zeArr,ii==1);
    }
  return ret;
}

std::vector<int> PlayGround::takePlace(int maxNbOfCoresToAlloc, int nbCoresPerShot, std::vector<bool>& distributionOfCores, bool lastOne) const
{
  if(maxNbOfCoresToAlloc<1)
    throw Exception("PlayGround::takePlace : internal error ! no space to alloc !");
  int tmpMaxNbOfCoresToAlloc(maxNbOfCoresToAlloc);
  if(lastOne)
    tmpMaxNbOfCoresToAlloc=std::max(tmpMaxNbOfCoresToAlloc,(int)std::count(distributionOfCores.begin(),distributionOfCores.end(),true));
  std::vector<int> ret;
  std::vector<int> offsets(computeOffsets());
  int nbFullItem(0);
  std::size_t sz(offsets.size()-1);
  for(std::size_t i=0;i<sz && tmpMaxNbOfCoresToAlloc>=nbCoresPerShot;i++)
    {
      int d(offsets[i+1]-offsets[i]);
      if(nbCoresPerShot>d)
        continue;
      std::vector<bool> target(nbCoresPerShot,true);
      for(int j=0;j<=d-nbCoresPerShot && tmpMaxNbOfCoresToAlloc>=nbCoresPerShot;)
        {
          std::vector<bool> t(distributionOfCores.begin()+offsets[i]+j,distributionOfCores.begin()+offsets[i]+j+nbCoresPerShot);
          if(t==target)
            {
              nbFullItem++;
              tmpMaxNbOfCoresToAlloc-=nbCoresPerShot;
              std::fill(distributionOfCores.begin()+offsets[i]+j,distributionOfCores.begin()+offsets[i]+j+nbCoresPerShot,false);
              for(int k=offsets[i]+j;k<offsets[i]+j+nbCoresPerShot;k++)
                ret.push_back(k);
              j+=nbCoresPerShot;
            }
          else
            j++;
        }
    }
  if(nbFullItem>0)
    return ret;
  if(nbCoresPerShot<=1)
    throw Exception("PlayGround::takePlace : internal error !");
  // not enough contiguous place. Find the first wider contiguous place
  for(int kk=std::min(nbCoresPerShot-1,tmpMaxNbOfCoresToAlloc);kk>=1;kk--)
    {
      for(std::size_t i=0;i<sz && tmpMaxNbOfCoresToAlloc>=kk;i++)
        {
          int d(offsets[i+1]-offsets[i]);
          if(kk>d)
            continue;
          std::vector<bool> target(kk,true);
          for(int j=0;j<=d-kk && tmpMaxNbOfCoresToAlloc>=kk;)
            {
              std::vector<bool> t(distributionOfCores.begin()+offsets[i]+j,distributionOfCores.begin()+offsets[i]+j+kk);
              if(t==target)
                {
                  nbFullItem++;
                  tmpMaxNbOfCoresToAlloc-=kk;
                  std::fill(distributionOfCores.begin()+offsets[i]+j,distributionOfCores.begin()+offsets[i]+j+kk,false);
                  for(int k=offsets[i]+j;k<offsets[i]+j+kk;k++)
                    ret.push_back(k);
                  return ret;
                }
              else
                j++;
            }
        }
    }
  throw Exception("PlayGround::takePlace : internal error ! All cores are occupied !");
}

int PlayGround::fromWorkerIdToResId(int workerId, int nbProcPerNode) const
{
  std::size_t sz2(_data.size());
  std::vector<int> deltas(sz2+1); deltas[0]=0;
  for(std::size_t i=0;i<sz2;i++)
    deltas[i+1]=deltas[i]+(_data[i].second)/nbProcPerNode;
  int zePos(0);
  while(zePos<sz2 && (workerId<deltas[zePos] || workerId>=deltas[zePos+1]))
    zePos++;
  if(zePos==sz2)
    zePos=workerId%sz2;
  return zePos;
}

/*! 
 * you must garantee coherence between PlayGround::deduceMachineFrom, PlayGround::getNumberOfWorkers, and PartDefinition::computeWorkerIdsCovered
 */
std::string PlayGround::deduceMachineFrom(int workerId, int nbProcPerNode) const
{
  int zePos(fromWorkerIdToResId(workerId,nbProcPerNode));
  return _data[zePos].first;
}

/*! 
 * you must garantee coherence between PlayGround::deduceMachineFrom, PlayGround::getNumberOfWorkers, PlayGround::getFetchedCores and PartDefinition::computeWorkerIdsCovered
 */
int PlayGround::getNumberOfWorkers(int nbCoresPerWorker) const
{
  return getMaxNumberOfContainersCanBeHostedWithoutOverlap(nbCoresPerWorker);
}

PlayGround::~PlayGround()
{
}

//////////////////////

PartDefinition::PartDefinition(const PlayGround *pg, int nbOfCoresPerComp):_nbOfCoresPerComp(nbOfCoresPerComp)
{
  _pg.takeRef(pg);
}

PartDefinition::PartDefinition(const PartDefinition& other):_pg(other._pg),_nbOfCoresPerComp(other._nbOfCoresPerComp)
{
}

PartDefinition::~PartDefinition()
{
}

std::vector< YACS::BASES::AutoRefCnt<PartDefinition> > PartDefinition::partition(const std::vector< double >& wgs) const
{
  std::size_t sz(wgs.size());
  std::vector< std::pair<const PartDefinition *,double> > elts(sz);
  for(std::size_t i=0;i<sz;i++)
    elts[i]=std::pair<const PartDefinition *,double>(this,wgs[i]);
  return getPlayGround()->partition(elts);
}

YACS::BASES::AutoRefCnt<PartDefinition> PartDefinition::BuildFrom(const PlayGround *pg, int nbOfCoresPerComp, const std::vector<int>& coreIds)
{
  int spaceSz(pg->getNumberOfCoresAvailable()),sz(coreIds.size());
  if(sz>spaceSz)
    throw Exception("PartDefinition::BuildFrom : error 1 !");
  if(sz==0)
    throw Exception("PartDefinition::BuildFrom : error 2 !");
  int zeStart(coreIds.front()),zeEnd(coreIds.back());
  if(zeStart<0 || zeEnd<zeStart)
    throw Exception("PartDefinition::BuildFrom : error ! The content of core Ids is not OK !");
  for(std::size_t i=0;i<sz-1;i++)
    if(coreIds[i+1]<coreIds[i])
      throw Exception("PartDefinition::BuildFrom : error ! The content of core Ids is not OK 2 !");
  if(zeEnd-zeStart+1!=sz)
    {
      YACS::BASES::AutoRefCnt<PartDefinition> pd(new NonContigPartDefinition(pg,nbOfCoresPerComp,coreIds));
      return pd;
    }
  if(sz==spaceSz)
    {
      YACS::BASES::AutoRefCnt<PartDefinition> pd(new AllPartDefinition(pg,nbOfCoresPerComp));
      return pd;
    }
  YACS::BASES::AutoRefCnt<PartDefinition> pd(new ContigPartDefinition(pg,nbOfCoresPerComp,zeStart,zeEnd+1));
  return pd;
}

void PartDefinition::stashPart(int nbCoresStashed, double weightOfRemain, YACS::BASES::AutoRefCnt<PartDefinition>& pdStashed, YACS::BASES::AutoRefCnt<PartDefinition>& pdRemain) const
{
  if(nbCoresStashed<=0)
    throw Exception("stashPart : Invalid nbCoresStashed value !");
  if(weightOfRemain<=0.)
    throw Exception("stashPart : Invalid weight !");
  std::vector<bool> coresOn(getCoresOn());
  int nbCoresAvailable(std::count(coresOn.begin(),coresOn.end(),true));
  std::vector<int> ids(PlayGround::BuildVectOfIdsFromVecBool(coresOn));
  if(nbCoresAvailable==0)
    throw Exception("PartDefinition::stashPart : no available cores !");
  if(nbCoresAvailable<=nbCoresStashed)
    {
      int n0((int)(1./(1.+weightOfRemain)*nbCoresAvailable)); n0=std::max(n0,1);
      int n1(nbCoresAvailable-n0);
      if(n1<=0)
        {
          pdStashed=PartDefinition::BuildFrom(getPlayGround(),1,ids);
          pdRemain=PartDefinition::BuildFrom(getPlayGround(),1,ids);
        }
      else
        {
          std::vector<int> ids0(ids.begin(),ids.begin()+n0),ids1(ids.begin()+n0,ids.end());
          pdStashed=PartDefinition::BuildFrom(getPlayGround(),1,ids0);
          pdRemain=PartDefinition::BuildFrom(getPlayGround(),1,ids1);
        }
    }
  else
    {
      std::vector<int> ids0(ids.begin(),ids.begin()+nbCoresStashed),ids1(ids.begin()+nbCoresStashed,ids.end());
      pdStashed=PartDefinition::BuildFrom(getPlayGround(),1,ids0);
      pdRemain=PartDefinition::BuildFrom(getPlayGround(),1,ids1);
    }
}

/*! 
 * you must garantee coherence between PlayGround::deduceMachineFrom, PlayGround::getNumberOfWorkers, and PartDefinition::computeWorkerIdsCovered
 */
std::vector<std::size_t> PartDefinition::computeWorkerIdsCovered(int nbCoresPerComp) const
{
  std::vector<bool> coresOn(getCoresOn());
  return _pg->getWorkerIdsFullyFetchedBy(nbCoresPerComp,coresOn);
}

//////////////////////

ContigPartDefinition::ContigPartDefinition(const PlayGround *pg, int nbOfCoresPerComp, int zeStart, int zeStop):PartDefinition(pg,nbOfCoresPerComp),_start(zeStart),_stop(zeStop)
{
  if(_start<0 || _stop<_start || _stop>getSpaceSize())
    throw Exception("ContigPartDefinition constructor : Invalid input values");
}

ContigPartDefinition::ContigPartDefinition(const ContigPartDefinition& other):PartDefinition(other),_start(other._start),_stop(other._stop)
{
}

std::string ContigPartDefinition::printSelf() const
{
  std::ostringstream oss;
  oss << "Contiguous : start=" << _start << " stop=" << _stop;
  return oss.str();
}

std::vector<bool> ContigPartDefinition::getCoresOn() const
{
  std::vector<bool> ret(getSpaceSize(),false);
  for(int i=_start;i<_stop;i++)
    ret[i]=true;
  return ret;
}

ContigPartDefinition *ContigPartDefinition::copy() const
{
  return new ContigPartDefinition(*this);
}

int ContigPartDefinition::getNumberOfCoresConsumed() const
{
  return _stop-_start;
}

//////////////////////

NonContigPartDefinition::NonContigPartDefinition(const PlayGround *pg, int nbOfCoresPerComp, const std::vector<int>& ids):PartDefinition(pg,nbOfCoresPerComp),_ids(ids)
{
  checkOKIds();
}

NonContigPartDefinition::NonContigPartDefinition(const ContigPartDefinition& other):PartDefinition(other)
{
}

std::string NonContigPartDefinition::printSelf() const
{
  std::ostringstream oss;
  oss << "Non contiguous : ";
  for(std::vector<int>::const_iterator it=_ids.begin();it!=_ids.end();it++)
    oss << *it << ", ";
  return oss.str();
}

std::vector<bool> NonContigPartDefinition::getCoresOn() const
{
  std::vector<bool> ret(getSpaceSize(),false);
  for(std::vector<int>::const_iterator it=_ids.begin();it!=_ids.end();it++)
    ret[*it]=true;
  return ret;
}

NonContigPartDefinition *NonContigPartDefinition::copy() const
{
  return new NonContigPartDefinition(*this);
}

int NonContigPartDefinition::getNumberOfCoresConsumed() const
{
  return _ids.size();
}

void NonContigPartDefinition::checkOKIds() const
{
  int maxVal(getSpaceSize());
  if(_ids.empty())
    return;
  int val(_ids.front());
  if(val<0 || val>=maxVal)
    throw Exception("checkOKIds : error 2 !");
  std::size_t sz(_ids.size());
  for(std::size_t i=0;i<sz-1;i++)
    {
      if(_ids[i+1]<=_ids[i])
        throw Exception("checkOKIds : error 1 !");
      if(_ids[i+1]>=maxVal)
        throw Exception("checkOKIds : error 3 !");
    }
}

//////////////////////

AllPartDefinition::AllPartDefinition(const AllPartDefinition& other):PartDefinition(other)
{
}

std::string AllPartDefinition::printSelf() const
{
  std::ostringstream oss;
  oss << "All";
  return oss.str();
}

std::vector<bool> AllPartDefinition::getCoresOn() const
{
  std::vector<bool> ret(getSpaceSize(),true);
  return ret;
}

AllPartDefinition *AllPartDefinition::copy() const
{
  return new AllPartDefinition(*this);
}

int AllPartDefinition::getNumberOfCoresConsumed() const
{
  return getSpaceSize();
}

//////////////////////
 
 std::vector<int> ForTestOmlyHPContCls::getIDS() const
 {
   std::size_t sz(_ids.size());
   std::vector<int> ret(sz);
   for(std::size_t i=0;i<sz;i++)
     ret[i]=_ids[i];
   return ret;
 }
