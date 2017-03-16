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

#ifndef __OPTIMIZERLOOP_HXX__
#define __OPTIMIZERLOOP_HXX__

#include "YACSlibEngineExport.hxx"
#include "Pool.hxx"
#include "DynParaLoop.hxx"
#include "DynLibLoader.hxx"
#include "OptimizerAlg.hxx"
#include "ElementaryNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class OptimizerLoop;

    class FakeNodeForOptimizerLoop : public ElementaryNode
    {
      friend class OptimizerLoop;
    private:
      OptimizerLoop *_loop;
      std::string _message;
      bool _normal;
    private:
      FakeNodeForOptimizerLoop(OptimizerLoop *loop, bool normal, std::string message);
      FakeNodeForOptimizerLoop(const FakeNodeForOptimizerLoop& other);
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
      void exForwardFailed();
      void exForwardFinished();
      void execute();
      void aborted();
      void finished();
    private:
      static const char NAME[];
    };

    class YACSLIBENGINE_EXPORT OptimizerLoop : public DynParaLoop
    {
      friend class FakeNodeForOptimizerLoop;
      
    protected:
      Pool _myPool;
      bool _algInitOnFile;
      std::string _symbol;
      std::string _alglib;
      AnyInputPort _algoInitPort;
      ::YACS::BASES::DynLibLoader * _loader;
      OptimizerAlgBase *_alg;
      AnyInputPort _retPortForOutPool;
      std::vector<bool> _initNodeUpdated;
      bool _convergenceReachedWithOtherCalc;
      FakeNodeForOptimizerLoop *_nodeForSpecialCases;
      std::vector<AnyInputPort *> _interceptorsForOutPool;
      //! outputports interceptors leaving current scope.
      std::map<InputPort *, std::vector<InputPort *> > _interceptors;
      AnyOutputPort _algoResultPort;
    public:
      OptimizerLoop(const std::string& name, const std::string& algLibWthOutExt,
                    const std::string& symbolNameToOptimizerAlgBaseInstanceFactory,
                    bool algInitOnFile,bool initAlgo=true, Proc * procForTypes = NULL);
      OptimizerLoop(const OptimizerLoop& other, ComposedNode *father, bool editionOnly);
      ~OptimizerLoop();
      void init(bool start=true);
      void exUpdateState();
      int getNumberOfInputPorts() const;
      InputPort *edGetPortForOutPool() { return &_retPortForOutPool; }
      InputPort *edGetAlgoInitPort() { return &_algoInitPort; }
      OutputPort *edGetAlgoResultPort() { return &_algoResultPort; }
      InputPort *getInputPort(const std::string& name) const throw(Exception);
      std::list<InputPort *> getSetOfInputPort() const;
      std::list<InputPort *> getLocalInputPorts() const;
      void selectRunnableTasks(std::vector<Task *>& tasks);
      void getReadyTasks(std::vector<Task *>& tasks);
      YACS::Event updateStateOnFinishedEventFrom(Node *node);
      void checkNoCyclePassingThrough(Node *node) throw(Exception);
      virtual void accept(Visitor *visitor);
      virtual std::string getSymbol() const { return _symbol; }
      virtual std::string getAlgLib() const ;
      virtual void setAlgorithm(const std::string& alglib,const std::string& symbol,
                                bool checkLinks=true, Proc * procForTypes = NULL);
      virtual void checkBasicConsistency() const throw(Exception);
      virtual std::string typeName() {return "YACS__ENGINE__OptimizerLoop";}
      int getNumberOfOutputPorts() const;
      std::list<OutputPort *> getSetOfOutputPort() const;
      std::list<OutputPort *> getLocalOutputPorts() const;
      OutPort *getOutPort(const std::string& name) const throw(Exception);
      OutputPort *getOutputPort(const std::string& name) const throw(Exception);
      YACS::Event finalize();

    protected:
      virtual YACS::Event updateStateOnFailedEventFrom(Node *node, const Executor *execInst);
      void buildDelegateOf(InPort * & port, OutPort *initialStart, const std::list<ComposedNode *>& pointsOfView);
      void buildDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView);
      void checkControlDependancy(OutPort *start, InPort *end, bool cross,
                                  std::map < ComposedNode *,  std::list < OutPort * >, SortHierarc >& fw,
                                  std::vector<OutPort *>& fwCross,
                                  std::map< ComposedNode *, std::list < OutPort *>, SortHierarc >& bw,
                                  LinkInfo& info) const;
      void checkCFLinks(const std::list<OutPort *>& starts, InputPort *end, unsigned char& alreadyFed, bool direction, LinkInfo& info) const;
      void checkLinkPossibility(OutPort *start, const std::list<ComposedNode *>& pointsOfViewStart,
                                InPort *end, const std::list<ComposedNode *>& pointsOfViewEnd) throw(Exception);
    protected:
      void cleanInterceptors();
      void launchMaxOfSamples(bool first);
      bool isFullyLazy() const;
      bool isFullyBusy(unsigned& branchId) const;
      void initInterceptors(unsigned nbOfBr);
      void pushValueOutOfScopeForCase(unsigned branchId);
      Node *simpleClone(ComposedNode *father, bool editionOnly=true) const;
      virtual void loadAlgorithm();

    private:
      TypeCode * checkTypeCode(TypeCode * tc, const char * portName);

    protected:
      static const int NOT_RUNNING_BRANCH_ID;
      static const int NOT_INITIALIZED_BRANCH_ID;
      static const char NAME_OF_ALGO_INIT_PORT[];
      static const char NAME_OF_OUT_POOL_INPUT[];
      static const char NAME_OF_ALGO_RESULT_PORT[];
    };
  }
}

#endif
