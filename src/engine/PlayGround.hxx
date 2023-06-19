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

#ifndef __PLAYGROUND_HXX__
#define __PLAYGROUND_HXX__

#include "YACSlibEngineExport.hxx"
#include "RefCounter.hxx"
#include "AutoRefCnt.hxx"
#include "ComplexWeight.hxx"

#include <vector>
#include <string>
#include <mutex>
#include <map>

namespace YACS
{
  namespace ENGINE
  {
    class PartDefinition;

    class YACSLIBENGINE_EXPORT Resource
    {
    public:
      Resource(const std::string& name, int nbCores):_name(name),_nbCores(nbCores),_occupied(_nbCores,false) { }
      Resource(const std::pair<std::string,int>& p):_name(p.first),_nbCores(p.second),_occupied(_nbCores,false) { }
      std::pair<std::string,int> toPair() const { return {_name,_nbCores}; }
      int nbCores() const { return _nbCores; }
      std::string name() const { return _name; }
      std::size_t getNumberOfFreePlace(int nbCoresPerCont) const;
      std::vector<std::size_t> allocateFor(std::size_t& nbOfPlacesToTake, int nbCoresPerCont) const;
      void release(std::size_t workerId, int nbCoresPerCont) const;
      std::size_t getNumberOfWorkers(int nbCoresPerCont) const;
      void printSelf(std::ostream& oss) const;
    private:
      std::string _name;
      int _nbCores;
      mutable std::vector<bool> _occupied;
    };


    class ResourceIterator : public std::iterator<
                        std::input_iterator_tag,     // iterator_category
                        Resource,                    // value_type
                        long,                        // difference_type
                        const Resource*,             // pointer
                        std::pair<std::string,int> > // reference
    {
      const std::vector< Resource > *_vec;
      std::size_t _num;
    public:
      explicit ResourceIterator(const std::vector< Resource > *vec, const std::size_t num) : _vec(vec),_num(num) { }
      ResourceIterator& operator++() { _num++; return *this; }
      bool operator==(ResourceIterator other) const { return _num == other._num; }
      bool operator!=(ResourceIterator other) const { return !(*this == other); }
      reference operator*() const { return (*_vec)[_num].toPair(); }
    };
    
    class YACSLIBENGINE_EXPORT PlayGround : public RefCounter
    {
    public:
      PlayGround(const std::vector< std::pair<std::string,int> >& defOfRes):_data(defOfRes.begin(),defOfRes.end()) { checkCoherentInfo(); }
      PlayGround() { }
      std::string printSelf() const;
      void loadFromKernelCatalog();
      std::vector< std::pair<std::string,int> > getData() const { return std::vector< std::pair<std::string,int> >(ResourceIterator(&_data,0),ResourceIterator(&_data,_data.size())); }
      void setData(const std::vector< std::pair<std::string,int> >& defOfRes);
      int getNumberOfCoresAvailable() const;
      int getMaxNumberOfContainersCanBeHostedWithoutOverlap(int nbCoresPerCont) const;
      std::vector<int> computeOffsets() const;
      std::vector< YACS::BASES::AutoRefCnt<PartDefinition> > partition(const std::vector< std::pair<const PartDefinition *, const ComplexWeight *> >& parts, const std::vector<int> &nbCoresPerShot) const;
      int fromWorkerIdToResId(int workerId, int nbProcPerNode) const;
      std::string deduceMachineFrom(int workerId, int nbProcPerNode) const;
      int getNumberOfWorkers(int nbCoresPerWorker) const;
      void highlightOnIds(const std::vector<int>& coreIds, std::vector<bool>& v) const;
      std::vector<bool> getFetchedCores(int nbCoresPerWorker) const;
      std::vector<std::size_t> getWorkerIdsFullyFetchedBy(int nbCoresPerComp, const std::vector<bool>& coreFlags) const;
      static std::vector<int> BuildVectOfIdsFromVecBool(const std::vector<bool>& v);
      static std::vector<int> GetIdsMatching(const std::vector<bool>& bigArr, const std::vector<bool>& pat);
    public:// critical section part
      std::size_t getNumberOfFreePlace(int nbCoresPerCont) const;
      std::vector<std::size_t> allocateFor(std::size_t nbOfPlacesToTake, int nbCoresPerCont) const;
      void release(std::size_t workerId, int nbCoresPerCont) const;
      std::mutex& getLocker() const { return _locker; }
      void printMe() const;
    private:
      std::vector< std::pair <const ComplexWeight *, int> > bigToTiny(const std::vector< std::pair <const ComplexWeight *, int> > &weights, std::map<int,int> &saveOrder) const;
	  std::vector< std::vector<int> > backToOriginalOrder(const std::vector< std::vector<int> > &disorderVec, const std::map<int,int> &saveOrder) const;
	  int getCriticalPath(const std::vector<std::pair <const ComplexWeight *, int > >& weights, const std::vector<int>& maxNbOfCores) const;    
      std::vector< std::vector<int> > splitIntoParts(const std::vector<int>& coreIds, const std::vector<std::pair <const ComplexWeight *, int> >& weights) const;
      std::vector<int> takePlace(int maxNbOfCoresToAlloc, int nbCoresPerShot, std::vector<bool>& distributionOfCores, bool lastOne=false) const;
    private:
      void checkCoherentInfo() const;
    private:
      ~PlayGround();
    private:
      mutable std::mutex _locker;
      std::vector< Resource > _data;
    };

    class YACSLIBENGINE_EXPORT PartDefinition : public RefCounter
    {
    protected:
      PartDefinition(const PlayGround *pg);
      PartDefinition(const PartDefinition& other);
      virtual ~PartDefinition();
    public:
      //std::vector< YACS::BASES::AutoRefCnt<PartDefinition> > partition(const std::vector< const ComplexWeight *>& wgs) const;
      static YACS::BASES::AutoRefCnt<PartDefinition> BuildFrom(const PlayGround *pg, const std::vector<int>& coreIds);
      const PlayGround *getPlayGround() const { return _pg; }
      int getSpaceSize() const { return _pg->getNumberOfCoresAvailable(); }
      void stashPart(int nbCoresStashed, double weightOfRemain, YACS::BASES::AutoRefCnt<PartDefinition>& pdStashed, YACS::BASES::AutoRefCnt<PartDefinition>& pdRemain) const;
      std::vector<std::size_t> computeWorkerIdsCovered(int nbCoresPerComp) const;
      virtual std::string printSelf() const = 0;
      virtual std::vector<bool> getCoresOn() const = 0;
      virtual PartDefinition *copy() const = 0;
      virtual int getNumberOfCoresConsumed() const = 0;
    private:
      YACS::BASES::AutoConstRefCnt<PlayGround> _pg;
    };

    class YACSLIBENGINE_EXPORT ContigPartDefinition : public PartDefinition
    {
    public:
      ContigPartDefinition(const PlayGround *pg, int zeStart, int zeStop);
      ContigPartDefinition(const ContigPartDefinition& other);
      std::string printSelf() const;
      std::vector<bool> getCoresOn() const;
      int getStart() const { return _start; }
      int getStop() const { return _stop; }
      ContigPartDefinition *copy() const;
      int getNumberOfCoresConsumed() const;
    private:
      ~ContigPartDefinition() { }
    private:
      int _start;
      int _stop;
    };

    class YACSLIBENGINE_EXPORT NonContigPartDefinition : public PartDefinition
    {
    public:
      NonContigPartDefinition(const PlayGround *pg, const std::vector<int>& ids);
      NonContigPartDefinition(const ContigPartDefinition& other);
      std::string printSelf() const;
      std::vector<bool> getCoresOn() const;
      std::vector<int> getIDs() const { return _ids; }
      NonContigPartDefinition *copy() const;
      int getNumberOfCoresConsumed() const;
    private:
      void checkOKIds() const;
      ~NonContigPartDefinition() { }
    private:
      std::vector<int> _ids;
    };
      
    class YACSLIBENGINE_EXPORT AllPartDefinition : public PartDefinition
    {
    public:
      AllPartDefinition(const PlayGround *pg):PartDefinition(pg) { }
      AllPartDefinition(const AllPartDefinition& other);
      std::string printSelf() const;
      std::vector<bool> getCoresOn() const;
      AllPartDefinition *copy() const;
      int getNumberOfCoresConsumed() const;
    private:
      ~AllPartDefinition() { }
    };

    class YACSLIBENGINE_EXPORT ForTestOmlyHPContCls
    {
    public:

#ifndef SWIG
      void setContainerType(const std::string& ct) { _container_type=ct; }
      void setPD(YACS::BASES::AutoConstRefCnt<PartDefinition> pd) { _pd=pd; }
      void setIDS(const std::vector<std::size_t>& ids) { _ids=ids; }
#endif
      std::string getContainerType() const { return _container_type; }
      const PartDefinition *getPD() const { return _pd; }
      std::vector<int> getIDS() const;
    private:
      std::string _container_type;
      YACS::BASES::AutoConstRefCnt<PartDefinition> _pd;
      std::vector<std::size_t> _ids;
    };
  }
}

#endif
