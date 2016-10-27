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

#ifndef __FOREACHLOOP_HXX__
#define __FOREACHLOOP_HXX__

#include "YACSlibEngineExport.hxx"
#include "ElementaryNode.hxx"
#include "DynParaLoop.hxx"
#include "OutputPort.hxx"
#include "InputPort.hxx"
#include "AnyInputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class ForEachLoop;
    class SplitterNode;
    class AnySplitOutputPort;
    class TypeCode;
    class TypeCodeSeq;

    class InterceptorInputPort : public AnyInputPort
    {
      friend class ForEachLoop;
      friend class SplitterNode;
    private:
      AnySplitOutputPort *_repr;
    private:
      InterceptorInputPort(const std::string& name, Node *node, TypeCode* type);
      InterceptorInputPort(const InterceptorInputPort& other, Node *newHelder);
      void getAllRepresentants(std::set<InPort *>& repr) const;
      InputPort *clone(Node *newHelder) const;
      void setRepr(AnySplitOutputPort *repr);
    };

    class AnySplitOutputPort : public OutputPort
    {
      friend class ForEachLoop;
      friend class SplitterNode;
    private:
      OutPort *_repr;
      InterceptorInputPort *_intercptr;
      mutable unsigned int _cnt;
    private:
      bool decrRef();
      void incrRef() const;
      AnySplitOutputPort(const std::string& name, Node *node, TypeCode *type);
      AnySplitOutputPort(const AnySplitOutputPort& other, Node *newHelder);
      bool addInPort(InPort *inPort) throw(Exception);
      void getAllRepresented(std::set<OutPort *>& represented) const;
      int removeInPort(InPort *inPort, bool forward) throw(Exception);
      void addRepr(OutPort *repr, InterceptorInputPort *intercptr);
      OutPort *getRepr() const { return _repr; }
      OutputPort *clone(Node *newHelder) const;
    };

    class YACSLIBENGINE_EXPORT SeqAnyInputPort : public AnyInputPort
    {
      friend class ForEachLoop;
      friend class SplitterNode;
    public:
      unsigned getNumberOfElements() const;
      virtual std::string dump();
    private:
      SeqAnyInputPort(const std::string& name, Node *node, TypeCodeSeq* type);
      SeqAnyInputPort(const SeqAnyInputPort& other, Node *newHelder);
      InputPort *clone(Node *newHelder) const;
      Any *getValueAtRank(int i) const;
    };

    class SplitterNode : public ElementaryNode
    {
      friend class ForEachLoop;
    private:
      static const char NAME_OF_SEQUENCE_INPUT[];
    private:
      SplitterNode(const std::string& name, TypeCode *typeOfData, ForEachLoop *father);
      SplitterNode(const SplitterNode& other, ForEachLoop *father);
      InputPort *getInputPort(const std::string& name) const throw(Exception);
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
      unsigned getNumberOfElements() const;
      void execute();
      void init(bool start=true);
      void putSplittedValueOnRankTo(int rankInSeq, int branch, bool first);
    private:
      SeqAnyInputPort _dataPortToDispatch;
    };

    class FakeNodeForForEachLoop : public ElementaryNode
    {
      friend class ForEachLoop;
    private:
      ForEachLoop *_loop;
      bool _normalFinish;
    private:
      FakeNodeForForEachLoop(ForEachLoop *loop, bool normalFinish);
      FakeNodeForForEachLoop(const FakeNodeForForEachLoop& other);
      Node *simpleClone(ComposedNode *father, bool editionOnly) const;
      void exForwardFailed();
      void exForwardFinished();
      void execute();
      void aborted();
      void finished();
    private:
      static const char NAME[];
    };

    class YACSLIBENGINE_EXPORT ForEachLoopPassedData
    {
    public:
      ForEachLoopPassedData(const std::vector<unsigned int>& passedIds, const std::vector<SequenceAny *>& passedOutputs, const std::vector<std::string>& nameOfOutputs);
      ~ForEachLoopPassedData();
      void init();
      void checkCompatibilyWithNb(int nbOfElts) const;
      void checkLevel2(const std::vector<AnyInputPort *>& ports) const;
      int getNumberOfEltsAlreadyDone() const { return (int)_passedIds.size(); }
      int toAbsId(int localId) const;
      int toAbsIdNot(int localId) const;
      int getNumberOfElementsToDo() const;
      void assignAlreadyDone(const std::vector<SequenceAny *>& execVals) const;
    private:
      std::vector<unsigned int> _passedIds;
      std::vector<SequenceAny *> _passedOutputs;
      std::vector<std::string> _nameOfOutputs;
      mutable std::vector<bool> _flagsIds;
    };

    class Executor;

    class YACSLIBENGINE_EXPORT ForEachLoop : public DynParaLoop
    {
      friend class SplitterNode;
      friend class FakeNodeForForEachLoop;

    public:
      static const char NAME_OF_SPLITTERNODE[];
    protected:
      static const int NOT_RUNNING_BRANCH_ID;
    protected:
      int _currentIndex;
      SplitterNode _splitterNode;
      FakeNodeForForEachLoop *_nodeForSpecialCases;
      std::vector<AnySplitOutputPort *> _outGoingPorts;//! ports linked to node outside the current scope
      std::vector<InterceptorInputPort *> _intecptrsForOutGoingPorts;//!ports created for TypeCodes correctness
      //part of attributes defining graph dynamically built on control notification
      unsigned _execCurrentId;
      std::vector<SequenceAny *> _execVals;
      std::vector< std::vector<AnyInputPort *> > _execOutGoingPorts;
      ForEachLoopPassedData *_passedData;
    public:
      ForEachLoop(const std::string& name, TypeCode *typeOfDataSplitted);
      ForEachLoop(const ForEachLoop& other, ComposedNode *father, bool editionOnly);
      ~ForEachLoop();
      void init(bool start=true);
      void exUpdateState();
      void exUpdateProgress();
      void getReadyTasks(std::vector<Task *>& tasks);
      int getNumberOfInputPorts() const;
      //
      void checkNoCyclePassingThrough(Node *node) throw(Exception);
      void selectRunnableTasks(std::vector<Task *>& tasks);
      //
      unsigned getExecCurrentId() const { return _execCurrentId; } // for update progress bar on GUI part
      std::list<InputPort *> getSetOfInputPort() const;
      std::list<InputPort *> getLocalInputPorts() const;
      InputPort *edGetSeqOfSamplesPort() { return &_splitterNode._dataPortToDispatch; }
      InputPort *getInputPort(const std::string& name) const throw(Exception);
      OutPort *getOutPort(const std::string& name) const throw(Exception);
      OutputPort *getOutputPort(const std::string& name) const throw(Exception);
      Node *getChildByShortName(const std::string& name) const throw(Exception);
      std::list<OutputPort *> getLocalOutputPorts() const;
      void accept(Visitor *visitor);
      void writeDot(std::ostream &os) const;
      virtual std::string typeName() {return "YACS__ENGINE__ForEachLoop";}
      virtual void resetState(int level);
      std::string getProgress() const;
      std::list<ProgressWeight> getProgressWeight() const;
      int getCurrentIndex() const { return _currentIndex; }
      int getNbOfElementsToBeProcessed() const;
      static int getFEDeltaBetween(OutPort *start, InPort *end);
#ifndef SWIG
      std::vector<unsigned int> getPassedResults(Executor *execut, std::vector<SequenceAny *>& outputs, std::vector<std::string>& nameOfOutputs) const;
      void assignPassedResults(const std::vector<unsigned int>& passedIds, const std::vector<SequenceAny *>& passedOutputs, const std::vector<std::string>& nameOfOutputs);
#endif
    protected:
      Node *simpleClone(ComposedNode *father, bool editionOnly=true) const;
      void checkLinkPossibility(OutPort *start, const std::list<ComposedNode *>& pointsOfViewStart,
                                InPort *end, const std::list<ComposedNode *>& pointsOfViewEnd) throw(Exception);
      YACS::Event updateStateOnFinishedEventFrom(Node *node);
      YACS::Event updateStateForInitNodeOnFinishedEventFrom(Node *node, unsigned int id);
      YACS::Event updateStateForWorkNodeOnFinishedEventFrom(Node *node, unsigned int id, bool isNormalFinish);
      YACS::Event updateStateForFinalizeNodeOnFinishedEventFrom(Node *node, unsigned int id);
      YACS::Event updateStateOnFailedEventFrom(Node *node, const Executor *execInst);
      void buildDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView);
      void getDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView) throw(Exception);
      void releaseDelegateOf(OutPort *portDwn, OutPort *portUp, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView) throw(Exception);
    protected:
      void cleanDynGraph();
      void pushAllSequenceValues();
      void createOutputOutOfScopeInterceptors(int branchNb);
      void prepareSequenceValues(int sizeOfSamples);
      OutPort *getDynOutPortByAbsName(int branchNb, const std::string& name);
      void storeOutValsInSeqForOutOfScopeUse(int rank, int branchNb);
    private:
      int getFinishedId();
    public:
      static void InterceptorizeNameOfPort(std::string& portName);
      static const char INTERCEPTOR_STR[];
    };
  }
} 

#endif
