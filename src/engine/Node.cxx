#include "Node.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "ComposedNode.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"

using namespace YACS::ENGINE;
using namespace std;

Node::Node(const string& name):_name(name),_inGate(this),_outGate(this),_father(0),_colour(YACS::White),_state(YACS::INITED)
{
}

Node::~Node()
{
}

/**
 *  initialisation of all input and output ports and gates, for execution
 */

void Node::init()
{
  _inGate.exReset();
  for(set<OutputPort *>::iterator iter=_setOfOutputPort.begin();iter!=_setOfOutputPort.end();iter++)
    (*iter)->exInit();
  for(set<InputPort *>::iterator iter2=_setOfInputPort.begin();iter2!=_setOfInputPort.end();iter2++)
    (*iter2)->exInit();
  if(_inGate.exIsReady())
    _state=YACS::TOACTIVATE;
  else
    _state=YACS::INITED;
}

/**
 *  get the set of all nodes connected to the outGate
 */

set<Node *> Node::getOutNodes() const
{
  set<Node *> ret;
  set<InGate *> inGates=_outGate.edSetInGate();
  for(set<InGate *>::iterator iter=inGates.begin();iter!=inGates.end();iter++)
    ret.insert((*iter)->getNode());
  return ret;
}

/**
 * @note : Update the '_state' attribute.
 *          Typically called by 'this->_inGate' when 'this->_inGate' is ready.
 */

void Node::exUpdateState()
{
  if(_inGate.exIsReady())
    if(areAllInputPortsValid())
      _state=YACS::TOACTIVATE;
    else
      {
	string what("Node::exUpdateState : Invalid graph given : Node with name \"");
	what+=_name; what+="\" ready to run whereas some inputports are not set correctly\nCheck coherence DF/CF";
	throw Exception(what);
      }
}

int Node::getNumberOfInputPorts() const
{
  return _setOfInputPort.size();
}

int Node::getNumberOfOutputPorts() const
{
  return _setOfOutputPort.size();
}

InputPort *Node::getInputPort(const string& name) const throw(Exception)
{
  return getPort<InputPort>(name,_setOfInputPort);
}

OutputPort *Node::getOutputPort(const string& name) const throw(Exception)
{
  return getPort<OutputPort>(name,_setOfOutputPort);
}

InputDataStreamPort *Node::getInputDataStreamPort(const string& name) const throw(Exception)
{
  return getPort<InputDataStreamPort>(name,_setOfInputDataStreamPort);
}

OutputDataStreamPort *Node::getOutputDataStreamPort(const string& name) const throw(Exception)
{
  return getPort<OutputDataStreamPort>(name,_setOfOutputDataStreamPort);
}


/**
 * gets a set of the composed nodes that constitute the ascendancy of this node, starting from root
 * or from a particular ancestor
 * @param  levelToStop   composed node which is the oldest ancestor required
 * @return               ascendancy, direct father first in set.
 */

set<ComposedNode *> Node::getAllAscendanceOf(ComposedNode *levelToStop)
{
  set<ComposedNode *> ret;
  for(ComposedNode *iter=_father;iter!=levelToStop && iter!=0; iter=iter->_father)
      ret.insert(iter);
  return ret;
}

/**
 *  @return Implementation of node: C++, Python, CORBA...
 *  _implementation is set by a derived class in a Runtime
 *  it normally applies only to elementaryNodes and it is used by Ports to select Data Converters.
 *  Potential problem with Ports attached to composed Nodes...
 */

string Node::getImplementation()
{
  return _implementation;
}

/**
 * checks if all input ports contains a valid data. Used at execution to change the state of the node
 * for activation.
 */

bool Node::areAllInputPortsValid() const
{
  bool ret=true;
  for(set<InputPort *>::const_iterator iter=_setOfInputPort.begin();iter!=_setOfInputPort.end();iter++)
    {
      ret=!(*iter)->isEmpty();
      if (!ret) break;
    }
  return ret;
}

ComposedNode *Node::getRootNode() throw(Exception)
{
  if(!_father)
    throw Exception("No root node");
  ComposedNode *iter=_father;
  while(iter->_father)
    iter=iter->_father;
  return iter;
}

/**
 * checks validity of ports name, that must not contain a particular character '?'
 * USAGE NOT CLEAR, not used so far, when are those characters set ?
 */

void Node::checkValidityOfPortName(const string& name) throw(Exception)
{
  if(name.find(SEP_CHAR_IN_PORT, 0 )!=string::npos)
    {
      string what("Port name "); what+=name; what+="not valid because it contains character "; what+=SEP_CHAR_IN_PORT;
      throw Exception(what);
    }
}

/**
 * @note : Check that 'node1' and 'node2' have exactly the same father
 * @exception : If 'node1' and 'node2' have NOT exactly the same father
 */
ComposedNode *Node::checkHavingCommonFather(Node *node1, Node *node2) throw(Exception)
{
  if(node1!=0 && node2!=0)
    {
      if(node1->_father==node2->_father)
	return node1->_father;
    }
  throw Exception("check failed : nodes have not the same father");
}

/**
 * set color for inGates : display
 * USAGE NOT CLEAR, not used so far
 */

void Node::initForDFS() const
{
  _colour=YACS::White;
  set<InGate *> inGates=_outGate.edSetInGate();
  for(set<InGate *>::iterator iter=inGates.begin();iter!=inGates.end();iter++)
    (*iter)->initForDFS();
}
