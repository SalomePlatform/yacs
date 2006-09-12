#include "ElementaryNode.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "ComposedNode.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"

using namespace YACS::ENGINE;

ElementaryNode::ElementaryNode(const std::string& name):Node(name)
{
}

ElementaryNode::~ElementaryNode()
{
  for(std::list<InputPort *>::iterator iter1=_listOfInputPort.begin();iter1!=_listOfInputPort.end();iter1++)
    delete *iter1;
  for(std::list<OutputPort *>::iterator iter2=_listOfOutputPort.begin();iter2!=_listOfOutputPort.end();iter2++)
    delete *iter2;
  for(std::list<InputDataStreamPort *>::iterator iter3=_listOfInputDataStreamPort.begin();iter3!=_listOfInputDataStreamPort.end();iter3++)
    delete *iter3;
  for(std::list<OutputDataStreamPort *>::iterator iter4=_listOfOutputDataStreamPort.begin();iter4!=_listOfOutputDataStreamPort.end();iter4++)
    delete *iter4;
}

void ElementaryNode::getReadyTasks(std::vector<Task *>& tasks)
{
  if(_state==YACS::TOACTIVATE)
    tasks.push_back(this);
}

void ElementaryNode::edRemovePort(Port *port) throw(Exception)
{
  if(port->getNode()!=this)
    throw Exception("ElementaryNode::edRemovePort : Port is not ownered by this");
  std::string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance==InputPort::NAME)
    edRemovePortTypedFromList<InputPort>(dynamic_cast<InputPort *>(port),_listOfInputPort);
  else if(typeOfPortInstance==OutputPort::NAME)
    edRemovePortTypedFromList<OutputPort>(dynamic_cast<OutputPort *>(port),_listOfOutputPort);
  else if(typeOfPortInstance==InputDataStreamPort::NAME)
    edRemovePortTypedFromList<InputDataStreamPort>(dynamic_cast<InputDataStreamPort *>(port),_listOfInputDataStreamPort);
  else if(typeOfPortInstance==OutputDataStreamPort::NAME)
    edRemovePortTypedFromList<OutputDataStreamPort>(dynamic_cast<OutputDataStreamPort *>(port),_listOfOutputDataStreamPort);
  else
    throw Exception("ElementaryNode::edRemovePort : unknown port type");
  delete port;
}

std::list<ElementaryNode *> ElementaryNode::getRecursiveConstituents()
{
  std::list<ElementaryNode *> ret;
  ret.push_back(this);
  return ret;
}

InputPort *ElementaryNode::edAddInputPort(const std::string& inputPortName, YACS::DynType type) throw(Exception)
{
  InputPort *ret=edAddPort<InputPort,YACS::DynType>(inputPortName,_listOfInputPort,type);
  //By default all inputports are seen from upper level nodes NOT outputports
  ComposedNode *iter=_father;
  while(iter)
    {
      iter->publishInputPort(_listOfInputPort.back());
      iter=iter->_father;
    }
  return ret;
}

OutputPort *ElementaryNode::edAddOutputPort(const std::string& outputPortName, YACS::DynType type) throw(Exception)
{
  return edAddPort<OutputPort,YACS::DynType>(outputPortName,_listOfOutputPort,type);
}

InputDataStreamPort *ElementaryNode::edAddInputDataStreamPort(const std::string& inputPortDSName, YACS::StreamType type) throw(Exception)
{
  return edAddPort<InputDataStreamPort,YACS::StreamType>(inputPortDSName,_listOfInputDataStreamPort,type);
}

OutputDataStreamPort *ElementaryNode::edAddOutputDataStreamPort(const std::string& outputPortDSName, YACS::StreamType type) throw(Exception)
{
  return edAddPort<OutputDataStreamPort,YACS::StreamType>(outputPortDSName,_listOfOutputDataStreamPort,type);
}

void ElementaryNode::disconnectAllLinksConnectedTo(Node *node)
{
  std::list<InputPort *> inputDF=node->getListOfInputPort();
  for(std::list<OutputPort *>::iterator iter1=_listOfOutputPort.begin();iter1!=_listOfOutputPort.end();iter1++)
    for(std::list<InputPort *>::iterator iter2=inputDF.begin();iter2!=inputDF.end();iter2++)
      (*iter1)->edRemoveInputPort(*iter2);
  std::list<InputDataStreamPort *> inputDS=node->getListOfInputDataStreamPort();
  for(std::list<OutputDataStreamPort *>::iterator iter3=_listOfOutputDataStreamPort.begin();iter3!=_listOfOutputDataStreamPort.end();iter3++)
    for(std::list<InputDataStreamPort *>::iterator iter4=inputDS.begin();iter4!=inputDS.end();iter4++)
      (*iter3)->edRemoveInputDataStreamPort(*iter4);
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
