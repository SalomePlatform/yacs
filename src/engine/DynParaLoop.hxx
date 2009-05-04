//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef __DYNPARALOOP_HXX__
#define __DYNPARALOOP_HXX__

#include "ComposedNode.hxx"
#include "AnyInputPort.hxx"
#include "AnyOutputPort.hxx"
#include "OutputPort.hxx"

namespace YACS
{
  namespace ENGINE
  { 
    /*!
     * \brief Base class for dynamically (fully or semifully) built graphs.
     */
    class DynParaLoop : public ComposedNode
    {
    protected:
      typedef enum
        {
          INIT_NODE = 5,
          WORK_NODE = 6
      } TypeOfNode;
    protected:
      Node *_node;
      Node *_initNode;
      unsigned _nbOfEltConsumed;
      std::vector<int> _execIds;
      AnyInputPort _nbOfBranches;
      AnyOutputPort _splittedPort;
      std::vector<Node *> _execNodes;
      std::vector<Node *> _execInitNodes;
    protected:
      static const char NAME_OF_SPLITTED_SEQ_OUT[];
      static const char NAME_OF_NUMBER_OF_BRANCHES[];
    protected:
      DynParaLoop(const std::string& name, TypeCode *typeOfDataSplitted);
      virtual ~DynParaLoop();
      DynParaLoop(const DynParaLoop& other, ComposedNode *father, bool editionOnly);
    public:
      Node *edRemoveNode();
      Node *edRemoveInitNode();
      //Node* DISOWNnode is a SWIG notation to indicate that the ownership of the node is transfered to C++
      Node *edSetNode(Node *DISOWNnode);
      Node *edSetInitNode(Node *DISOWNnode);
      void init(bool start=true);
      InputPort *edGetNbOfBranchesPort() { return &_nbOfBranches; }
      int getNumberOfInputPorts() const;
      int getNumberOfOutputPorts() const;
      unsigned getNumberOfEltsConsumed() const { return _nbOfEltConsumed; }
      std::list<OutputPort *> getSetOfOutputPort() const;
      OutputPort *edGetSamplePort() { return &_splittedPort; }
      OutPort *getOutPort(const std::string& name) const throw(Exception);
      InputPort *getInputPort(const std::string& name) const throw(Exception);
      OutputPort *getOutputPort(const std::string& name) const throw(Exception);
      //! For the moment false is returned : impovement about it coming soon.
      bool isPlacementPredictableB4Run() const;
      void edRemoveChild(Node *node) throw(Exception);
      std::list<Node *> edGetDirectDescendants() const;
      std::list<InputPort *> getSetOfInputPort() const;
      std::list<InputPort *> getLocalInputPorts() const;
      unsigned getNumberOfBranchesCreatedDyn() const throw(Exception);
      Node *getChildByShortName(const std::string& name) const throw(Exception);
      Node *getChildByNameExec(const std::string& name, unsigned id) const throw(Exception);
      std::vector<Node *> getNodes() const { return _execNodes; } // need to use in GUI part for adding observers for clone nodes
      bool isMultiplicitySpecified(unsigned& value) const;
      void forceMultiplicity(unsigned value);
      virtual void checkBasicConsistency() const throw(Exception);
    protected:
      void buildDelegateOf(InPort * & port, OutPort *initialStart, const std::list<ComposedNode *>& pointsOfView);
      void buildDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView);
      void checkCFLinks(const std::list<OutPort *>& starts, InputPort *end, unsigned char& alreadyFed, bool direction, LinkInfo& info) const;
      void checkControlDependancy(OutPort *start, InPort *end, bool cross,
                                  std::map < ComposedNode *,  std::list < OutPort * >, SortHierarc >& fw,
                                  std::vector<OutPort *>& fwCross,
                                  std::map< ComposedNode *, std::list < OutPort *>, SortHierarc >& bw,
                                  LinkInfo& info) const;
    protected:
      void cleanDynGraph();
      void prepareInputsFromOutOfScope(int branchNb);
      void putValueOnBranch(Any *val, unsigned branchId, bool first);
      TypeOfNode getIdentityOfNotifyerNode(const Node *node, unsigned& id);
      InputPort *getDynInputPortByAbsName(int branchNb, const std::string& name, bool initNodeAdmitted);
    };
  }
}

#endif
