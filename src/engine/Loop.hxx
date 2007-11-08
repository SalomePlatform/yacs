#ifndef __LOOP_HXX__
#define __LOOP_HXX__

#include "StaticDefinedComposedNode.hxx"
#include "InputDataStreamPort.hxx"
#include "ElementaryNode.hxx"
#include "OutputPort.hxx"
#include "InputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Loop;
    class ForLoop;
    class WhileLoop;
    class DFToDSForLoop;
    class DSToDFForLoop;

    class InputPort4DF2DS : public InputPort
    {
    public:
      InputPort4DF2DS(DFToDSForLoop *node, TypeCode* type);
      void getAllRepresentants(std::set<InPort *>& repr) const;
      void put(const void *data) throw(ConversionException);
      InputPort *clone(Node *newHelder) const;
      void *get() const throw(Exception);
      void exRestoreInit();
      void exSaveInit();
      void put(Any *data);
      ~InputPort4DF2DS();
    protected:
      Any *_data;
    };

    class DFToDSForLoop : public ElementaryNode
    {
      friend class Loop;
    private:
      int _nbOfTimeUsed;
      Loop *_loopArtificiallyBuiltMe;
    private:
      DFToDSForLoop(Loop *loop, const std::string& name, TypeCode* type);
      void loopHasOneMoreRef() { _nbOfTimeUsed++; }
      bool loopHasOneLessRef() { return --_nbOfTimeUsed==0; }
      void getReadyTasks(std::vector<Task *>& tasks);
      InputPort *getInputPort(const std::string& name) const throw(Exception);
      OutputDataStreamPort *getOutputDataStreamPort(const std::string& name) const throw(Exception);
      Node *simpleClone(ComposedNode *father, bool editionOnly=true) const;
      //run part
      void execute();
      void load();
    public:
      ~DFToDSForLoop();
    };

    class OutputPort4DS2DF : public OutputPort
    {
    public:
      OutputPort4DS2DF(DSToDFForLoop *node, TypeCode *type);
      void getAllRepresented(std::set<OutPort *>& represented) const;
      void put(const void *data) throw(ConversionException);
      OutputPort *clone(Node *newHelder) const;
      void put(Any *data);
      ~OutputPort4DS2DF();
    protected:
      Any *_data;
    };
    
    class InputDataStreamPort4DS2DF : public InputDataStreamPort
    {
    public:
      InputDataStreamPort4DS2DF(DSToDFForLoop *node, TypeCode* type);
      void getAllRepresentants(std::set<InPort *>& repr) const;
    };

    class DSToDFForLoop : public ElementaryNode
    {
      friend class Loop;
    private:
      int _nbOfTimeUsed;
      Loop *_loopArtificiallyBuiltMe;
    private:
      DSToDFForLoop(Loop *loop, const std::string& name, TypeCode* type);
      Node *simpleClone(ComposedNode *father, bool editionOnly=true) const;
      void loopHasOneMoreRef() { _nbOfTimeUsed++; }
      bool loopHasOneLessRef() { return --_nbOfTimeUsed==0; }
      void getReadyTasks(std::vector<Task *>& tasks);
      OutputPort *getOutputPort(const std::string& name) const throw(Exception);
      InputDataStreamPort *getInputDataStreamPort(const std::string& name) const throw(Exception);
      //run part
      void execute();
      void load();
    public:
      ~DSToDFForLoop();
    };

    class FakeNodeForLoop : public ElementaryNode
    {
      friend class ForLoop;
      friend class WhileLoop;
    private:
      Loop *_loop;
      bool _normalFinish;
      bool _internalError;
    private:
      FakeNodeForLoop(Loop *loop, bool normalFinish, bool internalError=false);
      FakeNodeForLoop(const FakeNodeForLoop& other);
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
      void exForwardFailed();
      void exForwardFinished();
      void execute();
      void aborted();
      void finished();
    };

/*! \brief Base class for loop node
 *
 * \ingroup Nodes
 *
 * \see ForLoop
 * \see WhileLoop
 */
    class Loop : public StaticDefinedComposedNode
    {
      friend class DSToDFForLoop;
      friend class FakeNodeForLoop;
      friend void NbDoneLoader(Loop* node, int val);
    protected:
      Node *_node;
      int _nbOfTurns;
      FakeNodeForLoop *_nodeForNullTurnOfLoops;
      std::set<DSToDFForLoop *> _inputsTraducer;
      std::set<DFToDSForLoop *> _outputsTraducer;
    public:
      Loop(const Loop& other, ComposedNode *father, bool editionOnly);
      Loop(const std::string& name);
      ~Loop();
      void init(bool start=true);
      int getNbOfTurns() const { return _nbOfTurns; }
      //Node* DISOWNnode is a SWIG notation to indicate that the ownership of the node is transfered to C++
      Node *edSetNode(Node *DISOWNnode);
      Node *edRemoveNode();
      //! Returns the port which value is used to take decision about the continuation of the loop.
      virtual InputPort *getDecisionPort() const = 0;
      void getReadyTasks(std::vector<Task *>& tasks);
      void edRemoveChild(Node *node) throw(Exception);
      bool isRepeatedUnpredictablySeveralTimes() const { return true; }
      void selectRunnableTasks(std::vector<Task *>& tasks);
      std::set<Node *> edGetDirectDescendants() const;
      std::list<InputPort *> getSetOfInputPort() const;
      int getNumberOfInputPorts() const;
      Node *getChildByShortName(const std::string& name) const throw(Exception);
      static TypeCode* MappingDF2DS(TypeCode* type) throw(Exception);
      static TypeCode* MappingDS2DF(TypeCode* type) throw(Exception);
      virtual bool edAddDFLink(OutPort *start, InPort *end) throw(Exception);
      void writeDot(std::ostream &os);
      virtual void accept(Visitor *visitor);
    protected:
      void buildDelegateOf(InPort * & port, OutPort *initialStart, const std::set<ComposedNode *>& pointsOfView);
      void buildDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::set<ComposedNode *>& pointsOfView);
      void getDelegateOf(InPort * & port, OutPort *initialStart, const std::set<ComposedNode *>& pointsOfView) throw(Exception);
      void getDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::set<ComposedNode *>& pointsOfView) throw(Exception);
      void releaseDelegateOf(InPort * & port, OutPort *initialStart, const std::set<ComposedNode *>& pointsOfView) throw(Exception);
      void releaseDelegateOf(OutPort *portDwn, OutPort *portUp, InPort *finalTarget, const std::set<ComposedNode *>& pointsOfView) throw(Exception);
      void checkNoCyclePassingThrough(Node *node) throw(Exception);
      void checkControlDependancy(OutPort *start, InPort *end, bool cross,
                                  std::map < ComposedNode *,  std::list < OutPort * >, SortHierarc >& fw,
                                  std::vector<OutPort *>& fwCross,
                                  std::map< ComposedNode *, std::list < OutPort *>, SortHierarc >& bw,
                                  LinkInfo& info) const;
      void checkCFLinks(const std::list<OutPort *>& starts, InputPort *end, unsigned char& alreadyFed, bool direction, LinkInfo& info) const;
      static bool isNecessaryToBuildSpecificDelegateDF2DS(const std::set<ComposedNode *>& pointsOfView);
    };

    void NbDoneLoader(Loop* node, int val);
  }
}

#endif
