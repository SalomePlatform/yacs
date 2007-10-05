#ifndef __NODE_HXX__
#define __NODE_HXX__

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
    class Task;
    class InPort;
    class OutPort;
    class InputPort;
    class OutputPort;
    class DynParaLoop;
    class ForEachLoop;
    class ComposedNode;
    class ElementaryNode;
    class Switch;
    class InputDataStreamPort;
    class OutputDataStreamPort;
    class Visitor;
    
/*! \brief Base class for all nodes
 *
 * \ingroup Nodes
 *
 *
 */
    class Node
    {
      friend class Bloc;
      friend class Loop;
      friend class Switch;
      friend class InputPort;
      friend class OutputPort;
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
      std::string _name;
      ComposedNode *_father;
      YACS::StatesForNode _state;
      static const char SEP_CHAR_IN_PORT[];
      static int _total;
      int _numId;
      std::string _implementation;
      std::map<std::string,std::string> _propertyMap;
    protected:
      Node(const std::string& name);
      Node(const Node& other, ComposedNode *father);
      virtual void performDuplicationOfPlacement(const Node& other) = 0;
      virtual Node *simpleClone(ComposedNode *father, bool editionOnly=true) const = 0;
    public:
      virtual ~Node();
      virtual void init(bool start=true);
      //! \b This method \b MUST \b NEVER \b BE \b VIRTUAL
      Node *clone(ComposedNode *father, bool editionOnly=true) const;
      void setState(YACS::StatesForNode theState); // To centralize state changes
      virtual YACS::StatesForNode getState() const { return _state; }
      virtual YACS::StatesForNode getEffectiveState();
      virtual YACS::StatesForNode getEffectiveState(Node*);
      std::string getColorState(YACS::StatesForNode state);
      InGate *getInGate() { return &_inGate; }
      OutGate *getOutGate() { return &_outGate; }
      const std::string& getName() const { return _name; }
      ComposedNode * getFather() const { return _father; }
      const std::string getId();
      bool exIsControlReady() const;
      std::set<Node *> getOutNodes() const;
      virtual void writeDot(std::ostream &os);
      virtual void exUpdateState();
      virtual void exFailedState();
      virtual void exDisabledState();
      virtual void getReadyTasks(std::vector<Task *>& tasks) = 0;
      virtual std::set<ElementaryNode *> getRecursiveConstituents() const = 0;
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
      virtual OutPort *getOutPort(const std::string& name) const throw(Exception);
      virtual std::set<OutPort *> getAllOutPortsLeavingCurrentScope() const = 0;
      virtual std::set<InPort *> getAllInPortsComingFromOutsideOfCurrentScope() const = 0;
      virtual InputPort *getInputPort(const std::string& name) const throw(Exception) = 0;
      virtual OutputPort *getOutputPort(const std::string& name) const throw(Exception) = 0;
      virtual InputDataStreamPort *getInputDataStreamPort(const std::string& name) const throw(Exception) = 0;
      virtual OutputDataStreamPort *getOutputDataStreamPort(const std::string& name) const throw(Exception) = 0;
      std::set<ComposedNode *> getAllAscendanceOf(ComposedNode *levelToStop = 0);
      std::string getImplementation();
      virtual ComposedNode *getRootNode() throw(Exception);
      virtual void setProperty(const std::string& name,const std::string& value);
      virtual Node *getChildByName(const std::string& name) const throw(Exception) = 0;
      virtual void accept(Visitor *visitor) = 0;
      std::string getQualifiedName() const;
      int getNumId();
      virtual void sendEvent(const std::string& event);
      static std::map<int,Node *> idMap;
    protected:
      virtual void exForwardFailed();
      virtual void exForwardFinished();
      virtual void edDisconnectAllLinksWithMe();
      static void checkValidityOfPortName(const std::string& name) throw(Exception);
      static ComposedNode *checkHavingCommonFather(Node *node1, Node *node2) throw(Exception);
    };

    void StateLoader(Node* node, YACS::StatesForNode state);
  }
}

#endif
