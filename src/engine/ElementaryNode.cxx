#include "ElementaryNode.hxx"
#include "Runtime.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "ComposedNode.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"

using namespace YACS::ENGINE;
using namespace std;

ElementaryNode::ElementaryNode(const string& name):Node(name)
{
}

ElementaryNode::~ElementaryNode()
{
  for(set<InputPort *>::iterator iter1=_setOfInputPort.begin();iter1!=_setOfInputPort.end();iter1++)
    delete *iter1;
  for(set<OutputPort *>::iterator iter2=_setOfOutputPort.begin();iter2!=_setOfOutputPort.end();iter2++)
    delete *iter2;
  for(set<InputDataStreamPort *>::iterator iter3=_setOfInputDataStreamPort.begin();iter3!=_setOfInputDataStreamPort.end();iter3++)
    delete *iter3;
  for(set<OutputDataStreamPort *>::iterator iter4=_setOfOutputDataStreamPort.begin();iter4!=_setOfOutputDataStreamPort.end();iter4++)
    delete *iter4;
}

/**
 * add this node task to a given set of ready tasks, if this task is ready to activate
 */

void ElementaryNode::getReadyTasks(vector<Task *>& tasks)
{
  if(_state==YACS::TOACTIVATE)
    tasks.push_back(this);
}

/**
 * remove port from node at edition. Ports are typed.
 */

void ElementaryNode::edRemovePort(Port *port) throw(Exception)
{
  if(port->getNode()!=this)
    throw Exception("ElementaryNode::edRemovePort : Port is not owned by this");
  string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance==InputPort::NAME)
    edRemovePortTypedFromSet<InputPort>(dynamic_cast<InputPort *>(port),_setOfInputPort);
  else if(typeOfPortInstance==OutputPort::NAME)
    edRemovePortTypedFromSet<OutputPort>(dynamic_cast<OutputPort *>(port),_setOfOutputPort);
  else if(typeOfPortInstance==InputDataStreamPort::NAME)
    edRemovePortTypedFromSet<InputDataStreamPort>(dynamic_cast<InputDataStreamPort *>(port),_setOfInputDataStreamPort);
  else if(typeOfPortInstance==OutputDataStreamPort::NAME)
    edRemovePortTypedFromSet<OutputDataStreamPort>(dynamic_cast<OutputDataStreamPort *>(port),_setOfOutputDataStreamPort);
  else
    throw Exception("ElementaryNode::edRemovePort : unknown port type");
  delete port;
}

/**
 * @return a set with only this node. (Same method in composed nodes)
 */

set<ElementaryNode *> ElementaryNode::getRecursiveConstituents()
{
  set<ElementaryNode *> ret;
  ret.insert(this);
  return ret;
}

/**
 * the input port is also published recursively in ancestors because it may be visible from everywhere.
 * WARNING: CHECK CASE OF BLOC: ONLY INPUT PORTS NOT INTERNALLY CONNECTED MUST BE VISIBLE.
 */

InputPort *ElementaryNode::edAddInputPort(const string& inputPortName, TypeCode* type) throw(Exception)
{
  InputPort *ret = 0;
  if (edCheckAddPort<InputPort, TypeCode*>(inputPortName,_setOfInputPort,type))
    {
      //InputPort *ret=edAddPort<InputPort, TypeCode*>(inputPortName,_setOfInputPort,type);
      ret = getRuntime()->createInputPort(inputPortName, _implementation, this, type);
      _setOfInputPort.insert(ret);
      //By default all inputports are seen from upper level nodes NOT outputports
      ComposedNode *iter=_father;
      while(iter)
	{
	  iter->publishInputPort(ret);
	  iter=iter->_father;
	}
    }
  return ret;
}

/**
 * The output port is not published in father. Father must create an output port.
 */ 

OutputPort *ElementaryNode::edAddOutputPort(const string& outputPortName, TypeCode* type) throw(Exception)
{
  OutputPort *ret =0;
  if (edCheckAddPort<OutputPort, TypeCode*>(outputPortName,_setOfOutputPort,type))
    {
      ret = getRuntime()->createOutputPort(outputPortName, _implementation, this, type);
      _setOfOutputPort.insert(ret);
    }
  return ret;
}

InputDataStreamPort *ElementaryNode::edAddInputDataStreamPort(const string& inputPortDSName, TypeCode* type) throw(Exception)
{
  return edAddPort<InputDataStreamPort,TypeCode*>(inputPortDSName,_setOfInputDataStreamPort,type);
}

OutputDataStreamPort *ElementaryNode::edAddOutputDataStreamPort(const string& outputPortDSName, TypeCode* type) throw(Exception)
{
  return edAddPort<OutputDataStreamPort,TypeCode*>(outputPortDSName,_setOfOutputDataStreamPort,type);
}

/**
 * Disconnect all links from output ports of this node to input ports of a given node
 */

void ElementaryNode::disconnectAllLinksConnectedTo(Node *node)
{
  set<InputPort *> inputDF=node->getSetOfInputPort();
  for(set<OutputPort *>::iterator iter1=_setOfOutputPort.begin();iter1!=_setOfOutputPort.end();iter1++)
    for(set<InputPort *>::iterator iter2=inputDF.begin();iter2!=inputDF.end();iter2++)
      (*iter1)->edRemoveInputPortOneWay(*iter2);
  set<InputDataStreamPort *> inputDS=node->getSetOfInputDataStreamPort();
  for(set<OutputDataStreamPort *>::iterator iter3=_setOfOutputDataStreamPort.begin();iter3!=_setOfOutputDataStreamPort.end();iter3++)
    for(set<InputDataStreamPort *>::iterator iter4=inputDS.begin();iter4!=inputDS.end();iter4++)
      (*iter3)->edRemoveInputDataStreamPortOneWay(*iter4);
  _outGate.edRemoveInGateOneWay(node->getInGate());
}

/**
 * get the input port name used by the current node (see composed nodes)
 */

const string ElementaryNode::getInputPortName(const InputPort * inputPort) throw (Exception)
{
  Node *node = inputPort->getNode();
  if ( node != this ) 
    {
      string what("InputPort "); what += inputPort->getName(); what += " does not belong to node "; what += node->getName();
      throw Exception(what);
    }
  return inputPort->getName();
}

const string ElementaryNode::getOutputPortName(const OutputPort *outputPort) throw (Exception)
{
}

void ElementaryNode::begin()
{
  _state=ACTIVATED;
}

bool ElementaryNode::isReady()
{
  return _state==TOACTIVATE;
}

void ElementaryNode::finished()
{
  _state=DONE;
}
