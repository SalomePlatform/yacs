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
// std::vector<bool> PlayGround::getFetchedCores(int nbCoresPerWorker) const
// {
//   int nbCores(getNumberOfCoresAvailable());
//   std::vector<bool> ret(nbCores,false);
//   if(nbCoresPerWorker==1)
//     std::fill(ret.begin(),ret.end(),true);
//   else
//     {
//       std::size_t posBg(0);
//       for(std::vector< std::pair<std::string,int> >::const_iterator it=_data.begin();it!=_data.end();it++)
//         {
//           int nbElemsToPutOn(((*it).second/nbCoresPerWorker)*nbCoresPerWorker);
//           std::fill(ret.begin()+posBg,ret.begin()+posBg+nbElemsToPutOn,true);
//           posBg+=(*it).second;
//         }
//     }
//   return ret;
//}
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

std::vector< YACS::BASES::AutoRefCnt<PartDefinition> > PlayGround::partition(const std::vector< std::pair<const PartDefinition *, const ComplexWeight *> >& parts, const std::vector< int>& nbCoresPerShot) const
{
  std::size_t sz(parts.size()),szs(getNumberOfCoresAvailable());
  if (sz!=nbCoresPerShot.size())
    throw Exception("PlayGround::partition : incoherent vector size !");
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
  for(std::vector< std::pair<const PartDefinition *, const ComplexWeight *> >::const_iterator it=parts.begin();it!=parts.end();it++,i++)
    {
      const PartDefinition *pd((*it).first);
      if(!pd)
        throw Exception("Presence of null pointer as part def !");
      if(pd->getPlayGround()!=this)
        throw Exception("Presence of non homogeneous playground !");
      std::vector<bool> bs(pd->getCoresOn()); // tab of length nbCores, with True or False for each Core
      for(std::size_t j=0;j<szs;j++)
        zeArr[j*sz+i]=bs[j]; // remplis une table avec les valeurs de bs. La table est [nb part] * [nb cores]
    }
  std::vector< std::vector<int> > retIds(sz);
  for(std::size_t i=0;i<szs;i++)
    {
      std::vector<bool> code(zeArr.begin()+i*sz,zeArr.begin()+(i+1)*sz);// vecteur contenant le true/false d'un coeur pour ttes les partitions
      std::vector<int> locIds(GetIdsMatching(zeArr,code)); // liste des coeurs qui peuvent correspondre au pattern code
      std::vector<int> partsIds(BuildVectOfIdsFromVecBool(code));// pour chaque partition retourne l'id de la premiere partition à true
      if(partsIds.empty())
        continue;
      std::vector<std::pair <const ComplexWeight *, int> > wg;
      for(std::vector<int>::const_iterator it=partsIds.begin();it!=partsIds.end();it++)
        {
          wg.push_back(std::pair <const ComplexWeight *, int> (parts[*it].second, nbCoresPerShot[*it]));
        }
      std::vector< std::vector<int> > ress(splitIntoParts(locIds,wg));
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
      ret[i]=PartDefinition::BuildFrom(this,v);
    }
  return ret;
}

std::vector< std::vector<int> > PlayGround::splitIntoParts(const std::vector<int>& coreIds, const std::vector< std::pair <const ComplexWeight *, int> >& weights) const
{
  std::size_t sz(weights.size());
  if(sz==0)
    return std::vector< std::vector<int> >();
  std::vector< std::vector<int> > ret;
  std::vector< std::vector<int> > disorderRet(sz);
  std::vector<bool> zeArr(getNumberOfCoresAvailable(),false);
  highlightOnIds(coreIds,zeArr);
  int nbOfCoresToSplit(coreIds.size());
  int totalSpace(coreIds.size());
  int remainingSpace(totalSpace);
  std::vector<int> nbOfCoresAllocated(sz);
  std::vector<int> nbCoresPerShot(sz,-1);  
  // first every other branchs take its minimal part of the cake
  // and remove branch without valid weight
  int i(0);
  std::map<int,int> saveOrder;
  const std::vector< std::pair <const ComplexWeight *, int> > sortedWeights(bigToTiny(weights, saveOrder));
  for(std::vector<std::pair <const ComplexWeight *, int> >::const_iterator it=sortedWeights.begin();it!=sortedWeights.end();it++,i++)
    {
      nbCoresPerShot[i]=(*it).second;
      if ((*it).first->isUnsetLoopWeight())
        {
      	  nbOfCoresAllocated[i]=nbCoresPerShot[i]; // branch with only elementary nodes
        }
      else if (!(*it).first->hasValidLoopWeight())
        {
          nbOfCoresAllocated[i]=std::max((int)((double)totalSpace/(double)(sz)), nbCoresPerShot[i]); // branch with undefined weight, takes his part proportionnally to the number of branchs
        }
      else
        {
          nbOfCoresAllocated[i]=nbCoresPerShot[i];
        }
    }
  remainingSpace-=std::accumulate(nbOfCoresAllocated.begin(), nbOfCoresAllocated.end(), 0);
  //get critical path (between path with loopWeight)
  int criticalPathRank=getCriticalPath(sortedWeights, nbOfCoresAllocated);
  if (criticalPathRank!=-1)
    {
      // add cores to critical path while enough cores are availables
	  while (remainingSpace >= nbCoresPerShot[criticalPathRank])
	    {
	      nbOfCoresAllocated[criticalPathRank]+=nbCoresPerShot[criticalPathRank];
	      remainingSpace-=nbCoresPerShot[criticalPathRank];
	      criticalPathRank=getCriticalPath(sortedWeights, nbOfCoresAllocated);
	    }
	  //fill other paths with remaining cores (if possible) (reuse fromBigToTiny here?)
	  // and takePlace
	  int coresToAdd;
	  int j(0);
	  for(std::vector<int>::iterator it=nbOfCoresAllocated.begin();it!=nbOfCoresAllocated.end();it++,j++)
	    {
	      coresToAdd=((int)(remainingSpace/nbCoresPerShot[j]))*nbCoresPerShot[j];
	      *it+=coresToAdd;
	      remainingSpace-=coresToAdd;
	    }
    }
  int k(0);
  for(std::vector<int>::iterator it=nbOfCoresAllocated.begin();it!=nbOfCoresAllocated.end();it++,k++)
    {
      disorderRet[k]=takePlace(*it,nbCoresPerShot[k],zeArr,k==(sz-1));
    }   
  ret=backToOriginalOrder(disorderRet, saveOrder);
  return ret;
}

std::vector< std::pair <const ComplexWeight *, int> > PlayGround::bigToTiny(const std::vector< std::pair <const ComplexWeight *, int> > &weights, std::map<int,int> &saveOrder) const
{
  int maxCoresPerShot(0), rankMax(0);
  int i(0);
  std::vector< std::pair <const ComplexWeight *, int> > ret;
  std::size_t sz(weights.size());
  while (ret.size()<sz)
    {
      for(std::vector<std::pair <const ComplexWeight *, int> >::const_iterator it=weights.begin();it!=weights.end();it++,i++)
        {
          if ((maxCoresPerShot<(*it).second) && (saveOrder.find(i)==saveOrder.end()))
            {
              maxCoresPerShot=(*it).second;
              rankMax=i;
            }
        }
      ret.push_back(std::pair <const ComplexWeight *, int>(weights[rankMax].first,weights[rankMax].second));
      saveOrder[rankMax]=ret.size()-1;
      maxCoresPerShot=0;
      i=0;
    }
  return ret;	
}

std::vector< std::vector<int> > PlayGround::backToOriginalOrder(const std::vector< std::vector<int> > &disorderVec, const std::map<int,int> &saveOrder) const
{
  std::vector< std::vector<int> > ret;
  std::size_t sz(disorderVec.size());
  if (disorderVec.size()!=saveOrder.size())
    throw Exception("PlayGround::backToOriginalOrder : incoherent vector size !");
  for (int i=0; i<sz; i++)
      ret.push_back(disorderVec[saveOrder.at(i)]);
  return ret;
}

int PlayGround::getCriticalPath(const std::vector<std::pair <const ComplexWeight *, int > >& weights, const std::vector<int>& nbOfCoresAllocated) const
{
  double maxWeight(0.);
  double pathWeight(0.);
  int rankMaxPath(-1);
  if ((weights.size()!=nbOfCoresAllocated.size()) || (weights.size()==0))
    throw Exception("PlayGround::getCriticalPath : internal error !");
  int i=0;
  for(std::vector<std::pair <const ComplexWeight *, int> >::const_iterator it=weights.begin();it!=weights.end();it++,i++)
    {
      if (nbOfCoresAllocated[i]==0)
        throw Exception("PlayGround::getCriticalPath : nbOfCoresAllocated is null! error !");
      if (!(*it).first->isDefaultValue())
        {
  	      pathWeight=(*it).first->calculateTotalLength(nbOfCoresAllocated[i]);
  	      if (pathWeight > maxWeight)
  	        {
  	          maxWeight=pathWeight;
  	          rankMaxPath=i;
  	        }
        }	  
    }
  return rankMaxPath;
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

PartDefinition::PartDefinition(const PlayGround *pg)
{
  _pg.takeRef(pg);
}

PartDefinition::PartDefinition(const PartDefinition& other):_pg(other._pg)
{
}

PartDefinition::~PartDefinition()
{
}

// std::vector< YACS::BASES::AutoRefCnt<PartDefinition> > PartDefinition::partition(const std::vector< const ComplexWeight *>& wgs) const
// {
//   std::size_t sz(wgs.size());
//   std::vector< std::pair<const PartDefinition *, const ComplexWeight *> > elts(sz);
//   for(std::size_t i=0;i<sz;i++)
//     elts[i]=std::pair<const PartDefinition *, const ComplexWeight *>(this,wgs[i]);
//   return getPlayGround()->partition(elts);
// }

YACS::BASES::AutoRefCnt<PartDefinition> PartDefinition::BuildFrom(const PlayGround *pg, const std::vector<int>& coreIds)
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
      YACS::BASES::AutoRefCnt<PartDefinition> pd(new NonContigPartDefinition(pg,coreIds));
      return pd;
    }
  if(sz==spaceSz)
    {
      YACS::BASES::AutoRefCnt<PartDefinition> pd(new AllPartDefinition(pg));
      return pd;
    }
  YACS::BASES::AutoRefCnt<PartDefinition> pd(new ContigPartDefinition(pg,zeStart,zeEnd+1));
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
          pdStashed=PartDefinition::BuildFrom(getPlayGround(),ids);
          pdRemain=PartDefinition::BuildFrom(getPlayGround(),ids);
        }
      else
        {
          std::vector<int> ids0(ids.begin(),ids.begin()+n0),ids1(ids.begin()+n0,ids.end());
          pdStashed=PartDefinition::BuildFrom(getPlayGround(),ids0);
          pdRemain=PartDefinition::BuildFrom(getPlayGround(),ids1);
        }
    }
  else
    {
      std::vector<int> ids0(ids.begin(),ids.begin()+nbCoresStashed),ids1(ids.begin()+nbCoresStashed,ids.end());
      pdStashed=PartDefinition::BuildFrom(getPlayGround(),ids0);
      pdRemain=PartDefinition::BuildFrom(getPlayGround(),ids1);
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

ContigPartDefinition::ContigPartDefinition(const PlayGround *pg, int zeStart, int zeStop):PartDefinition(pg),_start(zeStart),_stop(zeStop)
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

NonContigPartDefinition::NonContigPartDefinition(const PlayGround *pg, const std::vector<int>& ids):PartDefinition(pg),_ids(ids)
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
