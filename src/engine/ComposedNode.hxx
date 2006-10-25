#ifndef __COMPOSEDNODE_HXX__
#define __COMPOSEDNODE_HXX__

#include "Node.hxx"
#include "Scheduler.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Bloc;
    class InPort;
    class OutPort;
    class ElementaryNode;
    
    class ComposedNode : public Node, public Scheduler
    {
      friend class Bloc;
      friend class ElementaryNode;
    protected:
      ComposedNode(const std::string& name);
    public:
      void notifyFrom(const Task *sender, YACS::Event event);
      bool edAddLink(OutputPort *start, InputPort *end) throw(Exception);
      bool edAddLink(OutGate *start, InGate *end) throw(Exception);
      bool edAddCFLink(Node *nodeS, Node *nodeE) throw(Exception);
      void edRemoveLink(OutputPort *start, InputPort *end) throw(Exception);
      void edRemoveLink(OutGate *start, InGate *end) throw(Exception);
      virtual void publishOutputPort(OutputPort *port) throw(Exception);
      virtual bool isRepeatedUnpredictablySeveralTimes() const { return false; }
      virtual const std::string getInputPortName(const InputPort *) throw (Exception);
      virtual const std::string getOutputPortName(const OutputPort *) throw (Exception);
    protected:
      ComposedNode *getRootNode() throw(Exception);
      void disconnectAllLinksConnectedTo(Node *node);
      virtual void publishInputPort(InputPort *port);
      virtual void unpublishInputPort(InputPort *port);
      YACS::Event updateStateFrom(Node *node, YACS::Event event);//update the state of this. Precondition : node->_father == this
      virtual YACS::Event updateStateOnStartEventFrom(Node *node) = 0;//transition 3 doc P.R
      virtual YACS::Event updateStateOnFinishedEventFrom(Node *node) = 0;//transition 9 doc P.R.
      virtual InPort *buildDelegateOf(InPort *port, const std::set<ComposedNode *>& pointsOfView);
      virtual OutPort *buildDelegateOf(OutPort *port, const std::set<ComposedNode *>& pointsOfView);
      virtual InPort *getDelegateOf(InPort *port, const std::set<ComposedNode *>& pointsOfView) throw(Exception);
      virtual OutPort *getDelegateOf(OutPort *port, const std::set<ComposedNode *>& pointsOfView) throw(Exception);
      virtual InPort *releaseDelegateOf(InPort *port, const std::set<ComposedNode *>& pointsOfView) throw(Exception);
      virtual OutPort *releaseDelegateOf(OutPort *port, const std::set<ComposedNode *>& pointsOfView) throw(Exception);
      virtual void checkNoCyclePassingThrough(Node *node) throw(Exception) = 0;
      void checkInMyDescendance(Node *nodeToTest) const throw(Exception);
      static ComposedNode *getLowestCommonAncestor(Node *node1, Node *node2) throw(Exception);
    };
  }
}

#endif
