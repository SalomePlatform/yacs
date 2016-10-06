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

#ifndef __SWITCH_HXX__
#define __SWITCH_HXX__

#include "YACSlibEngineExport.hxx"
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

    class YACSLIBENGINE_EXPORT CollectorSwOutPort : public OutPort
    {
      friend class Switch;
    private:
      int edGetNumberOfOutLinks() const;
      std::set<InPort *> edSetInPort() const;
      bool isAlreadyLinkedWith(InPort *withp) const;
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

    class YACSLIBENGINE_EXPORT Switch : public StaticDefinedComposedNode
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
      Node *edGetNode(int caseId);
      Node *edSetNode(int caseId, Node *DISOWNnode) throw(Exception);
      void edChangeCase(int oldCase, int newCase);
      virtual bool edAddChild(Node *DISOWNnode) throw(Exception);
      int getMaxCase();
      void getReadyTasks(std::vector<Task *>& tasks);
      std::list<Node *> edGetDirectDescendants() const;
      InputPort *edGetConditionPort() { return &_condition; }
      void writeDot(std::ostream &os) const;
      int getNumberOfInputPorts() const;
      int getMaxLevelOfParallelism() const;
      void edRemoveChild(Node *node) throw(Exception);
      std::list<InputPort *> getSetOfInputPort() const;
      std::list<InputPort *> getLocalInputPorts() const;
      YACS::StatesForNode getEffectiveState() const;
      YACS::StatesForNode getEffectiveState(const Node* node) const;
      OutPort *getOutPort(const std::string& name) const throw(Exception);
      InputPort* getInputPort(const std::string& name) const throw(Exception);
      Node *getChildByShortName(const std::string& name) const throw(Exception);
      std::string getMyQualifiedName(const Node *directSon) const;
      std::string getCaseId(const Node *node) const throw(Exception);
      virtual void accept(Visitor *visitor);
      int getRankOfNode(Node *node) const;
      virtual std::string typeName() {return "YACS__ENGINE__Switch";}
      std::list<ProgressWeight> getProgressWeight() const;
    protected:
      YACS::Event updateStateOnFinishedEventFrom(Node *node);
      Node *simpleClone(ComposedNode *father, bool editionOnly=true) const;
      std::set<InPort *> getAllInPortsComingFromOutsideOfCurrentScope() const;
      void checkLinkPossibility(OutPort *start, const std::list<ComposedNode *>& pointsOfViewStart,
                                InPort *end, const std::list<ComposedNode *>& pointsOfViewEnd) throw(Exception);
      void buildDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView);
      void getDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView) throw(Exception);
      void releaseDelegateOf(OutPort *portDwn, OutPort *portUp, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView) throw(Exception);
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
