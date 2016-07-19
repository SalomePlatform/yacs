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

#ifndef __NODE_HXX__
#define __NODE_HXX__

#include "YACSlibEngineExport.hxx"
#include "InGate.hxx"
#include "OutGate.hxx"
#include "Exception.hxx"
#include "define.hxx"

#include <set>
#include <string>
#include <vector>
#include <map>

namespace YACS
{
  namespace ENGINE
  {
    void YACSLIBENGINE_EXPORT StateLoader(Node* node, YACS::StatesForNode state);
    class Task;
    class InPort;
    class OutPort;
    class InputPort;
    class OutputPort;
    class InPropertyPort;
    class DynParaLoop;
    class ForEachLoop;
    class ComposedNode;
    class Proc;
    class ElementaryNode;
    class Switch;
    class InputDataStreamPort;
    class OutputDataStreamPort;
    class Visitor;

    struct ProgressWeight
    {
    	int weightDone;
    	int weightTotal;
    };

    class YACSLIBENGINE_EXPORT NodeStateNameMap : public std::map<YACS::StatesForNode, std::string>
    {
    public:
      NodeStateNameMap();
    };

    class YACSLIBENGINE_EXPORT Node
    {
      friend class Bloc;
      friend class Loop;
      friend class Switch;
      friend class InputPort;
      friend class OutputPort;
      friend class InPropertyPort;
      friend class DynParaLoop;
      friend class ForEachLoop;
      friend class ComposedNode;
      friend class ElementaryNode;
      friend class Visitor;
      friend void StateLoader(Node* node, YACS::StatesForNode state);
    public:
      mutable YACS::Colour _colour;
    protected:
      InGate _inGate;
      OutGate _outGate;
      InPropertyPort * _inPropertyPort;
      std::string _name;
      ComposedNode *_father;
      YACS::StatesForNode _state;
      int _modified;
      std::string _errorDetails;
      static const char SEP_CHAR_IN_PORT[];
      static int _total;
      int _numId;
      std::string _implementation;
      std::map<std::string,std::string> _propertyMap;
    protected:
      Node(const std::string& name);
      Node(const Node& other, ComposedNode *father);
      //! performs a duplication of placement using clone method of containers and components. clone behaviour is driven by attachOnCloning attribute.
      virtual void performDuplicationOfPlacement(const Node& other) = 0;
      //! performs a also duplication of placement but here containers and components are not copied at all whatever the value of attachedOnCloning.
      virtual void performShallowDuplicationOfPlacement(const Node& other) = 0;
      virtual Node *simpleClone(ComposedNode *father, bool editionOnly=true) const = 0;
    public:
      virtual ~Node();
      virtual void init(bool start=true);
      virtual void shutdown(int level);
      virtual void resetState(int level);
      //! \b This method \b MUST \b NEVER \b BE \b VIRTUAL
      Node *clone(ComposedNode *father, bool editionOnly=true) const;
      //! \b This method \b MUST \b NEVER \b BE \b VIRTUAL
      Node *cloneWithoutCompAndContDeepCpy(ComposedNode *father, bool editionOnly=true) const;
      void setState(YACS::StatesForNode theState); // To centralize state changes
      virtual YACS::StatesForNode getState() const { return _state; }
      virtual YACS::StatesForNode getEffectiveState() const;
      virtual YACS::StatesForNode getEffectiveState(const Node*) const;
      std::string getColorState(YACS::StatesForNode state) const;
      static std::string getStateName(YACS::StatesForNode state);
      InGate *getInGate() { return &_inGate; }
      OutGate *getOutGate() { return &_outGate; }
      const std::string& getName() const { return _name; }
      void setName(const std::string& name);
      ComposedNode * getFather() const { return _father; }
      const std::string getId() const;
      bool exIsControlReady() const;
      std::list<Node *> getOutNodes() const;
      virtual void writeDot(std::ostream &os) const;
      virtual void exUpdateState();
      virtual void exFailedState();
      virtual void exDisabledState();
      virtual void getReadyTasks(std::vector<Task *>& tasks) = 0;
      virtual std::list<ElementaryNode *> getRecursiveConstituents() const = 0;
      virtual std::list<ProgressWeight> getProgressWeight() const = 0;
      virtual int getNumberOfInputPorts() const = 0;
      virtual int getNumberOfOutputPorts() const = 0;
      std::list<InPort *> getSetOfInPort() const;
      std::list<OutPort *> getSetOfOutPort() const;
      virtual std::list<InputPort *> getSetOfInputPort() const = 0;
      virtual std::list<OutputPort *> getSetOfOutputPort() const = 0;
      virtual std::list<InputPort *> getLocalInputPorts() const = 0;
      virtual std::list<OutputPort *> getLocalOutputPorts() const = 0;
      virtual std::set<InputPort *> edGetSetOfUnitializedInputPort() const;
      virtual bool edAreAllInputPortInitialized() const;
      virtual std::string getInPortName(const InPort *) const throw (Exception) = 0;
      virtual std::string getOutPortName(const OutPort *) const throw (Exception) = 0;
      virtual std::list<InputDataStreamPort *> getSetOfInputDataStreamPort() const = 0;
      virtual std::list<OutputDataStreamPort *> getSetOfOutputDataStreamPort() const = 0;
      InPort *getInPort(const std::string& name) const throw(Exception);
      InPropertyPort *getInPropertyPort() const throw(Exception);
      virtual OutPort *getOutPort(const std::string& name) const throw(Exception);
      virtual std::set<OutPort *> getAllOutPortsLeavingCurrentScope() const = 0;
      virtual std::set<InPort *> getAllInPortsComingFromOutsideOfCurrentScope() const = 0;
      virtual std::vector< std::pair<OutPort *, InPort *> > getSetOfLinksLeavingCurrentScope() const = 0;
      virtual std::vector< std::pair<InPort *, OutPort *> > getSetOfLinksComingInCurrentScope() const =0;
      virtual InputPort *getInputPort(const std::string& name) const throw(Exception);
      virtual OutputPort *getOutputPort(const std::string& name) const throw(Exception) = 0;
      virtual InputDataStreamPort *getInputDataStreamPort(const std::string& name) const throw(Exception) = 0;
      virtual OutputDataStreamPort *getOutputDataStreamPort(const std::string& name) const throw(Exception) = 0;
      std::list<ComposedNode *> getAllAscendanceOf(ComposedNode *levelToStop = 0) const;
      bool operator>(const Node& other) const;
      bool operator<(const Node& other) const;
      std::string getImplementation() const;
      virtual ComposedNode *getRootNode() const throw(Exception);
      virtual void setProperty(const std::string& name,const std::string& value);
      virtual std::string getProperty(const std::string& name);
      std::map<std::string,std::string> getProperties() ;
      std::map<std::string,std::string> getPropertyMap() { return _propertyMap; }
      virtual void setProperties(std::map<std::string,std::string> properties);
      virtual Node *getChildByName(const std::string& name) const throw(Exception) = 0;
      virtual Proc *getProc();
      virtual const Proc *getProc() const;
      virtual void accept(Visitor *visitor) = 0;
      virtual int getMaxLevelOfParallelism() const = 0;
      std::string getQualifiedName() const;
      int getNumId();
      std::vector<std::pair<std::string,int> > getDPLScopeInfo(ComposedNode *gfn);
      virtual void applyDPLScope(ComposedNode *gfn);
      virtual void sendEvent(const std::string& event);
      virtual void sendEvent2(const std::string& event, void *something);
      static std::map<int,Node *> idMap;
      virtual std::string typeName() { return "YACS__ENGINE__Node"; }
      virtual std::string getErrorDetails() const { return _errorDetails; }
      virtual void setErrorDetails(const std::string& error) { _errorDetails=error; }
      virtual void modified();
      virtual int isModified() { return _modified; }
      virtual int isValid();
      virtual void edUpdateState();
      virtual std::string getErrorReport();
      virtual std::string getContainerLog();
      virtual void ensureLoading();
      virtual void getCoupledNodes(std::set<Task*>& coupledNodes) { }
      virtual void cleanNodes();
    protected:
      virtual void exForwardFailed();
      virtual void exForwardFinished();
      virtual void edDisconnectAllLinksWithMe();
      static void checkValidityOfPortName(const std::string& name) throw(Exception);
      static ComposedNode *checkHavingCommonFather(Node *node1, Node *node2) throw(Exception);
    };

  }
}

#endif
