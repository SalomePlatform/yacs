#ifndef __NODE_HXX__
#define __NODE_HXX__

#include "define.hxx"
#include "InGate.hxx"
#include "OutGate.hxx"
#include "Exception.hxx"

#include <list>
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
      std::list<InputPort *> _listOfInputPort;
      std::list<OutputPort *> _listOfOutputPort;
      std::list<InputDataStreamPort *> _listOfInputDataStreamPort;
      std::list<OutputDataStreamPort *> _listOfOutputDataStreamPort;
      static const char SEP_CHAR_IN_PORT='?';
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
      std::list<Node *> getOutNodes() const;
      virtual void exUpdateState();
      virtual void getReadyTasks(std::vector<Task *>& tasks) = 0;
      virtual std::list<ElementaryNode *> getRecursiveConstituents() = 0;
      virtual int getNumberOfInputPorts() const;
      virtual int getNumberOfOutputPorts() const;
      virtual std::list<InputPort *> getListOfInputPort() const { return _listOfInputPort; }
      virtual std::list<OutputPort *> getListOfOutputPort() const { return _listOfOutputPort; }
      virtual InputPort *getInputPort(const std::string& name) const throw(Exception);
      virtual OutputPort *getOutputPort(const std::string& name) const throw(Exception);
      virtual std::list<InputDataStreamPort *> getListOfInputDataStreamPort() const { return _listOfInputDataStreamPort; }
      virtual std::list<OutputDataStreamPort *> getListOfOutputDataStreamPort() const { return _listOfOutputDataStreamPort; }
      virtual InputDataStreamPort *getInputDataStreamPort(const std::string& name) const throw(Exception);
      virtual OutputDataStreamPort *getOutputDataStreamPort(const std::string& name) const throw(Exception);
      std::list<ComposedNode *> getAllAscendanceOf(ComposedNode *levelToStop = 0);
    protected:
      bool areAllInputPortsValid() const;
      virtual ComposedNode *getRootNode() throw(Exception);
      virtual void disconnectAllLinksConnectedTo(Node *node) = 0;
      static void checkValidityOfPortName(const std::string& name) throw(Exception);
      static ComposedNode *checkHavingCommonFather(Node *node1, Node *node2) throw(Exception);
      template<class PORT>
      PORT *getPort(const std::string& name, const std::list<PORT *>& listOfPorts) const throw(Exception);
      template<class PORT, class ENUMTYPE>
      PORT *edAddPort(const std::string& portName, std::list<PORT *>& listOfPorts, ENUMTYPE type) throw(Exception);
      template<class PORT>
      static void edRemovePortTypedFromList(PORT *port, std::list<PORT *>& listOfPorts) throw(Exception);
      template<class PORT>
      static bool isPortNameAlreadyExist(const std::string& portName, const std::list<PORT *>& listOfPorts);
    private:
      void initForDFS() const;
    };

    template<class PORT>
    PORT *Node::getPort(const std::string& name, const std::list<PORT *>& listOfPorts) const throw(Exception)
    {
      for(typename std::list<PORT *>::const_iterator iter=listOfPorts.begin();iter!=listOfPorts.end();iter++)
	{
	  if((*iter)->getName()==name)
	    return *iter;
	}
      std::string what="Node::getPort : unexisting "; what+=PORT::NAME;
      what+=" with name ";
      what+=name;
      throw Exception(what);
    }

    template<class PORT, class ENUMTYPE>
    PORT *Node::edAddPort(const std::string& portName, std::list<PORT *>& listOfPorts, ENUMTYPE type) throw(Exception)
    {
      checkValidityOfPortName(portName);
      if(isPortNameAlreadyExist<PORT>(portName, listOfPorts))
	{
	  std::string what="Port of type "; what+=PORT::NAME; what += " with name : "; what+=portName; what+=" already exists";
	  throw Exception(what);
	}
      PORT *ret=new PORT(portName,this,type);
      listOfPorts.push_back(ret);
      return ret;
    }

    template<class PORT>
    void Node::edRemovePortTypedFromList(PORT *port, std::list<PORT *>& listOfPorts) throw(Exception)
    {
      if(!isPortNameAlreadyExist<PORT>(port->getName(), listOfPorts))
	throw Exception("Port is not part of the list : unable to remove it");
      listOfPorts.remove(port);
    }

    template<class PORT>
    bool Node::isPortNameAlreadyExist(const std::string& portName, const std::list<PORT *>& listOfPorts)
    {
      for(typename std::list<PORT *>::const_iterator iter=listOfPorts.begin();iter!=listOfPorts.end();iter++)
	{
	  if((*iter)->getName()==portName)
	    return true;
	}
      return false;
    }
  }
}

#endif
