#include "Loop.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"
#include "Runtime.hxx"

//#include "TypeCheckerDataStream.hxx"

using namespace YACS::ENGINE;
using namespace std;

DFToDSForLoop::DFToDSForLoop(Loop *loop, const string& name, TypeCode* type):ElementaryNode(""),_nbOfTimeUsed(1)
{
  _name="DF2DS For "; _name+=loop->getName(); _name+=" representing port "; _name+=name;
  _father=loop;
//   _setOfInputPort.insert(new InputPort("",this,type)); // probleme si constructeur protege, a voir
  _setOfOutputDataStreamPort.insert(new OutputDataStreamPort("",this,Loop::MappingDF2DS(type)));
}

DFToDSForLoop::~DFToDSForLoop()
{
}

InputPort *DFToDSForLoop::getInputPort(const string& name) const throw(Exception)
{
  set<InputPort *>::iterator it =_setOfInputPort.begin();
  return (*it);
}

OutputDataStreamPort *DFToDSForLoop::getOutputDataStreamPort(const string& name) const throw(Exception)
{
  set<OutputDataStreamPort *> ::iterator it =_setOfOutputDataStreamPort.begin();
  return (*it);
}

void DFToDSForLoop::execute()
{
  //TO IMPLEMENT
}

DSToDFForLoop::DSToDFForLoop(Loop *loop, const string& name, TypeCode* type):ElementaryNode(""),_nbOfTimeUsed(1)
{
  _name="DS2DF For "; _name+=loop->getName(); _name+=" representing port "; _name+=name;
  _father=loop;
  //  _setOfOutputPort.insert(new OutputPort("",this,Loop::MappingDS2DF(type))); // probleme si constructeur protege, a voir
  _setOfInputDataStreamPort.insert(new InputDataStreamPort("",this,type));
}

DSToDFForLoop::~DSToDFForLoop()
{
}

OutputPort *DSToDFForLoop::getOutputPort(const string& name) const throw(Exception)
{
  set<OutputPort *>::iterator it = _setOfOutputPort.begin();
  return (*it);
}

InputDataStreamPort *DSToDFForLoop::getInputDataStreamPort(const string& name) const throw(Exception)
{
  set<InputDataStreamPort *>::iterator it = _setOfInputDataStreamPort.begin();
  return (*it);
}

void DSToDFForLoop::execute()
{
  //TO IMPLEMENT
}

Loop::Loop(const string& name):ComposedNode(name),_node(0)
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

void Loop::edAddExtraInputPort(const string& inputPortName, TypeCode* type) throw(Exception)
{
  InputPort *ret = 0;
  if (edCheckAddPort<InputPort, TypeCode*>(inputPortName,_setOfInputPort,type))
    {
      //InputPort *ret=edAddPort<InputPort, TypeCode*>(inputPortName,_setOfInputPort,type);
      ret = getRuntime()->createInputPort(inputPortName, _implementation, this, type);
      _setOfExtraInputPort.insert(ret);
    }
      //edAddPort<InputPort,TypeCode*>(inputPortName,_setOfExtraInputPort,type);
}

void Loop::edRemoveExtraInputPort(InputPort *inputPort)
{
  edRemovePortTypedFromSet<InputPort>(inputPort,_setOfExtraInputPort);
}

TypeCode* Loop::MappingDF2DS(TypeCode* type) throw(Exception)
{
//   switch(type)
//     {
//       case Double:
// 	return SDouble;
//     }
  string what("Loop::MappingDF2DS : unable to perform DataFlow to DataStream traduction for dataflow type ");
  //what+=Data::edGetTypeInPrintableForm(type);
  throw Exception(what);
}

TypeCode* Loop::MappingDS2DF(TypeCode* type) throw(Exception)
{
//   switch(type)
//     {
//       case SDouble:
// 	return Double;
//     }
  string what("Loop::MappingDS2DF : unable to perform DataStream to DataFlow traduction for datastream type ");
  //what+=TypeCheckerDataStream::edGetTypeInPrintableForm(type);
  throw Exception(what);
}

InPort *Loop::buildDelegateOf(InPort *port, const set<ComposedNode *>& pointsOfView)
{
  string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=InputPort::NAME)
    return port;
  else
    if(!isNecessaryToBuildSpecificDelegateDF2DS(pointsOfView))
      return port;
  InputPort *portCasted=(InputPort *)port;
  set<DSToDFForLoop*>::iterator iter;
  //Determinig if a DSToDFForLoop node has already been created for delegation of 'port'
  for(iter=_inputsTraducer.begin();iter!=_inputsTraducer.end();iter++)
    if((*iter)->getOutputPort("")->isAlreadyInSet(portCasted))
      break;
  if(iter==_inputsTraducer.end())
    {//first time that 'port' is delegated on higher level
      _inputsTraducer.insert(new DSToDFForLoop(this,portCasted->getName(),Loop::MappingDF2DS(portCasted->edGetType())));
      iter=_inputsTraducer.end(); iter--;
      (*iter)->getOutputPort("")->edAddInputPort(portCasted);
      //WARNING control flow has to be added
      (*iter)->getOutGate()->edAddInGate(portCasted->getNode()->getInGate());//WARNING HERE MAYBE HAS TO BE IMPROVED - SEPARATE COUNTERS
    }
  else
    (*iter)->loopHasOneMoreRef();
  return (*iter)->getInputDataStreamPort("");
}

OutPort *Loop::buildDelegateOf(OutPort *port, const set<ComposedNode *>& pointsOfView)
{
  string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=OutputPort::NAME)
    return port;
  else
    if(!isNecessaryToBuildSpecificDelegateDF2DS(pointsOfView))
      return port;
  OutputPort *portCasted=(OutputPort *)port;
  set<DFToDSForLoop*>::iterator iter;
  //Determinig if a DFToDSForLoop node has already been created for delegation of 'port'
  for(iter=_outputsTraducer.begin();iter!=_outputsTraducer.end();iter++)
    if(portCasted->isAlreadyInSet((*iter)->getInputPort("")))
      break;
  if(iter==_outputsTraducer.end())
    {//first time that 'port' is delegated on higher level
      _outputsTraducer.insert(new DFToDSForLoop(this,portCasted->getName(),portCasted->edGetType()));
      iter=_outputsTraducer.end(); iter--;
      portCasted->edAddInputPort((*iter)->getInputPort(""));
      //WARNING control flow has to be added
      portCasted->getNode()->getOutGate()->edAddInGate((*iter)->getInGate());//WARNING HERE MAYBE HAS TO BE IMPROVED - SEPARATE COUNTERS
    }
  else
    (*iter)->loopHasOneMoreRef();
  return (*iter)->getOutputDataStreamPort("");
}

InPort *Loop::getDelegateOf(InPort *port, const set<ComposedNode *>& pointsOfView) throw(Exception)
{
  string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=InputPort::NAME)
    return port;
  else
    if(!isNecessaryToBuildSpecificDelegateDF2DS(pointsOfView))
      return port;
  InputPort *portCasted=(InputPort *)port;
  set<DSToDFForLoop*>::iterator iter;
  for(iter=_inputsTraducer.begin();iter!=_inputsTraducer.end();iter++)
    if((*iter)->getOutputPort("")->isAlreadyInSet(portCasted))
      break;
  if(iter==_inputsTraducer.end())
    {
      string what("Loop::getDelegateOf Port with name "); what+=portCasted->getName(); what+=" not exported by loop "; what+=_name; 
      throw Exception(what);
    }
  else
    return (*iter)->getInputDataStreamPort("");
}

OutPort *Loop::getDelegateOf(OutPort *port, const set<ComposedNode *>& pointsOfView) throw(Exception)
{
  string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=OutputPort::NAME)
    return port;
  else
    if(!isNecessaryToBuildSpecificDelegateDF2DS(pointsOfView))
      return port;
  OutputPort *portCasted=(OutputPort *)port;
  set<DFToDSForLoop*>::iterator iter;
  for(iter=_outputsTraducer.begin();iter!=_outputsTraducer.end();iter++)
    if(portCasted->isAlreadyInSet((*iter)->getInputPort("")))
      break;
  if(iter==_outputsTraducer.end())
    {
      string what("Loop::getDelegateOf Port with name "); what+=portCasted->getName(); what+=" not exported by loop "; what+=_name; 
      throw Exception(what);
    }
  else
    return (*iter)->getOutputDataStreamPort("");
}

InPort *Loop::releaseDelegateOf(InPort *port, const set<ComposedNode *>& pointsOfView) throw(Exception)
{
  string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=InputPort::NAME)
    return port;
  else
    if(!isNecessaryToBuildSpecificDelegateDF2DS(pointsOfView))
      return port;
  InputPort *portCasted=(InputPort *)port;
  set<DSToDFForLoop*>::iterator iter;
  for(iter=_inputsTraducer.begin();iter!=_inputsTraducer.end();iter++)
    if((*iter)->getOutputPort("")->isAlreadyInSet(portCasted))
      break;
  if(iter==_inputsTraducer.end())
    {
      string what("Loop::releaseDelegateOf Port with name "); what+=portCasted->getName(); what+=" not exported by loop "; what+=_name; 
      throw Exception(what);
    }
  else
    {
      InPort *ret=(*iter)->getInputDataStreamPort("");
      if((*iter)->loopHasOneLessRef())
	_inputsTraducer.erase(iter);
      return ret;
    }
}

OutPort *Loop::releaseDelegateOf(OutPort *port, const set<ComposedNode *>& pointsOfView) throw(Exception)
{
    string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=OutputPort::NAME)
    return port;
  else
    if(!isNecessaryToBuildSpecificDelegateDF2DS(pointsOfView))
      return port;
  OutputPort *portCasted=(OutputPort *)port;
  set<DFToDSForLoop*>::iterator iter;
  for(iter=_outputsTraducer.begin();iter!=_outputsTraducer.end();iter++)
    if(portCasted->isAlreadyInSet((*iter)->getInputPort("")))
      break;
  if(iter==_outputsTraducer.end())
    {
      string what("Loop::releaseDelegateOf Port with name "); what+=portCasted->getName(); what+=" not exported by loop "; what+=_name; 
      throw Exception(what);
    }
  else
    {
      OutPort *ret=(*iter)->getOutputDataStreamPort("");
      if((*iter)->loopHasOneLessRef())
	_outputsTraducer.erase(iter);
      return ret;
    }
}

void Loop::checkNoCyclePassingThrough(Node *node) throw(Exception)
{
  throw Exception("Loop::checkNoCyclePassingThrough : Internal error occured");
}

/**
 * @note : States if a DF port must be considered on an upper level in hierarchy as a DS port or not from 'pointsOfView' observers.
 * @return : 
 *            - True : a traduction DF->DS has to be done
 *            - False : no traduction needed
 */
bool Loop::isNecessaryToBuildSpecificDelegateDF2DS(const set<ComposedNode *>& pointsOfView)
{
  bool ret=false;
  for(set<ComposedNode *>::const_iterator iter=pointsOfView.begin();iter!=pointsOfView.end() && !ret;iter++)
    ret=(*iter)->isRepeatedUnpredictablySeveralTimes();
  return ret;
}
