// Copyright (C) 2006-2022  CEA/DEN, EDF R&D
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

#ifndef __DYNPARALOOP_HXX__
#define __DYNPARALOOP_HXX__

#include "YACSlibEngineExport.hxx"
#include "ComposedNode.hxx"
#include "AnyInputPort.hxx"
#include "AnyOutputPort.hxx"
#include "OutputPort.hxx"
#include "NbBranches.hxx"

#include <memory>

namespace YACS
{
  namespace ENGINE
  { 
    /*!
     * \brief Base class for dynamically (fully or semifully) built graphs.
     * \ingroup Nodes
     */
    class YACSLIBENGINE_EXPORT DynParaLoop : public ComposedNode
    {
    protected:
      typedef enum
        {
          INIT_NODE = 5,
          WORK_NODE = 6,
          FINALIZE_NODE = 7
      } TypeOfNode;
    protected:
      Node *_node;
      Node *_initNode;
      Node *_finalizeNode;
      unsigned _nbOfEltConsumed;
      std::vector<int> _execIds;
      //NbBranches _nbOfBranches2;
      std::unique_ptr<NbBranchesAbstract> _nbOfBranches;
      AnyOutputPort _splittedPort;
      std::vector<Node *> _execNodes;
      std::vector<Node *> _execInitNodes;
      std::vector<Node *> _execFinalizeNodes;
      int _initializingCounter;
      int _unfinishedCounter;
      int _failedCounter;
      ComplexWeight _weight;
      double _loopWeight;
    protected:
      static const char NAME_OF_SPLITTED_SEQ_OUT[];
      static const char OLD_NAME_OF_SPLITTED_SEQ_OUT[];
    protected:
      DynParaLoop(const std::string& name, TypeCode *typeOfDataSplitted, std::unique_ptr<NbBranchesAbstract>&& branchManager);
      virtual ~DynParaLoop();
      DynParaLoop(const DynParaLoop& other, ComposedNode *father, bool editionOnly);
    public:
      Node *edRemoveNode();
      Node *edRemoveInitNode();
      Node *edRemoveFinalizeNode();
      //Node* DISOWNnode is a SWIG notation to indicate that the ownership of the node is transfered to C++
      Node *edSetNode(Node *DISOWNnode);
      Node *edSetInitNode(Node *DISOWNnode);
      Node *edSetFinalizeNode(Node *DISOWNnode);
      virtual bool edAddDFLink(OutPort *start, InPort *end) ;
      void init(bool start=true);
      InputPort *edGetNbOfBranchesPort() { return _nbOfBranches->getPort(); }
      int getNumberOfInputPorts() const;
      int getNumberOfOutputPorts() const;
      unsigned getNumberOfEltsConsumed() const { return _nbOfEltConsumed; }
      int getBranchIDOfNode(Node *node) const;
      std::list<OutputPort *> getSetOfOutputPort() const;
      std::list<OutputPort *> getLocalOutputPorts() const;
      OutputPort *edGetSamplePort() { return &_splittedPort; }
      OutPort *getOutPort(const std::string& name) const ;
      InputPort *getInputPort(const std::string& name) const ;
      OutputPort *getOutputPort(const std::string& name) const ;
      //! For the moment false is returned : impovement about it coming soon.
      bool isPlacementPredictableB4Run() const;
      void edRemoveChild(Node *node) ;
      bool isLoop() const override { return true; }
      virtual bool edAddChild(Node *DISOWNnode) ;
      std::list<Node *> edGetDirectDescendants() const;
      std::list<InputPort *> getSetOfInputPort() const;
      std::list<InputPort *> getLocalInputPorts() const;
      unsigned getNumberOfBranchesCreatedDyn() const ;
      Node *getChildByShortName(const std::string& name) const ;
      Node *getChildByNameExec(const std::string& name, unsigned id) const ;
      std::vector<Node *> getNodes() const { return _execNodes; } // need to use in GUI part for adding observers for clone nodes
      ComplexWeight * getWeight();
      void setWeight(double loopWeight);
      void getWeightRegardingDPL(ComplexWeight *weight) {weight->addWeight(getWeight());}
      bool isMultiplicitySpecified(unsigned& value) const;
      void forceMultiplicity(unsigned value);
      virtual void checkBasicConsistency() const ;
      virtual std::string getErrorReport();
      void accept(Visitor *visitor);
      Node * getInitNode();
      Node * getExecNode();
      Node * getFinalizeNode();
      int getMaxLevelOfParallelism() const;
      void partitionRegardingDPL(const PartDefinition *pd, std::map<ComposedNode *, YACS::BASES::AutoRefCnt<PartDefinition> >& zeMap);
      virtual void cleanDynGraph();
    protected:
      void buildDelegateOf(InPort * & port, OutPort *initialStart, const std::list<ComposedNode *>& pointsOfView);
      void buildDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView);
      void checkCFLinks(const std::list<OutPort *>& starts, InputPort *end, unsigned char& alreadyFed, bool direction, LinkInfo& info) const;
      void checkControlDependancy(OutPort *start, InPort *end, bool cross,
                                  std::map < ComposedNode *,  std::list < OutPort * >, SortHierarc >& fw,
                                  std::vector<OutPort *>& fwCross,
                                  std::map< ComposedNode *, std::list < OutPort *>, SortHierarc >& bw,
                                  LinkInfo& info) const;
      virtual void checkLinkPossibility(OutPort *start, const std::list<ComposedNode *>& pointsOfViewStart,
                                       InPort *end, const std::list<ComposedNode *>& pointsOfViewEnd) ;
    protected:
      void prepareInputsFromOutOfScope(int branchNb);
      void putValueOnBranch(Any *val, unsigned branchId, bool first);
      TypeOfNode getIdentityOfNotifyerNode(const Node *node, unsigned& id);
      InputPort *getDynInputPortByAbsName(int branchNb, const std::string& name, bool initNodeAdmitted);
      virtual void forwardExecStateToOriginalBody(Node *execNode);
      virtual YACS::Event updateStateOnFailedEventFrom(Node *node, const Executor *execInst);
      std::vector<Node *> cloneAndPlaceNodesCoherently(const std::vector<Node *> & origNodes);
      Node * checkConsistencyAndSetNode(Node* &nodeToReplace, Node* DISOWNnode);
      Node * removeNode(Node* &nodeToRemove);
      virtual void shutdown(int level);
    };
  }
}

#endif
