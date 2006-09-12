#include "OutputPort.hxx"
#include "InputPort.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char OutputPort::NAME[]="OutputPort";

OutputPort::OutputPort(const std::string& name, Node *node, DynType type):DataFlowPort(name,node,type),OutPort(node),Port(node)
{
}

std::string OutputPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

Data OutputPort::foGet() const
{
  return _data;
}

void OutputPort::exInit()
{
  _data.exInit();
}

void OutputPort::exPut(Data data) throw(ConversionException)
{
  _data=data;
  for(list<InputPort *>::iterator iter=_listOfInputPort.begin();iter!=_listOfInputPort.end();iter++)
    (*iter)->exAccept(data);
}

bool OutputPort::edAddInputPort(InputPort *inputPort) throw(ConversionException)
{
  if(!Data::areStaticallyCompatible(edGetType(),inputPort->edGetType()))
    throw ConversionException(Data::edGetTypeInPrintableForm(edGetType()),Data::edGetTypeInPrintableForm(inputPort->edGetType()));
  if(!isAlreadyInList(inputPort))
    {
      _listOfInputPort.push_back(inputPort);
      inputPort->edNotifyReferenced();
      return true;
    }
  else
    return false;
}

list<InputPort *> OutputPort::edListInputPort()
{
  return _listOfInputPort;
}

void OutputPort::edRemoveInputPort(InputPort *inputPort) throw(Exception)
{
  if(isAlreadyInList(inputPort))
    _listOfInputPort.remove(inputPort);
  else
    throw Exception("OutputPort::edRemoveInputPort : link does not exist, unable to remove it");
}

OutputPort::~OutputPort()
{
}

bool OutputPort::isAlreadyInList(InputPort *inputPort) const
{
  bool ret=false;
  for(list<InputPort *>::const_iterator iter=_listOfInputPort.begin();iter!=_listOfInputPort.end() && !ret;iter++)
    if((*iter)==inputPort)
      ret=true;
  return ret;
}

bool OutputPort::addInPort(InPort *inPort) throw(Exception)
{
  if(inPort->getNameOfTypeOfCurrentInstance()!=InputPort::NAME)
    {
      std::string what="not compatible type of port requested during building of link FROM ";
      what+=NAME; what+=" TO "; what+=inPort->getNameOfTypeOfCurrentInstance();
      throw Exception(what);
    }
  return edAddInputPort(static_cast<InputPort*>(inPort));
}

void OutputPort::removeInPort(InPort *inPort) throw(Exception)
{
  if(inPort->getNameOfTypeOfCurrentInstance()!=InputPort::NAME)
    {
      std::string what="not compatible type of port requested during destruction of for link FROM ";
      what+=NAME; what+=" TO "; what+=inPort->getNameOfTypeOfCurrentInstance();
      throw Exception(what);
    }
  edRemoveInputPort(static_cast<InputPort*>(inPort));
}

bool OutputPort::isLinked()
{
  return _listOfInputPort.empty();
}
