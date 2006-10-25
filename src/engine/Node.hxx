#ifndef __NODE_HXX__
#define __NODE_HXX__

#include "InGate.hxx"
#include "OutGate.hxx"
#include "Exception.hxx"
#include "define.hxx"

#include <set>
#include <string>
#include <vector>

namespace YACS
{
  namespace ENGINE
  {
    class Task;
    class InputPort;
    class OutputPort;
    class ComposedNode;
    class ElementaryNode;
    class InputDataStreamPort;
    class OutputDataStreamPort;
    
    class Node
    {
      friend class Bloc;
      friend class ComposedNode;
      friend class ElementaryNode;
    protected:
      InGate _inGate;
      OutGate _outGate;
      std::string _name;
      ComposedNode *_father;
      YACS::StatesForNode _state;
      std::set<InputPort *> _setOfInputPort;
      std::set<OutputPort *> _setOfOutputPort;
      std::set<InputDataStreamPort *> _setOfInputDataStreamPort;
      std::set<OutputDataStreamPort *> _setOfOutputDataStreamPort;
      static const char SEP_CHAR_IN_PORT='?';
      std::string _implementation;
    private:
      //for graphs algorithms
      mutable YACS::Colour _colour;
    protected:
      Node(const std::string& name);
    public:
      virtual ~Node();
      virtual void init();
      InGate *getInGate() { return &_inGate; }
      OutGate *getOutGate() { return &_outGate; }
      const std::string& getName() const { return _name; }
      std::set<Node *> getOutNodes() const;
      virtual void exUpdateState();
      virtual void getReadyTasks(std::vector<Task *>& tasks) = 0;
      virtual std::set<ElementaryNode *> getRecursiveConstituents() = 0;
      virtual int getNumberOfInputPorts() const;
      virtual int getNumberOfOutputPorts() const;
      virtual std::set<InputPort *> getSetOfInputPort() const { return _setOfInputPort; }
      virtual std::set<OutputPort *> getSetOfOutputPort() const { return _setOfOutputPort; }
      virtual InputPort *getInputPort(const std::string& name) const throw(Exception);
      virtual OutputPort *getOutputPort(const std::string& name) const throw(Exception);
      virtual const std::string getInputPortName(const InputPort *) throw (Exception) =0;
      virtual const std::string getOutputPortName(const OutputPort *) throw (Exception) =0;
      virtual std::set<InputDataStreamPort *> getSetOfInputDataStreamPort() const { return _setOfInputDataStreamPort; }
      virtual std::set<OutputDataStreamPort *> getSetOfOutputDataStreamPort() const { return _setOfOutputDataStreamPort; }
      virtual InputDataStreamPort *getInputDataStreamPort(const std::string& name) const throw(Exception);
      virtual OutputDataStreamPort *getOutputDataStreamPort(const std::string& name) const throw(Exception);
      std::set<ComposedNode *> getAllAscendanceOf(ComposedNode *levelToStop = 0);
      std::string getImplementation();
    protected:
      bool areAllInputPortsValid() const;
      virtual ComposedNode *getRootNode() throw(Exception);
      virtual void disconnectAllLinksConnectedTo(Node *node) = 0;
      static void checkValidityOfPortName(const std::string& name) throw(Exception);
      static ComposedNode *checkHavingCommonFather(Node *node1, Node *node2) throw(Exception);
      template<class PORT>
      PORT *getPort(const std::string& name, const std::set<PORT *>& setOfPorts) const throw(Exception);
      template<class PORT, class ENUMTYPE>
      PORT *edAddPort(const std::string& portName, std::set<PORT *>& setOfPorts, ENUMTYPE type) throw(Exception);
      template<class PORT, class ENUMTYPE>
      bool edCheckAddPort(const std::string& portName, std::set<PORT *>& setOfPorts, ENUMTYPE type) throw(Exception);
      template<class PORT>
      static void edRemovePortTypedFromSet(PORT *port, std::set<PORT *>& setOfPorts) throw(Exception);
      template<class PORT>
      static bool isPortNameAlreadyExist(const std::string& portName, const std::set<PORT *>& setOfPorts);
    private:
      void initForDFS() const;
    };

    /**
     * protected: get a port in a set given it's name
     */

    template<class PORT>
    PORT *Node::getPort(const std::string& name, const std::set<PORT *>& setOfPorts) const throw(Exception)
    {
      for(typename std::set<PORT *>::const_iterator iter=setOfPorts.begin();iter!=setOfPorts.end();iter++)
	{
	  if((*iter)->getName()==name)
	    return *iter;
	}
      std::string what="Node::getPort : unexisting "; what+=PORT::NAME;
      what+=" with name ";
      what+=name;
      throw Exception(what);
    }

    /**
     * protected: add a port given it's name and type, in a given set of ports
     * WHY TEMPLATE PARAMETER ENUMTYPE?
     */

    template<class PORT, class ENUMTYPE>
    PORT *Node::edAddPort(const std::string& portName, std::set<PORT *>& setOfPorts, ENUMTYPE type) throw(Exception)
    {
      checkValidityOfPortName(portName);
      if(isPortNameAlreadyExist<PORT>(portName, setOfPorts))
	{
	  std::string what="Port of type "; what+=PORT::NAME; what += " with name : "; what+=portName; what+=" already exists";
	  throw Exception(what);
	}
      PORT *ret=new PORT(portName,this,type);
      //      PORT *ret= getRuntime()->createInputPort(portName, _implementation, type);
      setOfPorts.insert(ret);
      return ret;
    }

    template<class PORT, class ENUMTYPE>
    bool Node::edCheckAddPort(const std::string& portName, std::set<PORT *>& setOfPorts, ENUMTYPE type) throw(Exception)
    {
      checkValidityOfPortName(portName);
      if(isPortNameAlreadyExist<PORT>(portName, setOfPorts))
	{
	  std::string what="Port of type "; what+=PORT::NAME; what += " with name : "; what+=portName; what+=" already exists";
	  throw Exception(what);
	}
//       PORT *ret=new PORT(portName,this,type);
//       PORT *ret= getRuntime()->createOutputPort(portName, _implementation, type);
//       setOfPorts.insert(ret);
      return true;
    }

    /**
     * protected: remove a port from a given set
     */

    template<class PORT>
    void Node::edRemovePortTypedFromSet(PORT *port, std::set<PORT *>& setOfPorts) throw(Exception)
    {
      if(!isPortNameAlreadyExist<PORT>(port->getName(), setOfPorts))
	throw Exception("Port is not part of the set : unable to remove it");
      setOfPorts.erase(port);
    }

    /**
     * protected: checks existence of a port, given it's name, in a set
     */

    template<class PORT>
    bool Node::isPortNameAlreadyExist(const std::string& portName, const std::set<PORT *>& setOfPorts)
    {
      for(typename std::set<PORT *>::const_iterator iter=setOfPorts.begin();iter!=setOfPorts.end();iter++)
	{
	  if((*iter)->getName()==portName)
	    return true;
	}
      return false;
    }
  }
}

#endif
