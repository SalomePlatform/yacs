#ifndef __SWITCH_HXX__
#define __SWITCH_HXX__

#include "StaticDefinedComposedNode.hxx"
#include "ElementaryNode.hxx"
#include "AnyInputPort.hxx"
#include "OutPort.hxx"

#include <map>

namespace YACS
{
  namespace ENGINE
  {
    class Switch;

    class CollectorSwOutPort : public OutPort
    {
      friend class Switch;
    private:
      int edGetNumberOfOutLinks() const;
      std::set<InPort *> edSetInPort() const;
      bool isAlreadyLinkedWith(InPort *with) const;
      std::string getNameOfTypeOfCurrentInstance() const;
      void edRemoveAllLinksLinkedWithMe() throw(Exception);
      TypeOfChannel getTypeOfChannel() const;
      void getAllRepresented(std::set<OutPort *>& represented) const;
      bool addInPort(InPort *inPort) throw(Exception);
      int removeInPort(InPort *inPort, bool forward) throw(Exception);
    public:
      void getHumanReprOfIncompleteCases(std::ostream& stream) const;
    private://Specific part
      bool removePotentialProducerForMaster();
      void checkConsistency(LinkInfo& info) const;
      CollectorSwOutPort(Switch *master, InPort *port);
      CollectorSwOutPort(const CollectorSwOutPort& other, Switch *master);
      void addPotentialProducerForMaster(OutPort *port);
      bool checkManagementOfPort(OutPort *port) throw(Exception);
    private:
      InPort *_consumer;
      std::string _className;
      OutPort *_currentProducer;
      std::map<int, OutPort *> _potentialProducers;
    };

    class FakeNodeForSwitch : public ElementaryNode
    {
      friend class Switch;
    private:
      Switch *_sw;
      bool _normalFinish;
      bool _internalError;
    private:
      FakeNodeForSwitch(Switch *sw, bool normalFinish, bool internalError=false);
      FakeNodeForSwitch(const FakeNodeForSwitch& other);
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
      void exForwardFailed();
      void exForwardFinished();
      void execute();
      void aborted();
      void finished();
    };

    class Switch : public StaticDefinedComposedNode
    {
      friend class FakeNodeForSwitch;
      friend class CollectorSwOutPort;
    public:
      static const char DEFAULT_NODE_NAME[];
      static const char SELECTOR_INPUTPORT_NAME[];
      static const int ID_FOR_DEFAULT_NODE;
    protected:
      AnyInputPort _condition;
      std::map< int , Node * > _mapOfNode;//Nodes ownered
      FakeNodeForSwitch *_undispatchableNotificationNode;
      std::map<InPort *, CollectorSwOutPort * > _outPortsCollector;
      mutable std::vector<CollectorSwOutPort *> _alreadyExistingCollectors;
    public:
      Switch(const Switch& other, ComposedNode *father, bool editionOnly);
      Switch(const std::string& name);
      ~Switch();
      void exUpdateState();
      void init(bool start=true);
      //Node* DISOWNnode is a SWIG notation to indicate that the ownership of the node is transfered to C++
      Node *edSetDefaultNode(Node *DISOWNnode);
      Node *edReleaseDefaultNode() throw(Exception);
      Node *edReleaseCase(int caseId) throw(Exception);
      Node *edSetNode(int caseId, Node *DISOWNnode) throw(Exception);
      void getReadyTasks(std::vector<Task *>& tasks);
      void selectRunnableTasks(std::vector<Task *>& tasks);
      std::set<Node *> edGetDirectDescendants() const;
      InputPort *edGetConditionPort() { return &_condition; }
      void writeDot(std::ostream &os);
      int getNumberOfInputPorts() const;
      void edRemoveChild(Node *node) throw(Exception);
      std::list<InputPort *> getSetOfInputPort() const;
      YACS::StatesForNode getEffectiveState();
      YACS::StatesForNode getEffectiveState(Node* node);
      OutPort *getOutPort(const std::string& name) const throw(Exception);
      InputPort* getInputPort(const std::string& name) const throw(Exception);
      Node *getChildByShortName(const std::string& name) const throw(Exception);
      std::string getMyQualifiedName(const Node *directSon) const;
      std::string getCaseId(const Node *node) const throw(Exception);
      virtual void accept(Visitor *visitor);
      int getRankOfNode(Node *node) const;
    protected:
      YACS::Event updateStateOnFinishedEventFrom(Node *node);
      Node *simpleClone(ComposedNode *father, bool editionOnly=true) const;
      std::set<InPort *> getAllInPortsComingFromOutsideOfCurrentScope() const;
      std::vector< std::pair<InPort *, OutPort *> > getSetOfLinksComingInCurrentScope() const;
      void checkLinkPossibility(OutPort *start, const std::set<ComposedNode *>& pointsOfViewStart,
                                InPort *end, const std::set<ComposedNode *>& pointsOfViewEnd) throw(Exception);
      void buildDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::set<ComposedNode *>& pointsOfView);
      void getDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::set<ComposedNode *>& pointsOfView) throw(Exception);
      void releaseDelegateOf(OutPort *portDwn, OutPort *portUp, InPort *finalTarget, const std::set<ComposedNode *>& pointsOfView) throw(Exception);
      void checkCFLinks(const std::list<OutPort *>& starts, InputPort *end, unsigned char& alreadyFed, bool direction, LinkInfo& info) const;
      void checkControlDependancy(OutPort *start, InPort *end, bool cross,
                                  std::map < ComposedNode *,  std::list < OutPort * > >& fw,
                                  std::vector<OutPort *>& fwCross,
                                  std::map< ComposedNode *, std::list < OutPort *> >& bw,
                                  LinkInfo& info) const;
      void checkNoCyclePassingThrough(Node *node) throw(Exception);
    private:
      int getNbOfCases() const;
      static std::string getRepresentationOfCase(int i);
    };
  }
}

#endif
