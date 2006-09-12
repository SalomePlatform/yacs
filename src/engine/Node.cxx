#include "Node.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "ComposedNode.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"

using namespace YACS::ENGINE;

Node::Node(const std::string& name):_name(name),_inGate(this),_outGate(this),_father(0),_colour(YACS::White),_state(YACS::INITED)
{
}

Node::~Node()
{
}

void Node::init()
{
  _inGate.exReset();
  for(std::list<OutputPort *>::iterator iter=_listOfOutputPort.begin();iter!=_listOfOutputPort.end();iter++)
    (*iter)->exInit();
  for(std::list<InputPort *>::iterator iter2=_listOfInputPort.begin();iter2!=_listOfInputPort.end();iter2++)
    (*iter2)->exInit();
  if(_inGate.exIsReady())
    _state=YACS::TOACTIVATE;
  else
    _state=YACS::INITED;
}

std::list<Node *> Node::getOutNodes() const
{
  std::list<Node *> ret;
  std::list<InGate *> inGates=_outGate.edListInGate();
  for(std::list<InGate *>::iterator iter=inGates.begin();iter!=inGates.end();iter++)
    ret.push_back((*iter)->getNode());
  return ret;
}

/**
 * @ note : Update the '_state' attribute.
 *          Typically called by 'this->_inGate' when 'this->_inGate' is ready.
 */
void Node::exUpdateState()
{
  if(_inGate.exIsReady())
    if(areAllInputPortsValid())
      _state=YACS::TOACTIVATE;
    else
      {
	std::string what("Node::exUpdateState : Invalid graph given : Node with name \"");
	what+=_name; what+="\" ready to run whereas some inputports are not set correctly\nCheck coherence DF/CF";
	throw Exception(what);
      }
}

int Node::getNumberOfInputPorts() const
{
  return _listOfInputPort.size();
}

int Node::getNumberOfOutputPorts() const
{
  return _listOfOutputPort.size();
}

InputPort *Node::getInputPort(const std::string& name) const throw(Exception)
{
  return getPort<InputPort>(name,_listOfInputPort);
}

OutputPort *Node::getOutputPort(const std::string& name) const throw(Exception)
{
  return getPort<OutputPort>(name,_listOfOutputPort);
}

InputDataStreamPort *Node::getInputDataStreamPort(const std::string& name) const throw(Exception)
{
  return getPort<InputDataStreamPort>(name,_listOfInputDataStreamPort);
}

OutputDataStreamPort *Node::getOutputDataStreamPort(const std::string& name) const throw(Exception)
{
  return getPort<OutputDataStreamPort>(name,_listOfOutputDataStreamPort);
}

std::list<ComposedNode *> Node::getAllAscendanceOf(ComposedNode *levelToStop)
{
  std::list<ComposedNode *> ret;
  for(ComposedNode *iter=_father;iter!=levelToStop && iter!=0; iter=iter->_father)
      ret.push_back(iter);
  return ret;
}

bool Node::areAllInputPortsValid() const
{
  bool ret=true;
  for(std::list<InputPort *>::const_iterator iter=_listOfInputPort.begin();iter!=_listOfInputPort.end();iter++)
    ret=!(*iter)->exGet().empty();
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

void Node::checkValidityOfPortName(const std::string& name) throw(Exception)
{
  if(name.find(SEP_CHAR_IN_PORT, 0 )!=std::string::npos)
    {
      std::string what("Port name "); what+=name; what+="not valid because it contains character "; what+=SEP_CHAR_IN_PORT;
      throw Exception(what);
    }
}

/**
 * @ note : Check that 'node1' and 'node2' have exactly the same father
 * @ exception : If 'node1' and 'node2' have NOT exactly the same father
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

void Node::initForDFS() const
{
  _colour=YACS::White;
  std::list<InGate *> inGates=_outGate.edListInGate();
  for(std::list<InGate *>::iterator iter=inGates.begin();iter!=inGates.end();iter++)
    (*iter)->initForDFS();
}
