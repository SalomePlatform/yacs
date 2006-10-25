#ifndef __LOOP_HXX__
#define __LOOP_HXX__

#include "ComposedNode.hxx"
#include "ElementaryNode.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Loop;
    
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
      InputPort *getInputPort(const std::string& name) const throw(Exception);
      OutputDataStreamPort *getOutputDataStreamPort(const std::string& name) const throw(Exception);
      //run part
      void execute();
    public:
      ~DFToDSForLoop();
    };

    class DSToDFForLoop : public ElementaryNode
    {
      friend class Loop;
    private:
      int _nbOfTimeUsed;
      Loop *_loopArtificiallyBuiltMe;
    private:
      DSToDFForLoop(Loop *loop, const std::string& name, TypeCode* type);
      void loopHasOneMoreRef() { _nbOfTimeUsed++; }
      bool loopHasOneLessRef() { return --_nbOfTimeUsed==0; }
      OutputPort *getOutputPort(const std::string& name) const throw(Exception);
      InputDataStreamPort *getInputDataStreamPort(const std::string& name) const throw(Exception);
      //run part
      void execute();
    public:
      ~DSToDFForLoop();
    };

    class Loop : public ComposedNode
    {
    protected:
      Node *_node;
      std::set<InputPort *> _setOfExtraInputPort;
      std::set<DSToDFForLoop *> _inputsTraducer;
      std::set<DFToDSForLoop *> _outputsTraducer;
    public:
      Loop(const std::string& name);
      ~Loop();
      void edSetNode(Node *node);
      void edAddExtraInputPort(const std::string& inputPortName, TypeCode* type) throw(Exception);
      void edRemoveExtraInputPort(InputPort *inputPort);
      bool isRepeatedUnpredictablySeveralTimes() const { return true; }
      static TypeCode* MappingDF2DS(TypeCode* type) throw(Exception);
      static TypeCode* MappingDS2DF(TypeCode* type) throw(Exception);
    protected:
      InPort *buildDelegateOf(InPort *port, const std::set<ComposedNode *>& pointsOfView);
      OutPort *buildDelegateOf(OutPort *port, const std::set<ComposedNode *>& pointsOfView);
      InPort *getDelegateOf(InPort *port, const std::set<ComposedNode *>& pointsOfView) throw(Exception);
      OutPort *getDelegateOf(OutPort *port, const std::set<ComposedNode *>& pointsOfView) throw(Exception);
      InPort *releaseDelegateOf(InPort *port, const std::set<ComposedNode *>& pointsOfView) throw(Exception);
      OutPort *releaseDelegateOf(OutPort *port, const std::set<ComposedNode *>& pointsOfView) throw(Exception);
      void checkNoCyclePassingThrough(Node *node) throw(Exception);
      static bool isNecessaryToBuildSpecificDelegateDF2DS(const std::set<ComposedNode *>& pointsOfView);
    };
  }
}

#endif
