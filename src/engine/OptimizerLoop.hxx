#ifndef __OPTIMIZERLOOP_HXX__
#define __OPTIMIZERLOOP_HXX__

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
    class OptimizerAlgStandardized : public OptimizerAlgSync
    {
    private:
      ::YACS::BASES::Thread *_threadInCaseOfNotEvent;
      ::YACS::BASES::DrivenCondition _condition;
      OptimizerAlgBase *_algBehind;
    public:
      OptimizerAlgStandardized(Pool *pool, OptimizerAlgBase *alg);
      ~OptimizerAlgStandardized();
      TypeCode *getTCForIn() const;
      TypeCode *getTCForOut() const;
      void setAlgPointer(OptimizerAlgBaseFactory algFactory);
      void parseFileToInit(const std::string& fileName);
      void initialize(const Any *input) throw (Exception);
      void takeDecision();
      void finish();
      void start();
    private:
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

    class OptimizerLoop : public DynParaLoop
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
                    bool algInitOnFile) throw(Exception);
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
    protected:
      void buildDelegateOf(InPort * & port, OutPort *initialStart, const std::set<ComposedNode *>& pointsOfView);
      void buildDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::set<ComposedNode *>& pointsOfView);
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
      static const char NAME_OF_FILENAME_INPUT[];
      static const char NAME_OF_OUT_POOL_INPUT[];
    };
  }
}

#endif
