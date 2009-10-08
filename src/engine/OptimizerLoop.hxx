//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef __OPTIMIZERLOOP_HXX__
#define __OPTIMIZERLOOP_HXX__

#include "YACSlibEngineExport.hxx"
#include "Pool.hxx"
#include "Thread.hxx"
#include "DynParaLoop.hxx"
#include "DynLibLoader.hxx"
#include "OptimizerAlg.hxx"
#include "ElementaryNode.hxx"
#include "DrivenCondition.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class YACSLIBENGINE_EXPORT OptimizerAlgStandardized : public OptimizerAlgSync
    {
    protected:
      ::YACS::BASES::Thread *_threadInCaseOfNotEvent;
      ::YACS::BASES::DrivenCondition _condition;
      OptimizerAlgBase *_algBehind;
    public:
      OptimizerAlgStandardized(Pool *pool, OptimizerAlgBase *alg);
      ~OptimizerAlgStandardized();
      TypeCode *getTCForIn() const;
      TypeCode *getTCForOut() const;
      void setAlgPointer(OptimizerAlgBase* alg);
      OptimizerAlgBase * getAlg(){return _algBehind;};
      void parseFileToInit(const std::string& fileName);
      void initialize(const Any *input) throw (Exception);
      void takeDecision();
      void finish();
      void start();
    protected:
      static void *threadFctForAsync(void* ownStack);
    };

    class OptimizerLoop;

    class FakeNodeForOptimizerLoop : public ElementaryNode
    {
      friend class OptimizerLoop;
    private:
      OptimizerLoop *_loop;
      unsigned _reason;
      bool _normal;
    private:
      FakeNodeForOptimizerLoop(OptimizerLoop *loop, bool normal, unsigned reason);
      FakeNodeForOptimizerLoop(const FakeNodeForOptimizerLoop& other);
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
      void exForwardFailed();
      void exForwardFinished();
      void execute();
      void aborted();
      void finished();
    private:
      static const char NAME[];
      static const unsigned char ALG_WITHOUT_START_CASES = 52;
      static const unsigned char NO_BRANCHES = 53;
      static const unsigned char NO_ALG_INITIALIZATION = 54;
    };

    class YACSLIBENGINE_EXPORT OptimizerLoop : public DynParaLoop
    {
      friend class FakeNodeForOptimizerLoop;
      
    protected:
      Pool _myPool;
      bool _algInitOnFile;
      std::string _symbol;
      AnyInputPort _portForInitFile;
      ::YACS::BASES::DynLibLoader _loader;
      OptimizerAlgStandardized *_alg;
      AnyInputPort _retPortForOutPool;
      std::vector<bool> _initNodeUpdated;
      bool _convergenceReachedWithOtherCalc;
      FakeNodeForOptimizerLoop *_nodeForSpecialCases;
      std::vector<AnyInputPort *> _interceptorsForOutPool;
      ::YACS::BASES::DrivenCondition _condForCompletenessB4Relaunch;
      //! outputports interceptors leaving current scope.
      std::map<InputPort *, std::vector<InputPort *> > _interceptors;
    public:
      OptimizerLoop(const std::string& name, const std::string& algLibWthOutExt,
                    const std::string& symbolNameToOptimizerAlgBaseInstanceFactory,
                    bool algInitOnFile,bool initAlgo=true);
      OptimizerLoop(const OptimizerLoop& other, ComposedNode *father, bool editionOnly);
      ~OptimizerLoop();
      void init(bool start=true);
      void exUpdateState();
      int getNumberOfInputPorts() const;
      InputPort *edGetPortForOutPool() { return &_retPortForOutPool; }
      InputPort *edGetPortForInitFile() { return &_portForInitFile; }
      InputPort *getInputPort(const std::string& name) const throw(Exception);
      std::list<InputPort *> getSetOfInputPort() const;
      std::list<InputPort *> getLocalInputPorts() const;
      void selectRunnableTasks(std::vector<Task *>& tasks);
      void getReadyTasks(std::vector<Task *>& tasks);
      YACS::Event updateStateOnFinishedEventFrom(Node *node);
      void checkNoCyclePassingThrough(Node *node) throw(Exception);
      virtual void accept(Visitor *visitor);
      std::string getSymbol() const { return _symbol; }
      std::string getAlgLib() const ;
      virtual void setAlgorithm(const std::string& alglib,const std::string& symbol,bool checkLinks=true);
      virtual void checkBasicConsistency() const throw(Exception);

    protected:
      void buildDelegateOf(InPort * & port, OutPort *initialStart, const std::list<ComposedNode *>& pointsOfView);
      void buildDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView);
      void checkControlDependancy(OutPort *start, InPort *end, bool cross,
                                  std::map < ComposedNode *,  std::list < OutPort * >, SortHierarc >& fw,
                                  std::vector<OutPort *>& fwCross,
                                  std::map< ComposedNode *, std::list < OutPort *>, SortHierarc >& bw,
                                  LinkInfo& info) const;
      void checkCFLinks(const std::list<OutPort *>& starts, InputPort *end, unsigned char& alreadyFed, bool direction, LinkInfo& info) const;
    protected:
      void cleanInterceptors();
      void launchMaxOfSamples(bool first);
      bool isFullyLazy() const;
      bool isFullyBusy(unsigned& branchId) const;
      void initInterceptors(unsigned nbOfBr);
      void pushValueOutOfScopeForCase(unsigned branchId);
      Node *simpleClone(ComposedNode *father, bool editionOnly=true) const;
    protected:
      static const int NOT_RUNNING_BRANCH_ID;
      static const int NOT_INITIALIZED_BRANCH_ID;
      static const char NAME_OF_FILENAME_INPUT[];
      static const char NAME_OF_OUT_POOL_INPUT[];
    };
  }
}

#endif
