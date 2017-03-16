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

#ifndef __COMPOSEDNODE_HXX__
#define __COMPOSEDNODE_HXX__

#include "YACSlibEngineExport.hxx"
#include "Node.hxx"
#include "Scheduler.hxx"

#include <set>
#include <string>
#include <vector>

namespace YACS
{
  namespace ENGINE
  {
    class Bloc;
    class Loop;
    class InPort;
    class OutPort;
    class LinkInfo;
    class ElementaryNode;
    
    class YACSLIBENGINE_EXPORT ComposedNode : public Node, public Scheduler
    {
      friend class Bloc;
      friend class Loop;
      friend class OutPort;
      friend class ElementaryNode;
    protected:
      static const char SEP_CHAR_BTW_LEVEL[];
    protected:
      ComposedNode(const std::string& name);
      ComposedNode(const ComposedNode& other, ComposedNode *father);
      void performDuplicationOfPlacement(const Node& other);
      void performShallowDuplicationOfPlacement(const Node& other);
    public:
      virtual ~ComposedNode();
      bool isFinished();
      void init(bool start=true);
      virtual void shutdown(int level);
      virtual void resetState(int level);
      std::string getName() const;
      std::string getTaskName(Task *task) const;
      DeploymentTree getDeploymentTree() const;
      DeploymentTree checkDeploymentTree(bool deep) const throw(Exception);
      std::vector<Task *> getNextTasks(bool& isMore);
      virtual bool isPlacementPredictableB4Run() const = 0;
      void notifyFrom(const Task *sender, YACS::Event event, const Executor *execInst);
      bool edAddLink(OutPort *start, InPort *end) throw(Exception);
      virtual bool edAddDFLink(OutPort *start, InPort *end) throw(Exception);
      //Node* DISOWNnode is a SWIG notation to indicate that the ownership of the node is transfered to C++
      virtual bool edAddChild(Node *DISOWNnode) throw(Exception);
      virtual void edRemoveChild(Node *node) throw(Exception);
      bool edAddLink(OutGate *start, InGate *end) throw(Exception);
      bool edAddCFLink(Node *nodeS, Node *nodeE) throw(Exception);
      void edRemoveCFLink(Node *nodeS, Node *nodeE) throw(Exception);
      void edRemoveLink(OutPort *start, InPort *end) throw(Exception);
      void edRemoveLink(OutGate *start, InGate *end) throw(Exception);
      virtual bool isRepeatedUnpredictablySeveralTimes() const { return false; }
      virtual std::list<Node *> edGetDirectDescendants() const =  0;
      virtual void removeRecursivelyRedundantCL();
      std::list<ElementaryNode *> getRecursiveConstituents() const;
      std::list<Node *> getAllRecursiveNodes();
      virtual std::list<Node *> getAllRecursiveConstituents(); // first implementation
      std::list<ProgressWeight> getProgressWeight() const;
      std::string getInPortName(const InPort *) const throw (Exception);
      std::string getOutPortName(const OutPort *) const throw (Exception);
      //
      int getNumberOfInputPorts() const;
      int getNumberOfOutputPorts() const;
      std::list<InputPort *> getSetOfInputPort() const;
      std::list<OutputPort *> getSetOfOutputPort() const;
      std::list<InputPort *> getLocalInputPorts() const;
      std::list<OutputPort *> getLocalOutputPorts() const;
      std::set<OutPort *> getAllOutPortsLeavingCurrentScope() const;
      std::set<InPort *> getAllInPortsComingFromOutsideOfCurrentScope() const;
      std::list<InputDataStreamPort *> getSetOfInputDataStreamPort() const;
      std::list<OutputDataStreamPort *> getSetOfOutputDataStreamPort() const;
      OutPort *getOutPort(const std::string& name) const throw(Exception);
      InputPort *getInputPort(const std::string& name) const throw(Exception);
      OutputPort *getOutputPort(const std::string& name) const throw(Exception);
      InputDataStreamPort *getInputDataStreamPort(const std::string& name) const throw(Exception);
      OutputDataStreamPort *getOutputDataStreamPort(const std::string& name) const throw(Exception);
      std::vector< std::pair<OutPort *, InPort *> > getSetOfInternalLinks() const;
      virtual std::vector< std::pair<OutPort *, InPort *> > getSetOfLinksLeavingCurrentScope() const;
      void checkConsistency(LinkInfo& info) const throw(Exception);
      virtual std::vector< std::pair<InPort *, OutPort *> > getSetOfLinksComingInCurrentScope() const;
      virtual std::string typeName() {return "YACS__ENGINE__ComposedNode";}
      virtual void edUpdateState();
      virtual void checkBasicConsistency() const throw(Exception);
      virtual std::string getErrorReport();
      //
      ComposedNode *getRootNode() const throw(Exception);
      bool isNodeAlreadyAggregated(const Node *node) const;
      virtual bool isNameAlreadyUsed(const std::string& name) const;
      Node *isInMyDescendance(Node *nodeToTest) const;
      std::string getChildName(const Node* node) const throw(Exception);
      virtual std::string getMyQualifiedName(const Node *directSon) const;
      Node *getChildByName(const std::string& name) const throw(Exception);
      static ComposedNode *getLowestCommonAncestor(Node *node1, Node *node2) throw(Exception);
      static std::string getLowestCommonAncestorStr(const std::string& node1, const std::string& node2);
      void loaded();
      void connected();
      void accept(Visitor *visitor);
      virtual void cleanNodes();
      virtual std::string getProgress() const { return "0"; }
    protected:
      struct SortHierarc
      {
        bool operator()(ComposedNode *n1, ComposedNode *n2) const
        {
          return *n1<*n2;
        }
      };
    protected:
      void edDisconnectAllLinksWithMe();
      static bool splitNamesBySep(const std::string& globalName, const char separator[],
                                  std::string& firstPart, std::string& lastPart, bool priority) throw(Exception);
      virtual Node *getChildByShortName(const std::string& name) const throw(Exception) = 0;
      YACS::Event updateStateFrom(Node *node, YACS::Event event, const Executor *execInst);//update the state of this. Precondition : node->_father == this
      virtual YACS::Event updateStateOnStartEventFrom(Node *node);//transition 3 doc P.R
      virtual YACS::Event updateStateOnFinishedEventFrom(Node *node) = 0;//transition 9 doc P.R.
      virtual YACS::Event updateStateOnFailedEventFrom(Node *node, const Executor *execInst);//transition 9 doc P.R.
      virtual void checkLinkPossibility(OutPort *start, const std::list<ComposedNode *>& pointsOfViewStart,
                                        InPort *end, const std::list<ComposedNode *>& pointsOfViewEnd) throw(Exception);
      virtual void buildDelegateOf(InPort * & port, OutPort *initialStart, const std::list<ComposedNode *>& pointsOfView);
      virtual void buildDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView);
      virtual void getDelegateOf(InPort * & port, OutPort *initialStart, const std::list<ComposedNode *>& pointsOfView) throw(Exception);
      virtual void getDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView) throw(Exception);
      virtual void releaseDelegateOf(InPort * & port, OutPort *initialStart, const std::list<ComposedNode *>& pointsOfView) throw(Exception);
      virtual void releaseDelegateOf(OutPort *portDwn, OutPort *portUp, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView) throw(Exception);
      virtual void checkNoCyclePassingThrough(Node *node) throw(Exception) = 0;
      void checkInMyDescendance(Node *nodeToTest) const throw(Exception);
      template <class PORT>
      std::string getPortName(const PORT * port) const throw (Exception);
      //For CF Computations
      void checkNoCrossHierachyWith(Node *node) const throw (Exception);
      virtual void performCFComputations(LinkInfo& info) const;
      virtual void destructCFComputations(LinkInfo& info) const;
      Node *getLowestNodeDealingAll(const std::list<OutPort *>& ports) const;
      void checkLinksCoherenceRegardingControl(const std::vector<OutPort *>& starts,
                                               InputPort *end, LinkInfo& info) const throw(Exception);
      virtual void checkControlDependancy(OutPort *start, InPort *end, bool cross,
                                          std::map < ComposedNode *,  std::list < OutPort * >, SortHierarc >& fw,
                                          std::vector<OutPort *>& fwCross,
                                          std::map< ComposedNode *, std::list < OutPort *>, SortHierarc >& bw,
                                          LinkInfo& info) const = 0;
      void solveObviousOrDelegateCFLinks(const std::list<OutPort *>& starts, InputPort *end, unsigned char& alreadyFed, bool direction, LinkInfo& info) const;
      virtual void checkCFLinks(const std::list<OutPort *>& starts, InputPort *end, unsigned char& alreadyFed, bool direction, LinkInfo& info) const;
    protected:
      //For internal calculations.
      static const unsigned char FED_ST = 2;
      static const unsigned char FREE_ST = 0;
      static const unsigned char FED_DS_ST = 1;
    };

    template <class PORT>
    std::string ComposedNode::getPortName(const PORT * port) const throw (Exception)
    {
      Node *node = port->getNode();
      std::string portName = port->getName();  
      checkInMyDescendance(node);
      Node *father = node;
      while (father != this)
        {
          portName = father->getQualifiedName() + Node::SEP_CHAR_IN_PORT + portName;
          father = father->_father;
        }
      return portName;
    }
  }
}

#endif
