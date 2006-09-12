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
      DFToDSForLoop(Loop *loop, const std::string& name, YACS::DynType type);
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
      DSToDFForLoop(Loop *loop, const std::string& name, YACS::StreamType type);
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
      std::list<InputPort *> _listOfExtraInputPort;
      std::list<DSToDFForLoop> _inputsTraducer;
      std::list<DFToDSForLoop> _outputsTraducer;
    public:
      Loop(const std::string& name);
      ~Loop();
      void edSetNode(Node *node);
      void edAddExtraInputPort(const std::string& inputPortName, YACS::DynType type) throw(Exception);
      void edRemoveExtraInputPort(InputPort *inputPort);
      bool isRepeatedUnpredictablySeveralTimes() const { return true; }
      static YACS::StreamType MappingDF2DS(YACS::DynType type) throw(Exception);
      static YACS::DynType MappingDS2DF(YACS::StreamType type) throw(Exception);
    protected:
      InPort *buildDelegateOf(InPort *port, const std::list<ComposedNode *>& pointsOfView);
      OutPort *buildDelegateOf(OutPort *port, const std::list<ComposedNode *>& pointsOfView);
      InPort *getDelegateOf(InPort *port, const std::list<ComposedNode *>& pointsOfView) throw(Exception);
      OutPort *getDelegateOf(OutPort *port, const std::list<ComposedNode *>& pointsOfView) throw(Exception);
      InPort *releaseDelegateOf(InPort *port, const std::list<ComposedNode *>& pointsOfView) throw(Exception);
      OutPort *releaseDelegateOf(OutPort *port, const std::list<ComposedNode *>& pointsOfView) throw(Exception);
      void checkNoCyclePassingThrough(Node *node) throw(Exception);
      static bool isNecessaryToBuildSpecificDelegateDF2DS(const std::list<ComposedNode *>& pointsOfView);
    };
  }
}

#endif
