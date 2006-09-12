#include "Loop.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"
#include "TypeCheckerDataStream.hxx"

using namespace YACS::ENGINE;

DFToDSForLoop::DFToDSForLoop(Loop *loop, const std::string& name, YACS::DynType type):ElementaryNode(""),_nbOfTimeUsed(1)
{
  _name="DF2DS For "; _name+=loop->getName(); _name+=" representing port "; _name+=name;
  _father=loop;
  _listOfInputPort.push_back(new InputPort("",this,type));
  _listOfOutputDataStreamPort.push_back(new OutputDataStreamPort("",this,Loop::MappingDF2DS(type)));
}

DFToDSForLoop::~DFToDSForLoop()
{
}

InputPort *DFToDSForLoop::getInputPort(const std::string& name) const throw(Exception)
{
  return (InputPort *) &_listOfInputPort.back();
}

OutputDataStreamPort *DFToDSForLoop::getOutputDataStreamPort(const std::string& name) const throw(Exception)
{
  return (OutputDataStreamPort *) &_listOfOutputDataStreamPort.back();
}

void DFToDSForLoop::execute()
{
  //TO IMPLEMENT
}

DSToDFForLoop::DSToDFForLoop(Loop *loop, const std::string& name, YACS::StreamType type):ElementaryNode(""),_nbOfTimeUsed(1)
{
  _name="DS2DF For "; _name+=loop->getName(); _name+=" representing port "; _name+=name;
  _father=loop;
  _listOfOutputPort.push_back(new OutputPort("",this,Loop::MappingDS2DF(type)));
  _listOfInputDataStreamPort.push_back(new InputDataStreamPort("",this,type));
}

DSToDFForLoop::~DSToDFForLoop()
{
}

OutputPort *DSToDFForLoop::getOutputPort(const std::string& name) const throw(Exception)
{
  return (OutputPort *) &_listOfOutputPort.back();
}

InputDataStreamPort *DSToDFForLoop::getInputDataStreamPort(const std::string& name) const throw(Exception)
{
  return (InputDataStreamPort *) &_listOfInputDataStreamPort.back();
}

void DSToDFForLoop::execute()
{
  //TO IMPLEMENT
}

Loop::Loop(const std::string& name):ComposedNode(name),_node(0)
{
}

Loop::~Loop()
{
  delete _node;
}

void Loop::edSetNode(Node *node)
{
  delete _node;
  _node=node;
}

void Loop::edAddExtraInputPort(const std::string& inputPortName, YACS::DynType type) throw(Exception)
{
  edAddPort<InputPort,YACS::DynType>(inputPortName,_listOfExtraInputPort,type);
}

void Loop::edRemoveExtraInputPort(InputPort *inputPort)
{
  edRemovePortTypedFromList<InputPort>(inputPort,_listOfExtraInputPort);
}

YACS::StreamType Loop::MappingDF2DS(YACS::DynType type) throw(Exception)
{
  switch(type)
    {
      case Double:
	return SDouble;
    }
  std::string what("Loop::MappingDF2DS : unable to perform DataFlow to DataStream traduction for dataflow type ");
  what+=Data::edGetTypeInPrintableForm(type);
  throw Exception(what);
}

YACS::DynType Loop::MappingDS2DF(YACS::StreamType type) throw(Exception)
{
  switch(type)
    {
      case SDouble:
	return Double;
    }
  std::string what("Loop::MappingDS2DF : unable to perform DataStream to DataFlow traduction for datastream type ");
  what+=TypeCheckerDataStream::edGetTypeInPrintableForm(type);
  throw Exception(what);
}

InPort *Loop::buildDelegateOf(InPort *port, const std::list<ComposedNode *>& pointsOfView)
{
  std::string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=InputPort::NAME)
    return port;
  else
    if(!isNecessaryToBuildSpecificDelegateDF2DS(pointsOfView))
      return port;
  InputPort *portCasted=(InputPort *)port;
  std::list<DSToDFForLoop>::iterator iter;
  //Determinig if a DSToDFForLoop node has already been created for delegation of 'port'
  for(iter=_inputsTraducer.begin();iter!=_inputsTraducer.end();iter++)
    if((*iter).getOutputPort("")->isAlreadyInList(portCasted))
      break;
  if(iter==_inputsTraducer.end())
    {//first time that 'port' is delegated on higher level
      _inputsTraducer.push_back(DSToDFForLoop(this,portCasted->getName(),Loop::MappingDF2DS(portCasted->edGetType())));
      iter=_inputsTraducer.end(); iter--;
      (*iter).getOutputPort("")->edAddInputPort(portCasted);
      //WARNING control flow has to be added
      (*iter).getOutGate()->edAddInGate(portCasted->getNode()->getInGate());//WARNING HERE MAYBE HAS TO BE IMPROVED - SEPARATE COUNTERS
    }
  else
    (*iter).loopHasOneMoreRef();
  return (*iter).getInputDataStreamPort("");
}

OutPort *Loop::buildDelegateOf(OutPort *port, const std::list<ComposedNode *>& pointsOfView)
{
  std::string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=OutputPort::NAME)
    return port;
  else
    if(!isNecessaryToBuildSpecificDelegateDF2DS(pointsOfView))
      return port;
  OutputPort *portCasted=(OutputPort *)port;
  std::list<DFToDSForLoop>::iterator iter;
  //Determinig if a DFToDSForLoop node has already been created for delegation of 'port'
  for(iter=_outputsTraducer.begin();iter!=_outputsTraducer.end();iter++)
    if(portCasted->isAlreadyInList((*iter).getInputPort("")))
      break;
  if(iter==_outputsTraducer.end())
    {//first time that 'port' is delegated on higher level
      _outputsTraducer.push_back(DFToDSForLoop(this,portCasted->getName(),portCasted->edGetType()));
      iter=_outputsTraducer.end(); iter--;
      portCasted->edAddInputPort((*iter).getInputPort(""));
      //WARNING control flow has to be added
      portCasted->getNode()->getOutGate()->edAddInGate((*iter).getInGate());//WARNING HERE MAYBE HAS TO BE IMPROVED - SEPARATE COUNTERS
    }
  else
    (*iter).loopHasOneMoreRef();
  return (*iter).getOutputDataStreamPort("");
}

InPort *Loop::getDelegateOf(InPort *port, const std::list<ComposedNode *>& pointsOfView) throw(Exception)
{
  std::string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=InputPort::NAME)
    return port;
  else
    if(!isNecessaryToBuildSpecificDelegateDF2DS(pointsOfView))
      return port;
  InputPort *portCasted=(InputPort *)port;
  std::list<DSToDFForLoop>::iterator iter;
  for(iter=_inputsTraducer.begin();iter!=_inputsTraducer.end();iter++)
    if((*iter).getOutputPort("")->isAlreadyInList(portCasted))
      break;
  if(iter==_inputsTraducer.end())
    {
      std::string what("Loop::getDelegateOf Port with name "); what+=portCasted->getName(); what+=" not exported by loop "; what+=_name; 
      throw Exception(what);
    }
  else
    return (*iter).getInputDataStreamPort("");
}

OutPort *Loop::getDelegateOf(OutPort *port, const std::list<ComposedNode *>& pointsOfView) throw(Exception)
{
  std::string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=OutputPort::NAME)
    return port;
  else
    if(!isNecessaryToBuildSpecificDelegateDF2DS(pointsOfView))
      return port;
  OutputPort *portCasted=(OutputPort *)port;
  std::list<DFToDSForLoop>::iterator iter;
  for(iter=_outputsTraducer.begin();iter!=_outputsTraducer.end();iter++)
    if(portCasted->isAlreadyInList((*iter).getInputPort("")))
      break;
  if(iter==_outputsTraducer.end())
    {
      std::string what("Loop::getDelegateOf Port with name "); what+=portCasted->getName(); what+=" not exported by loop "; what+=_name; 
      throw Exception(what);
    }
  else
    return (*iter).getOutputDataStreamPort("");
}

InPort *Loop::releaseDelegateOf(InPort *port, const std::list<ComposedNode *>& pointsOfView) throw(Exception)
{
  std::string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=InputPort::NAME)
    return port;
  else
    if(!isNecessaryToBuildSpecificDelegateDF2DS(pointsOfView))
      return port;
  InputPort *portCasted=(InputPort *)port;
  std::list<DSToDFForLoop>::iterator iter;
  for(iter=_inputsTraducer.begin();iter!=_inputsTraducer.end();iter++)
    if((*iter).getOutputPort("")->isAlreadyInList(portCasted))
      break;
  if(iter==_inputsTraducer.end())
    {
      std::string what("Loop::releaseDelegateOf Port with name "); what+=portCasted->getName(); what+=" not exported by loop "; what+=_name; 
      throw Exception(what);
    }
  else
    {
      InPort *ret=(*iter).getInputDataStreamPort("");
      if((*iter).loopHasOneLessRef())
	_inputsTraducer.erase(iter);
      return ret;
    }
}

OutPort *Loop::releaseDelegateOf(OutPort *port, const std::list<ComposedNode *>& pointsOfView) throw(Exception)
{
    std::string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=OutputPort::NAME)
    return port;
  else
    if(!isNecessaryToBuildSpecificDelegateDF2DS(pointsOfView))
      return port;
  OutputPort *portCasted=(OutputPort *)port;
  std::list<DFToDSForLoop>::iterator iter;
  for(iter=_outputsTraducer.begin();iter!=_outputsTraducer.end();iter++)
    if(portCasted->isAlreadyInList((*iter).getInputPort("")))
      break;
  if(iter==_outputsTraducer.end())
    {
      std::string what("Loop::releaseDelegateOf Port with name "); what+=portCasted->getName(); what+=" not exported by loop "; what+=_name; 
      throw Exception(what);
    }
  else
    {
      OutPort *ret=(*iter).getOutputDataStreamPort("");
      if((*iter).loopHasOneLessRef())
	_outputsTraducer.erase(iter);
      return ret;
    }
}

void Loop::checkNoCyclePassingThrough(Node *node) throw(Exception)
{
  throw Exception("Loop::checkNoCyclePassingThrough : Internal error occured");
}

/**
 * @ note : States if a DF port must be considered on an upper level in hierarchy as a DS port or not from 'pointsOfView' observers.
 * @ return : 
 *            - True : a traduction DF->DS has to be done
 *            - False : no traduction needed
 */
bool Loop::isNecessaryToBuildSpecificDelegateDF2DS(const std::list<ComposedNode *>& pointsOfView)
{
  bool ret=false;
  for(std::list<ComposedNode *>::const_iterator iter=pointsOfView.begin();iter!=pointsOfView.end() && !ret;iter++)
    ret=(*iter)->isRepeatedUnpredictablySeveralTimes();
  return ret;
}
